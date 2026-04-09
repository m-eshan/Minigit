#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "Commit.h"
#include "Utils.h"
#include <set>
#include <map>

class Repository {
private:
    fs::path repoPath;
    fs::path minigitPath;
    fs::path commitsPath;
    fs::path stagingPath;
    fs::path logsPath;
    fs::path stagedListPath;
    fs::path headPath;
    std::set<std::string> stagedFiles;
    std::map<std::string, std::vector<Commit>> branches;
    std::string currentBranch;
    std::string currentCommitId;
    std::vector<std::string> ignorePatterns;

    void loadIgnorePatterns();
    void saveLogs();
    void loadLogs();
    void saveStaged();
    void loadStaged();
    void loadCommitContents(Commit& commit);
    void saveHead() const;
    void loadHead();
    void restoreCommit(const Commit& fromCommit, const Commit& toCommit);
    std::map<std::string, std::vector<std::string>> buildSnapshotForCommit() const;
    Commit getLastCommit();
    Commit getLastCommitForBranch(const std::string& branchName) const;
    Commit getCurrentCommit() const;
    Commit findCommitById(const std::string& commitId, std::string* branchName = nullptr) const;
    std::vector<std::string> getAllFiles();
    std::vector<std::string> getModifiedFiles();
    std::vector<std::string> getUntrackedFiles();
    bool ensureInitialized(const std::string& action) const;
    bool isReservedPath(const std::string& relativePath) const;

public:
    Repository(const fs::path& path = fs::current_path());

    bool init();
    bool add(const std::string& filename);
    bool commit(const std::string& message);
    void log();
    void status();
    bool checkout(const std::string& commitId);
    void diff(const std::string& filename);
    bool branch(const std::string& name);
    bool merge(const std::string& branchName);
};

#endif // REPOSITORY_H
