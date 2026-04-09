#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

class Utils {
public:
    // Generate a unique commit ID using timestamp
    static std::string generateCommitId();

    // Get current timestamp as string
    static std::string getCurrentTime();

    // Read all lines from a file
    static std::vector<std::string> readFileLines(const fs::path& file);

    // Write lines to a file
    static void writeFileLines(const fs::path& file, const std::vector<std::string>& lines);

    // Check if file exists
    static bool fileExists(const fs::path& file);

    // Copy file from source to destination
    static void copyFile(const fs::path& from, const fs::path& to);

    // Get list of files in directory (recursive)
    static std::vector<std::string> getFilesInDir(const fs::path& dir);

    // Simple diff between two sets of lines
    static std::vector<std::string> diff(const std::vector<std::string>& oldLines, const std::vector<std::string>& newLines);

    // Check if file matches ignore pattern (simple implementation)
    static bool isIgnored(const std::string& filename, const std::vector<std::string>& ignorePatterns);
};

#endif // UTILS_H