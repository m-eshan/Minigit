# MiniGit GUI

A desktop GUI application for MiniGit, a lightweight version control system, built with Qt Widgets.

## Features

- **Init**: Initialize a new repository
- **Add File**: Select and stage a file using file dialog
- **Commit**: Commit staged files with a full repository snapshot
- **Log**: View commit history
- **Status**: Check repository status
- **Diff**: Show differences for a file
- **Checkout**: Restore files from a commit or switch branches
- **Branch**: Create a branch or switch to an existing one
- **Merge**: Merge a branch into current
- **Clear**: Clear the output log

## UI Components

- **Buttons Row**: Action buttons for each command
- **Output Area**: Colored text display for results and logs
- **Input Field**: Text input for messages, filenames, IDs, etc.

## Build Instructions

### Prerequisites

- Qt6 (or Qt5) installed
- CMake
- C++17 compiler

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

On Windows with Qt6, the build now places the runnable GUI in:

```bash
../dist/minigit_gui.exe
```

### Using Qt Creator

1. Open CMakeLists.txt in Qt Creator
2. Build and run the project

## File Structure

```
Minigit/
├── main.cpp              # Qt application entry point
├── MainWindow.h/.cpp     # GUI window implementation
├── Repository.h/.cpp     # Core repository logic
├── Commit.h              # Commit data structure
├── Utils.h/.cpp          # Utility functions
├── CMakeLists.txt        # Build configuration
└── README.md             # Documentation
```

## How It Works

The GUI directly integrates with the existing MiniGit backend classes. Each button click captures the console output from the Repository methods and displays it in the text area with appropriate colors:

- Green: Success messages
- Blue: Information (log, status)
- Orange: Diff output
- Red: Errors (via QMessageBox)

Input is handled through the bottom text field, with validation for required inputs.

The GUI detects the MiniGit project root automatically, so launching it from `build`, `dist`, or by double-clicking the `.exe` still points at the correct repository folder.

## Recommended Workflow

1. Click `Init` once inside your project folder.
2. Use `Add File` to stage one or more project files.
3. Enter a commit message and click `Commit`.
4. Enter a new branch name and click `Branch/Switch` to create or switch branches.
5. Enter an existing branch name or commit ID and click `Checkout` to move around history.
6. Enter another branch name and click `Merge` to merge it into the current branch.

## Smoke Test

After configuring CMake, you can run the automated repository smoke test with:

```bash
ctest --output-on-failure
```
