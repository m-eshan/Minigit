#include "Utils.h"

std::string Utils::generateCommitId() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return std::to_string(millis);
}

std::string Utils::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::string time_str = std::ctime(&time_t_now);
    time_str.erase(time_str.find_last_not_of(" \n\r\t") + 1); // trim
    return time_str;
}

std::vector<std::string> Utils::readFileLines(const fs::path& file) {
    std::vector<std::string> lines;
    std::ifstream ifs(file);
    if (!ifs) return lines;
    std::string line;
    while (std::getline(ifs, line)) {
        lines.push_back(line);
    }
    return lines;
}

void Utils::writeFileLines(const fs::path& file, const std::vector<std::string>& lines) {
    std::ofstream ofs(file);
    for (const auto& line : lines) {
        ofs << line << '\n';
    }
}

bool Utils::fileExists(const fs::path& file) {
    return fs::exists(file) && fs::is_regular_file(file);
}

void Utils::copyFile(const fs::path& from, const fs::path& to) {
    if (from == to) {
        return;
    }
    fs::create_directories(to.parent_path());
    fs::copy_file(from, to, fs::copy_options::overwrite_existing);
}

std::vector<std::string> Utils::getFilesInDir(const fs::path& dir) {
    std::vector<std::string> files;
    if (!fs::exists(dir) || !fs::is_directory(dir)) return files;
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (fs::is_regular_file(entry)) {
            files.push_back(fs::relative(entry.path(), dir).string());
        }
    }
    return files;
}

std::vector<std::string> Utils::diff(const std::vector<std::string>& oldLines, const std::vector<std::string>& newLines) {
    std::vector<std::string> result;
    size_t maxLines = std::max(oldLines.size(), newLines.size());
    for (size_t i = 0; i < maxLines; ++i) {
        std::string oldLine = (i < oldLines.size()) ? oldLines[i] : "";
        std::string newLine = (i < newLines.size()) ? newLines[i] : "";
        if (oldLine != newLine) {
            if (!oldLine.empty()) result.push_back("- " + oldLine);
            if (!newLine.empty()) result.push_back("+ " + newLine);
        }
    }
    return result;
}

bool Utils::isIgnored(const std::string& filename, const std::vector<std::string>& ignorePatterns) {
    for (const auto& pattern : ignorePatterns) {
        if (filename.find(pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}