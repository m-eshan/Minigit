# MiniGit

MiniGit is a lightweight version control system built in C++ with a Qt-based desktop GUI. It demonstrates the core ideas behind Git-style workflows, including repository initialization, staging, commits, history browsing, branching, and file recovery.

## What This Project Shows

- Repository initialization
- File staging and commit creation
- Commit history browsing
- Status, diff, checkout, branch, and merge actions
- A desktop GUI that makes the version-control flow easy to use

## Quick Demo

```text
1. Start MiniGit
2. Click Init to create a repository
3. Add a file to the staging area
4. Enter a commit message and commit the snapshot
5. Open Log to inspect history
6. Use Checkout, Branch, or Merge to move through versions
```

Example flow:

```text
Init -> Add File -> Commit -> Log -> Checkout
```

## Example Output

The GUI shows status messages directly in the application window. A typical run looks like this:

```text
Repository initialized successfully
File staged successfully
Commit created: first commit
Log loaded successfully
```

## How It Works

- `src/Repository.*` contains the core repository logic
- `src/Commit.h` defines the commit structure
- `src/Utils.*` provides helper functions used across the project
- `src/MainWindow.*` connects the GUI buttons and inputs to the backend logic
- `src/main.cpp` starts the Qt application

The interface keeps the project root in sync so you can launch the app from the build folder or executable and still work with the same MiniGit repository.

## Build Instructions

### Prerequisites

- Qt 5 or Qt 6
- CMake
- A C++17 compiler

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Using Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Build the project
3. Run the application

## Project Structure

```text
Minigit/
+-- src/
¦   +-- main.cpp
¦   +-- MainWindow.cpp
¦   +-- MainWindow.h
¦   +-- Repository.cpp
¦   +-- Repository.h
¦   +-- Commit.h
¦   +-- Utils.cpp
¦   +-- Utils.h
+-- tests/
¦   +-- RepositorySmoke.cpp
+-- assets/
¦   +-- .gitkeep
+-- .minigit/
+-- CMakeLists.txt
+-- README.md
+-- .gitignore
```

## Recommended Workflow

1. Initialize a repository.
2. Stage a file.
3. Commit with a clear message.
4. View history with Log.
5. Use Checkout, Branch, or Merge when you want to explore versions.

## Notes

- `assets/` can be used for screenshots or demo images.
- `tests/RepositorySmoke.cpp` is the smoke test entry point.

## Smoke Test

After configuring CMake, run the automated test suite with:

```bash
ctest --output-on-failure
```
