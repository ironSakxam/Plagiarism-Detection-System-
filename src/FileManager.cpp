// ==============================================================================
// FileManager.cpp - File I/O Module
// ==============================================================================
// PURPOSE: Handle all file reading and directory scanning operations
// This module is responsible for reading source code files from disk
// and returning their contents as strings to the Analyzer
// ==============================================================================

#include "FileManager.hpp"  // Include the header file with function declarations
#include <fstream>          // For file input stream operations (reading files)
#include <sstream>          // For string stream operations
#include <iostream>         // For console output (error messages, debugging)

namespace pd {

// ==============================================================================
// FUNCTION: scanDirectory
// ==============================================================================
// PURPOSE: Scan a directory and return a list of all C++ source files
// INPUT:    dir = the directory path to scan (e.g., "data/")
// OUTPUT:   vector of file paths found in that directory
// LOGIC:
//   1. Check if directory exists
//   2. Iterate through all files in the directory
//   3. Filter for .cpp files (source code)
//   4. Return list of matching files
// ==============================================================================
std::vector<std::filesystem::path> FileManager::scanDirectory(
    const std::filesystem::path& dir)  // The directory to scan
{
    // Create an empty vector to store the found file paths
    // vector<path> = list of file paths
    std::vector<std::filesystem::path> files;
    
    // Check if the provided path actually exists and is a directory
    // filesystem::exists() returns true if path exists, false otherwise
    if (!std::filesystem::exists(dir)) {
        // If directory doesn't exist, print error and return empty list
        std::cerr << "Error: Directory '" << dir << "' does not exist." 
                  << std::endl;  // Print to error console
        return files;  // Return empty vector (no files found)
    }
    
    // Check if the path is actually a directory (not a file)
    // filesystem::is_directory() returns true if it's a directory
    if (!std::filesystem::is_directory(dir)) {
        // If it's not a directory, print error and return empty list
        std::cerr << "Error: '" << dir << "' is not a directory." 
                  << std::endl;
        return files;  // Return empty vector
    }
    
    // Iterate through ALL files in the directory
    // directory_iterator loops through each entry in the folder
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        // For each file/folder in the directory:
        
        // Check if current entry is a regular file (not a folder)
        // is_regular_file() returns true if it's a file
        if (entry.is_regular_file()) {
            // Get the file extension (e.g., ".cpp", ".txt")
            // path().extension() returns the file extension as a string
            std::string extension = entry.path().extension().string();
            
            // Check if file has .cpp extension (source code files)
            if (extension == ".cpp" || extension == ".txt") {
                // If it's a C++ file, add it to our list
                // push_back() adds the file path to the end of the vector
                files.push_back(entry.path());
            }
        }
    }
    

    
    // Return the vector containing all found file paths
    return files;
}

// ==============================================================================
// FUNCTION: readFile
// ==============================================================================
// PURPOSE: Read the entire contents of a file and return as a string
// INPUT:    path = the file path to read (e.g., "data/student1.cpp")
// OUTPUT:   string containing the entire file contents
// LOGIC:
//   1. Open the file
//   2. Check if file opened successfully
//   3. Read entire file content
//   4. Return the content
// ==============================================================================
std::string FileManager::readFile(const std::filesystem::path& path)  // File path to read
{
    // Create an empty string to store file contents
    std::string content;
    
    // Create an input file stream object
    // ifstream = input file stream (used to read from files)
    std::ifstream file(path);
    
    // Check if the file was successfully opened
    // The file stream is valid if it was opened without errors
    if (!file.is_open()) {
        // If file couldn't be opened, print error message
        std::cerr << "Error: Could not open file '" << path << "'." 
                  << std::endl;
        // Return empty string to indicate failure
        return content;  // Empty string = file not readable
    }
    
    // Create a string stream to accumulate file contents
    // stringstream = temporary storage for building the full file content
    std::stringstream buffer;
    
    // Read the entire file content using the << operator
    // This reads ALL content from file stream into the stringstream
    buffer << file.rdbuf();  // rdbuf() = read the entire raw buffer
    
    // Convert stringstream to regular string
    // str() = convert stringstream contents to std::string
    content = buffer.str();
    
    // Close the file manually
    // While it would close automatically at end of scope, explicit close is good practice
    file.close();
    
    // Print success message (for debugging)
    std::cout << "Successfully read file: " << path << std::endl;
    
    // Return the file content as a string
    return content;
}

} // namespace pd (end of namespace)

