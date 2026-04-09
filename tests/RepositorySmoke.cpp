#include "Repository.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace {
namespace fs = std::filesystem;

std::string captureOutput(const std::function<void()>& func) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    func();
    std::cout.rdbuf(old);
    return buffer.str();
}

void writeFile(const fs::path& path, const std::string& contents) {
    fs::create_directories(path.parent_path());
    std::ofstream out(path);
    out << contents;
}

std::string readFile(const fs::path& path) {
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

std::string extractCommitId(const std::string& output) {
    const std::string marker = "with ID: ";
    const std::size_t pos = output.find(marker);
    if (pos == std::string::npos) {
        return "";
    }

    std::size_t end = output.find('\n', pos);
    return output.substr(pos + marker.size(), end - (pos + marker.size()));
}

bool contains(const std::string& text, const std::string& needle) {
    return text.find(needle) != std::string::npos;
}

bool expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << "\n";
        return false;
    }
    return true;
}
}

int main() {
    const fs::path repoDir = fs::temp_directory_path() / "minigit_smoke_repo";
    std::error_code ec;
    fs::remove_all(repoDir, ec);
    fs::create_directories(repoDir);

    bool ok = true;

    Repository repo(repoDir);
    ok &= expect(!repo.add("alpha.txt"), "add should fail before init");
    ok &= expect(repo.init(), "init should succeed");
    ok &= expect(!repo.init(), "second init should fail cleanly");

    writeFile(repoDir / "alpha.txt", "one\n");
    ok &= expect(repo.add("alpha.txt"), "initial add should succeed");
    const std::string firstCommitOutput = captureOutput([&]() { ok &= repo.commit("first commit"); });
    const std::string firstCommitId = extractCommitId(firstCommitOutput);
    ok &= expect(!firstCommitId.empty(), "first commit id should be present");

    writeFile(repoDir / "alpha.txt", "two\n");
    ok &= expect(repo.add("alpha.txt"), "second add should succeed");
    const std::string secondCommitOutput = captureOutput([&]() { ok &= repo.commit("second commit"); });
    const std::string secondCommitId = extractCommitId(secondCommitOutput);
    ok &= expect(!secondCommitId.empty(), "second commit id should be present");

    ok &= expect(repo.branch("feature"), "creating feature branch should succeed");
    const std::string featureStatus = captureOutput([&]() { repo.status(); });
    ok &= expect(contains(featureStatus, "On branch feature"), "branch command should switch to feature");

    writeFile(repoDir / "beta.txt", "feature only\n");
    ok &= expect(repo.add("beta.txt"), "feature add should succeed");
    const std::string featureCommitOutput = captureOutput([&]() { ok &= repo.commit("feature commit"); });
    const std::string featureCommitId = extractCommitId(featureCommitOutput);
    ok &= expect(!featureCommitId.empty(), "feature commit id should be present");

    ok &= expect(repo.checkout("master"), "checkout to master branch should succeed");
    ok &= expect(!fs::exists(repoDir / "beta.txt"), "switching to master should remove feature-only file");
    ok &= expect(readFile(repoDir / "alpha.txt") == "two\n", "master branch should keep its latest alpha contents");

    writeFile(repoDir / "alpha.txt", "two modified\n");
    const std::string diffOutput = captureOutput([&]() { repo.diff("alpha.txt"); });
    ok &= expect(contains(diffOutput, "- two") && contains(diffOutput, "+ two modified"), "diff should report working tree changes");
    ok &= expect(!repo.merge("feature"), "merge should refuse while local modifications exist");

    ok &= expect(repo.add("alpha.txt"), "modified master file should be stageable");
    const std::string masterCommitOutput = captureOutput([&]() { ok &= repo.commit("master tweak"); });
    const std::string masterCommitId = extractCommitId(masterCommitOutput);
    ok &= expect(!masterCommitId.empty(), "master tweak commit id should be present");

    writeFile(repoDir / "build" / "generated.txt", "skip me\n");
    ok &= expect(!repo.add("build/generated.txt"), "build outputs should not be stageable");

    ok &= expect(repo.merge("feature"), "merge from feature into master should succeed");
    ok &= expect(fs::exists(repoDir / "beta.txt"), "merge should bring feature file into master");
    ok &= expect(readFile(repoDir / "beta.txt") == "feature only\n", "merged file contents should match feature branch");
    ok &= expect(readFile(repoDir / "alpha.txt") == "two modified\n", "merge should keep current branch version for existing files");

    ok &= expect(repo.checkout(firstCommitId), "checkout by commit id should succeed");
    ok &= expect(readFile(repoDir / "alpha.txt") == "one\n", "old commit checkout should restore original alpha contents");
    ok &= expect(!fs::exists(repoDir / "beta.txt"), "old commit checkout should remove later files");

    Repository reloaded(repoDir);
    const std::string reloadedStatus = captureOutput([&]() { reloaded.status(); });
    ok &= expect(contains(reloadedStatus, "On branch master"), "reloaded repository should preserve current branch");
    ok &= expect(reloaded.checkout("feature"), "reloaded repository should switch to feature");
    ok &= expect(fs::exists(repoDir / "beta.txt"), "feature branch should still contain beta after reload");

    fs::remove_all(repoDir, ec);
    if (!ok) {
        return 1;
    }

    std::cout << "MiniGit smoke tests passed.\n";
    return 0;
}
