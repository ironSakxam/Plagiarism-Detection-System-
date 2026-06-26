// ==============================================================================
// main.cpp - Application Entry Point and Controller
// ==============================================================================
// PURPOSE: Wire all modules together and orchestrate the application flow.
// Preloads database documents, computes similarity sets, manages upload events,
// watches the database directory for real-time additions/modifications,
// and coordinates the main interactive rendering loop.
// ==============================================================================

#include <iostream>              // For console output
#include <string>                // For std::string
#include <vector>                // For std::vector
#include <algorithm>             // For std::sort, std::max
#include <filesystem>            // For directory operations
#include "include/FileManager.hpp"  // File I/O module
#include "include/Analyzer.hpp"     // Plagiarism analysis module
#include "include/UI.hpp"           // Graphics/UI module
#include "include/Document.hpp"     // Document data structure
#include "include/FileDialog.hpp"   // File dialog module
#include "raylib.h"                 // Raylib graphics library

using namespace pd;

// Struct to monitor directory files and hot-reload modified or new files
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

// Retrieve states for file modification tracking
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

// Helper to update and rank the comparison list
void updateComparisonList(const Document& sourceDoc, 
                          const std::vector<Document>& database, 
                          bool skipSelf, 
                          Analyzer& analyzer, 
                          std::vector<ComparisonItem>& outItems) {
    outItems.clear();
    for (const auto& dbDoc : database) {
        // Skip comparing a file against itself in default mode
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
    
    // Sort comparisons in descending order of similarity score
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
    
    // Initialize UI Window (width: 1600, height: 1000)
    const int WINDOW_WIDTH = 1600;
    const int WINDOW_HEIGHT = 1000;
    if (!ui.initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Code Plagiarism Guard")) {
        std::cerr << "Error: Failed to initialize UI window." << std::endl;
        return -1;
    }
    
    // Scan and load files from the database directory (data/)
    std::cout << "Scanning database directory...\n";
    std::filesystem::path dataDir = "data";
    std::vector<std::filesystem::path> files = fileManager.scanDirectory(dataDir);
    
    // Initial load of all documents in the database
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
    
    // Establish starting baseline state for database hot-reloading
    std::vector<FileState> cachedStates = getCurrentStates(files);
    std::cout << "[✓] Loaded " << dbDocs.size() << " database files.\n\n";
    
    // State variables for interactive navigation
    bool hasUploaded = false;
    Document uploadedDoc;
    int selectedIndex = 0;
    std::vector<ComparisonItem> displayItems;
    
    // Initialise default comparison mode (first document against the rest, if we have enough docs)
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
    
    // Lambda helper to reload database and update current comparison results
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
                // Keep the active file if it still exists, otherwise reset to index 0
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
        
        // --- DIRECTORY WATCHING: CHECK FOR DATABASE CHANGES EVERY 60 FRAMES (1 SECOND) ---
        if (frameCount >= 60) {
            frameCount = 0;
            std::vector<std::filesystem::path> currentFiles = fileManager.scanDirectory(dataDir);
            std::vector<FileState> currentStates = getCurrentStates(currentFiles);
            
            // Reload if files were added, deleted, or updated
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
        
        // Render current dashboard frame and check for user event triggers
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
        
        // Handle database card selection change
        if (newSelectedIndex != selectedIndex) {
            selectedIndex = newSelectedIndex;
            if (selectedIndex >= 0 && selectedIndex < (int)displayItems.size()) {
                std::cout << "Selected match display changed to index: " << selectedIndex 
                          << " (" << displayItems[selectedIndex].path.filename().string() << ")\n";
            }
        }
        
        // Handle reset dashboard request (back to default comparison mode)
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
        
        // Handle file upload selection from dialog
        if (!uploadedPath.empty()) {
            std::cout << "File uploaded from file dialog: " << uploadedPath << "\n";
            std::string content = fileManager.readFile(uploadedPath);
            if (!content.empty()) {
                uploadedDoc.path = uploadedPath;
                uploadedDoc.rawText = content;
                analyzer.preprocess(uploadedDoc);
                
                hasUploaded = true;
                selectedIndex = 0; // Default selection to top ranked similarity
                activeFilePath = uploadedPath;
                activeTokens = uploadedDoc.tokens.size();
                
                // Compare uploaded document against all currently loaded database docs
                updateComparisonList(uploadedDoc, dbDocs, false, analyzer, displayItems);
                std::cout << "Uploaded file preprocessed and matched against database.\n";
            } else {
                std::cerr << "Error: Selected file is empty or unreadable.\n";
            }
        }
        
        // Handle Import File to Database
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
        
        // Handle Delete File from Database
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
