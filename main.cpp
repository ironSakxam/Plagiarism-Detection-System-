#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "include/FileManager.hpp"
#include "include/Analyzer.hpp"
#include "include/UI.hpp"
#include "include/Document.hpp"
#include "include/FileDialog.hpp"
#include "raylib.h"

using namespace pd;

struct FileState {
    std::filesystem::path path;
    std::filesystem::file_time_type lastWriteTime;
    
    bool operator==(const FileState& other) const {
        return path == other.path && lastWriteTime == other.lastWriteTime;
    }
    
    bool operator!=(const FileState& other) const {
        return !(*this == other);
    }
};

std::vector<FileState> getCurrentStates(const std::vector<std::filesystem::path>& paths) {
    std::vector<FileState> states;
    for (const auto& p : paths) {
        try {
            if (std::filesystem::exists(p)) {
                states.push_back({p, std::filesystem::last_write_time(p)});
            }
        } catch (...) {
            // Ignore temporary access lock errors during compiler/editor saves
        }
    }
    return states;
}

void updateComparisonList(const Document& sourceDoc, 
                          const std::vector<Document>& database, 
                          bool skipSelf, 
                          Analyzer& analyzer, 
                          std::vector<ComparisonItem>& outItems) {
    outItems.clear();
    for (const auto& dbDoc : database) {
        if (skipSelf && dbDoc.path.filename() == sourceDoc.path.filename()) {
            continue;
        }
        double similarity = analyzer.computeJaccard(sourceDoc, dbDoc);
        ComparisonItem item;
        item.path = dbDoc.path;
        item.similarity = similarity;
        item.tokenCount = dbDoc.tokens.size();
        outItems.push_back(item);
    }
    
    std::sort(outItems.begin(), outItems.end(), [](const ComparisonItem& a, const ComparisonItem& b) {
        return a.similarity > b.similarity;
    });
}

int main() {
    std::cout << "=====================================\n"
              << "  Plagiarism Detection System v2.1\n"
              << "=====================================\n\n";
    std::cout << "Initializing modules...\n" << std::endl;
    
    FileManager fileManager;
    Analyzer analyzer;
    UI ui;
    
    const int WINDOW_WIDTH = 1600;
    const int WINDOW_HEIGHT = 1000;
    if (!ui.initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Plagiarism Detection System")) {
        std::cerr << "Error: Failed to initialize UI window." << std::endl;
        return -1;
    }
    
    std::filesystem::path dataDir = "data";
    std::vector<std::filesystem::path> files = fileManager.scanDirectory(dataDir);
    
    std::vector<Document> dbDocs;
    for (const auto& file : files) {
        Document doc;
        doc.path = file;
        doc.rawText = fileManager.readFile(file);
        if (!doc.rawText.empty()) {
            analyzer.preprocess(doc);
            dbDocs.push_back(doc);
        }
    }
    
    std::vector<FileState> cachedStates = getCurrentStates(files);
    std::cout << "[✓] Loaded " << dbDocs.size() << " database files.\n\n";
    
    bool hasUploaded = false;
    Document uploadedDoc;
    int selectedIndex = 0;
    std::vector<ComparisonItem> displayItems;
    
    std::string activeFilePath = "";
    int activeTokens = 0;
    if (dbDocs.size() >= 2) {
        activeFilePath = dbDocs[0].path.string();
        activeTokens = dbDocs[0].tokens.size();
        updateComparisonList(dbDocs[0], dbDocs, true, analyzer, displayItems);
    } else if (dbDocs.size() == 1) {
        activeFilePath = dbDocs[0].path.string();
        activeTokens = dbDocs[0].tokens.size();
    }
    
    auto reloadDatabase = [&]() {
        files = fileManager.scanDirectory(dataDir);
        dbDocs.clear();
        for (const auto& file : files) {
            Document doc;
            doc.path = file;
            doc.rawText = fileManager.readFile(file);
            if (!doc.rawText.empty()) {
                analyzer.preprocess(doc);
                dbDocs.push_back(doc);
            }
        }
        cachedStates = getCurrentStates(files);
        
        if (hasUploaded) {
            updateComparisonList(uploadedDoc, dbDocs, false, analyzer, displayItems);
            if (selectedIndex >= (int)displayItems.size()) {
                selectedIndex = std::max(0, (int)displayItems.size() - 1);
            }
        } else {
            if (dbDocs.size() >= 2) {
                bool activeExists = false;
                for (const auto& doc : dbDocs) {
                    if (doc.path.string() == activeFilePath) {
                        activeExists = true;
                        activeTokens = doc.tokens.size();
                        updateComparisonList(doc, dbDocs, true, analyzer, displayItems);
                        break;
                    }
                }
                if (!activeExists) {
                    activeFilePath = dbDocs[0].path.string();
                    activeTokens = dbDocs[0].tokens.size();
                    updateComparisonList(dbDocs[0], dbDocs, true, analyzer, displayItems);
                    selectedIndex = 0;
                }
                if (selectedIndex >= (int)displayItems.size()) {
                    selectedIndex = std::max(0, (int)displayItems.size() - 1);
                }
            } else if (dbDocs.size() == 1) {
                activeFilePath = dbDocs[0].path.string();
                activeTokens = dbDocs[0].tokens.size();
                displayItems.clear();
                selectedIndex = 0;
            } else {
                activeFilePath = "No files in data/";
                activeTokens = 0;
                displayItems.clear();
                selectedIndex = 0;
            }
        }
    };
    
    std::cout << "Starting interactive UI loop... Press ESC in the GUI window to exit.\n";
    std::cout << "=====================================\n\n";
    
    int frameCount = 0;
    
    while (!WindowShouldClose()) {
        frameCount++;
        
        // Check for database changes every 60 frames (1 second)
        if (frameCount >= 60) {
            frameCount = 0;
            std::vector<std::filesystem::path> currentFiles = fileManager.scanDirectory(dataDir);
            std::vector<FileState> currentStates = getCurrentStates(currentFiles);
            
            if (currentStates != cachedStates) {
                std::cout << "Database directory change detected. Reloading files...\n";
                reloadDatabase();
                std::cout << "[✓] Database reloaded. Total files: " << dbDocs.size() << "\n";
            }
        }
        
        int newSelectedIndex = selectedIndex;
        bool resetRequested = false;
        bool importRequested = false;
        bool deleteRequested = false;
        
        std::string uploadedPath = ui.render(
            hasUploaded,
            activeFilePath,
            activeTokens,
            displayItems,
            selectedIndex,
            newSelectedIndex,
            resetRequested,
            importRequested,
            deleteRequested
        );
        
        if (newSelectedIndex != selectedIndex) {
            selectedIndex = newSelectedIndex;
            if (selectedIndex >= 0 && selectedIndex < (int)displayItems.size()) {
                std::cout << "Selected match display changed to index: " << selectedIndex 
                          << " (" << displayItems[selectedIndex].path.filename().string() << ")\n";
            }
        }
        
        if (resetRequested && hasUploaded) {
            hasUploaded = false;
            selectedIndex = 0;
            if (dbDocs.size() >= 2) {
                activeFilePath = dbDocs[0].path.string();
                activeTokens = dbDocs[0].tokens.size();
                updateComparisonList(dbDocs[0], dbDocs, true, analyzer, displayItems);
            } else if (dbDocs.size() == 1) {
                activeFilePath = dbDocs[0].path.string();
                activeTokens = dbDocs[0].tokens.size();
                displayItems.clear();
            } else {
                activeFilePath = "No files in data/";
                activeTokens = 0;
                displayItems.clear();
            }
            std::cout << "Dashboard reset to default compare.\n";
        }
        
        if (!uploadedPath.empty()) {
            std::cout << "File uploaded from file dialog: " << uploadedPath << "\n";
            std::string content = fileManager.readFile(uploadedPath);
            if (!content.empty()) {
                uploadedDoc.path = uploadedPath;
                uploadedDoc.rawText = content;
                analyzer.preprocess(uploadedDoc);
                
                hasUploaded = true;
                selectedIndex = 0;
                activeFilePath = uploadedPath;
                activeTokens = uploadedDoc.tokens.size();
                
                updateComparisonList(uploadedDoc, dbDocs, false, analyzer, displayItems);
                std::cout << "Uploaded file preprocessed and matched against database.\n";
            } else {
                std::cerr << "Error: Selected file is empty or unreadable.\n";
            }
        }
        
        if (importRequested) {
            std::string fileToImport = openFileDialog();
            if (!fileToImport.empty()) {
                std::filesystem::path srcPath(fileToImport);
                std::filesystem::path destPath = dataDir / srcPath.filename();
                try {
                    std::filesystem::copy_file(srcPath, destPath, std::filesystem::copy_options::overwrite_existing);
                    std::cout << "Imported file to database: " << destPath << std::endl;
                    reloadDatabase();
                } catch (const std::exception& e) {
                    std::cerr << "Failed to import file: " << e.what() << std::endl;
                }
            }
        }
        
        if (deleteRequested) {
            if (selectedIndex >= 0 && selectedIndex < (int)displayItems.size()) {
                std::filesystem::path fileToDelete = displayItems[selectedIndex].path;
                try {
                    if (std::filesystem::remove(fileToDelete)) {
                        std::cout << "Deleted file: " << fileToDelete << std::endl;
                        reloadDatabase();
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Failed to delete file: " << e.what() << std::endl;
                }
            }
        }
    }
    
    std::cout << "\nShutting down...\n";
    ui.shutdown();
    std::cout << "[✓] Clean shutdown complete.\n";
    
    return 0;
}
