#ifndef COMMIT_H
#define COMMIT_H

#include <string>
#include <vector>
#include <map>

struct Commit {
    std::string id;
    std::string message;
    std::string timestamp;
    std::vector<std::string> files; // relative paths
    std::map<std::string, std::vector<std::string>> fileContents; // file to lines
};

#endif // COMMIT_H