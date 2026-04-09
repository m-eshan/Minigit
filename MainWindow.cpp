#include "MainWindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>

QString MainWindow::resolveRepositoryRoot() {
    QStringList candidateRoots = {
        QApplication::applicationDirPath(),
        QDir::currentPath()
    };

    for (const QString& candidate : candidateRoots) {
        QDir dir(candidate);
        for (int depth = 0; depth < 4; ++depth) {
            const bool looksLikeRepo =
                dir.exists("CMakeLists.txt") &&
                dir.exists("MainWindow.cpp") &&
                dir.exists("Repository.cpp");
            if (looksLikeRepo || dir.exists(".minigit")) {
                return dir.absolutePath();
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }

    return QDir::currentPath();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      repoRootPath(resolveRepositoryRoot()),
      repo(repoRootPath.toStdString()) {
    setWindowTitle("MiniGit GUI");
    setGeometry(100, 100, 800, 600);

    // Central widget
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    // Layouts
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Top buttons layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    initButton = new QPushButton("Init");
    addFileButton = new QPushButton("Add File");
    commitButton = new QPushButton("Commit");
    logButton = new QPushButton("Log");
    statusButton = new QPushButton("Status");
    diffButton = new QPushButton("Diff");
    checkoutButton = new QPushButton("Checkout");
    branchButton = new QPushButton("Branch/Switch");
    mergeButton = new QPushButton("Merge");
    clearButton = new QPushButton("Clear");

    buttonsLayout->addWidget(initButton);
    buttonsLayout->addWidget(addFileButton);
    buttonsLayout->addWidget(commitButton);
    buttonsLayout->addWidget(logButton);
    buttonsLayout->addWidget(statusButton);
    buttonsLayout->addWidget(diffButton);
    buttonsLayout->addWidget(checkoutButton);
    buttonsLayout->addWidget(branchButton);
    buttonsLayout->addWidget(mergeButton);
    buttonsLayout->addWidget(clearButton);

    mainLayout->addLayout(buttonsLayout);

    // Output text edit
    outputTextEdit = new QTextEdit();
    outputTextEdit->setReadOnly(true);
    mainLayout->addWidget(new QLabel("Output:"));
    mainLayout->addWidget(outputTextEdit);

    // Input line edit
    inputLineEdit = new QLineEdit();
    mainLayout->addWidget(new QLabel("Input (commit message, filename, branch, or commit ID):"));
    mainLayout->addWidget(inputLineEdit);
    inputLineEdit->setPlaceholderText("Commit message, branch name, filename, or commit ID");

    // Connect signals
    connect(initButton, &QPushButton::clicked, this, &MainWindow::onInitClicked);
    connect(addFileButton, &QPushButton::clicked, this, &MainWindow::onAddFileClicked);
    connect(commitButton, &QPushButton::clicked, this, &MainWindow::onCommitClicked);
    connect(logButton, &QPushButton::clicked, this, &MainWindow::onLogClicked);
    connect(statusButton, &QPushButton::clicked, this, &MainWindow::onStatusClicked);
    connect(diffButton, &QPushButton::clicked, this, &MainWindow::onDiffClicked);
    connect(checkoutButton, &QPushButton::clicked, this, &MainWindow::onCheckoutClicked);
    connect(branchButton, &QPushButton::clicked, this, &MainWindow::onBranchClicked);
    connect(mergeButton, &QPushButton::clicked, this, &MainWindow::onMergeClicked);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);

    outputTextEdit->append("<font color='gray'>Repository root: " + repoRootPath + "</font>");
}

MainWindow::~MainWindow() {}

QString MainWindow::captureOutput(std::function<void()> func) {
    std::stringstream ss;
    std::streambuf* old = std::cout.rdbuf(ss.rdbuf());
    try {
        func();
    } catch (const fs::filesystem_error& ex) {
        ss << "Filesystem error: " << ex.what() << "\n";
    } catch (const std::exception& ex) {
        ss << "Error: " << ex.what() << "\n";
    }
    std::cout.rdbuf(old);
    return QString::fromStdString(ss.str());
}

void MainWindow::onInitClicked() {
    QString output = captureOutput([&]() { repo.init(); });
    outputTextEdit->append("<font color='green'>" + output + "</font>");
}

void MainWindow::onAddFileClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select File to Add", repoRootPath);
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            QMessageBox::warning(this, "Error", "Selected item is not a valid file.");
            return;
        }
        QDir repoDir(repoRootPath);
        QString relativePath = repoDir.relativeFilePath(fileInfo.absoluteFilePath());
        if (relativePath.startsWith("..")) {
            QMessageBox::warning(this, "Error", "Selected file must be inside the repository folder.");
            return;
        }
        QString output = captureOutput([&]() { repo.add(relativePath.toStdString()); });
        outputTextEdit->append("<font color='green'>" + output + "</font>");
    }
}

void MainWindow::onCommitClicked() {
    QString message = inputLineEdit->text();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a commit message.");
        return;
    }
    QString output = captureOutput([&]() { repo.commit(message.toStdString()); });
    outputTextEdit->append("<font color='green'>" + output + "</font>");
    inputLineEdit->clear();
}

void MainWindow::onLogClicked() {
    QString output = captureOutput([&]() { repo.log(); });
    outputTextEdit->append("<font color='blue'>" + output + "</font>");
}

void MainWindow::onStatusClicked() {
    QString output = captureOutput([&]() { repo.status(); });
    outputTextEdit->append("<font color='blue'>" + output + "</font>");
}

void MainWindow::onDiffClicked() {
    QString filename = inputLineEdit->text();
    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a filename.");
        return;
    }
    QString output = captureOutput([&]() { repo.diff(filename.toStdString()); });
    outputTextEdit->append("<font color='orange'>" + output + "</font>");
    inputLineEdit->clear();
}

void MainWindow::onCheckoutClicked() {
    QString commitId = inputLineEdit->text();
    if (commitId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a branch name or commit ID.");
        return;
    }
    QString output = captureOutput([&]() { repo.checkout(commitId.toStdString()); });
    outputTextEdit->append("<font color='green'>" + output + "</font>");
    inputLineEdit->clear();
}

void MainWindow::onBranchClicked() {
    QString branchName = inputLineEdit->text();
    if (branchName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a branch name.");
        return;
    }
    QString output = captureOutput([&]() { repo.branch(branchName.toStdString()); });
    outputTextEdit->append("<font color='green'>" + output + "</font>");
    inputLineEdit->clear();
}

void MainWindow::onMergeClicked() {
    QString branchName = inputLineEdit->text();
    if (branchName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a branch name to merge.");
        return;
    }
    QString output = captureOutput([&]() { repo.merge(branchName.toStdString()); });
    outputTextEdit->append("<font color='green'>" + output + "</font>");
    inputLineEdit->clear();
}

void MainWindow::onClearClicked() {
    outputTextEdit->clear();
    outputTextEdit->append("<font color='gray'>Repository root: " + repoRootPath + "</font>");
}
