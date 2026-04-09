#include "Repository.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: minigit <command> [args...]\n";
        std::cout << "Commands:\n";
        std::cout << "  init\n";
        std::cout << "  add <filename>\n";
        std::cout << "  commit \"message\"\n";
        std::cout << "  log\n";
        std::cout << "  status\n";
        std::cout << "  checkout <commit_id>\n";
        std::cout << "  diff <filename>\n";
        std::cout << "  branch <name>\n";
        std::cout << "  merge <branch>\n";
        return 1;
    }

    Repository repo;
    std::string command = argv[1];

    if (command == "init") {
        repo.init();
    } else if (command == "add" && argc == 3) {
        repo.add(argv[2]);
    } else if (command == "commit" && argc == 3) {
        repo.commit(argv[2]);
    } else if (command == "log") {
        repo.log();
    } else if (command == "status") {
        repo.status();
    } else if (command == "checkout" && argc == 3) {
        repo.checkout(argv[2]);
    } else if (command == "diff" && argc == 3) {
        repo.diff(argv[2]);
    } else if (command == "branch" && argc == 3) {
        repo.branch(argv[2]);
    } else if (command == "merge" && argc == 3) {
        repo.merge(argv[2]);
    } else {
        std::cout << "Invalid command or arguments.\n";
        return 1;
    }

    return 0;
}
