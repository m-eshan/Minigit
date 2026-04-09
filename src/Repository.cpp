#include "Repository.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Repository::Repository(const fs::path& path) : repoPath(path), currentBranch("master") {
    minigitPath = repoPath / ".minigit";
    commitsPath = minigitPath / "commits";
    stagingPath = minigitPath / "staging";
    logsPath = minigitPath / "logs.txt";
    stagedListPath = minigitPath / "staged.txt";
    loadIgnorePatterns();
    if (fs::exists(logsPath)) {
        loadLogs();
    }
    if (fs::exists(stagedListPath)) {
        loadStaged();
    }
}

void Repository::loadIgnorePatterns() {
    fs::path ignoreFile = repoPath / ".minigitignore";
    if (Utils::fileExists(ignoreFile)) {
        auto lines = Utils::readFileLines(ignoreFile);
        ignorePatterns = lines;
    }
}

void Repository::saveLogs() {
    std::ofstream ofs(logsPath);
    for (const auto& branch : branches) {
        for (const auto& commit : branch.second) {
            ofs << commit.id << "|" << commit.message << "|" << commit.timestamp << "|" << branch.first;
            for (const auto& file : commit.files) {
                ofs << "|" << file;
            }
            ofs << "\n";
        }
    }
}

void Repository::loadLogs() {
    auto lines = Utils::readFileLines(logsPath);
    for (const auto& line : lines) {
        std::vector<std::string> parts;
        size_t start = 0;
        size_t pos;
        while ((pos = line.find('|', start)) != std::string::npos) {
            parts.push_back(line.substr(start, pos - start));
            start = pos + 1;
        }
        parts.push_back(line.substr(start));
        if (parts.size() >= 4) {
            std::string id = parts[0];
            std::string message = parts[1];
            std::string timestamp = parts[2];
            std::string branch = parts[3];
            std::vector<std::string> files(parts.begin() + 4, parts.end());
            Commit commit{id, message, timestamp, files};
            branches[branch].push_back(commit);
            loadCommitContents(branches[branch].back());
        }
    }
}

void Repository::loadCommitContents(Commit& commit) {
    fs::path commitDir = commitsPath / commit.id;
    for (const auto& file : commit.files) {
        commit.fileContents[file] = Utils::readFileLines(commitDir / file);
    }
}

void Repository::saveStaged() {
    std::ofstream ofs(stagedListPath);
    for (const auto& file : stagedFiles) {
        ofs << file << "\n";
    }
}

void Repository::loadStaged() {
    auto lines = Utils::readFileLines(stagedListPath);
    stagedFiles.clear();
    for (const auto& line : lines) {
        if (!line.empty()) {
            stagedFiles.insert(line);
        }
    }
}

bool Repository::init() {
    if (fs::exists(minigitPath)) {
        std::cout << "Repository already initialized.\n";
        return false;
    }
    fs::create_directories(commitsPath);
    fs::create_directories(stagingPath);
    std::ofstream ofs(logsPath);
    ofs.close();
    std::ofstream ofs2(stagedListPath);
    ofs2.close();
    std::cout << "Initialized empty MiniGit repository in " << minigitPath << "\n";
    return true;
}

bool Repository::add(const std::string& filename) {
    fs::path filePath = repoPath / filename;
    if (!Utils::fileExists(filePath)) {
        std::cout << "File not found: " << filename << "\n";
        return false;
    }
    if (Utils::isIgnored(filename, ignorePatterns)) {
        std::cout << "File is ignored: " << filename << "\n";
        return false;
    }
    fs::path stagingFile = stagingPath / filename;
    Utils::copyFile(filePath, stagingFile);
    stagedFiles.insert(filename);
    saveStaged();
    std::cout << "Added " << filename << " to staging area.\n";
    return true;
}

bool Repository::commit(const std::string& message) {
    if (stagedFiles.empty()) {
        std::cout << "Nothing to commit.\n";
        return false;
    }
    std::string commitId = Utils::generateCommitId();
    fs::path commitDir = commitsPath / commitId;
    fs::create_directories(commitDir);
    Commit commit{commitId, message, Utils::getCurrentTime()};
    for (const auto& file : stagedFiles) {
        fs::path src = stagingPath / file;
        fs::path dest = commitDir / file;
        Utils::copyFile(src, dest);
        commit.files.push_back(file);
        commit.fileContents[file] = Utils::readFileLines(src);
    }
    branches[currentBranch].push_back(commit);
    saveLogs();
    // Clear staging
    for (const auto& file : stagedFiles) {
        fs::remove(stagingPath / file);
    }
    stagedFiles.clear();
    saveStaged();
    std::cout << "Committed with ID: " << commitId << "\n";
    return true;
}

void Repository::log() {
    if (branches.find(currentBranch) == branches.end() || branches[currentBranch].empty()) {
        std::cout << "No commits yet.\n";
        return;
    }
    auto& commits = branches[currentBranch];
    for (auto it = commits.rbegin(); it != commits.rend(); ++it) {
        std::cout << "Commit: " << it->id << "\n";
        std::cout << "Message: " << it->message << "\n";
        std::cout << "Date: " << it->timestamp << "\n";
        std::cout << "Files: ";
        for (const auto& file : it->files) {
            std::cout << file << " ";
        }
        std::cout << "\n\n";
    }
}

void Repository::status() {
    std::cout << "On branch " << currentBranch << "\n";
    std::cout << "Staged files:\n";
    for (const auto& file : stagedFiles) {
        std::cout << "  " << file << "\n";
    }
    auto modified = getModifiedFiles();
    std::cout << "Modified files:\n";
    for (const auto& file : modified) {
        std::cout << "  " << file << "\n";
    }
    auto untracked = getUntrackedFiles();
    std::cout << "Untracked files:\n";
    for (const auto& file : untracked) {
        std::cout << "  " << file << "\n";
    }
}

std::vector<std::string> Repository::getModifiedFiles() {
    std::vector<std::string> modified;
    auto lastCommit = getLastCommit();
    if (lastCommit.id.empty()) return modified;
    for (const auto& file : lastCommit.files) {
        fs::path workingFile = repoPath / file;
        if (Utils::fileExists(workingFile)) {
            auto workingLines = Utils::readFileLines(workingFile);
            if (lastCommit.fileContents.find(file) != lastCommit.fileContents.end()) {
                if (workingLines != lastCommit.fileContents[file]) {
                    modified.push_back(file);
                }
            }
        }
    }
    return modified;
}

std::vector<std::string> Repository::getUntrackedFiles() {
    std::vector<std::string> untracked;
    auto allFiles = getAllFiles();
    auto lastCommit = getLastCommit();
    std::set<std::string> committedFiles(lastCommit.files.begin(), lastCommit.files.end());
    for (const auto& file : allFiles) {
        if (committedFiles.find(file) == committedFiles.end() && stagedFiles.find(file) == stagedFiles.end()) {
            if (!Utils::isIgnored(file, ignorePatterns)) {
                untracked.push_back(file);
            }
        }
    }
    return untracked;
}

std::vector<std::string> Repository::getAllFiles() {
    auto files = Utils::getFilesInDir(repoPath);
    files.erase(std::remove_if(files.begin(), files.end(), [this](const std::string& f) {
        return f.find(".minigit") == 0;
    }), files.end());
    return files;
}

Commit Repository::getLastCommit() {
    if (branches.find(currentBranch) != branches.end() && !branches[currentBranch].empty()) {
        return branches[currentBranch].back();
    }
    return Commit{"", "", ""};
}

bool Repository::checkout(const std::string& commitId) {
    Commit targetCommit;
    bool found = false;
    for (auto& branch : branches) {
        for (auto& commit : branch.second) {
            if (commit.id == commitId) {
                targetCommit = commit;
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (!found) {
        std::cout << "Commit not found: " << commitId << "\n";
        return false;
    }
    fs::path commitDir = commitsPath / commitId;
    for (const auto& file : targetCommit.files) {
        fs::path src = commitDir / file;
        fs::path dest = repoPath / file;
        Utils::copyFile(src, dest);
    }
    std::cout << "Checked out commit: " << commitId << "\n";
    return true;
}

void Repository::diff(const std::string& filename) {
    auto lastCommit = getLastCommit();
    if (lastCommit.id.empty()) {
        std::cout << "No previous commit.\n";
        return;
    }
    fs::path workingFile = repoPath / filename;
    if (!Utils::fileExists(workingFile)) {
        std::cout << "File not found: " << filename << "\n";
        return;
    }
    auto workingLines = Utils::readFileLines(workingFile);
    if (lastCommit.fileContents.find(filename) != lastCommit.fileContents.end()) {
        auto oldLines = lastCommit.fileContents[filename];
        auto differences = Utils::diff(oldLines, workingLines);
        if (differences.empty()) {
            std::cout << "No differences.\n";
        } else {
            for (const auto& diff : differences) {
                std::cout << diff << "\n";
            }
        }
    } else {
        std::cout << "File not in last commit.\n";
    }
}

bool Repository::branch(const std::string& name) {
    if (branches.find(name) != branches.end()) {
        std::cout << "Branch already exists: " << name << "\n";
        return false;
    }
    if (branches.find(currentBranch) != branches.end()) {
        branches[name] = branches[currentBranch];
    }
    std::cout << "Created branch: " << name << "\n";
    return true;
}

bool Repository::merge(const std::string& branchName) {
    if (branches.find(branchName) == branches.end()) {
        std::cout << "Branch not found: " << branchName << "\n";
        return false;
    }
    if (branchName == currentBranch) {
        std::cout << "Cannot merge branch into itself.\n";
        return false;
    }
    auto& otherCommits = branches[branchName];
    if (otherCommits.empty()) {
        std::cout << "Branch has no commits.\n";
        return false;
    }
    auto lastOther = otherCommits.back();
    // Simple merge: just checkout the last commit of the other branch
    checkout(lastOther.id);
    // Add a merge commit
    std::string message = "Merge branch '" + branchName + "' into " + currentBranch;
    commit(message);
    std::cout << "Merged " << branchName << " into " << currentBranch << "\n";
    return true;
}
