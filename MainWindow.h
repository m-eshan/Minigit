#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <sstream>
#include <iostream>
#include "Repository.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInitClicked();
    void onAddFileClicked();
    void onCommitClicked();
    void onLogClicked();
    void onStatusClicked();
    void onDiffClicked();
    void onCheckoutClicked();
    void onBranchClicked();
    void onMergeClicked();
    void onClearClicked();

private:
    QString repoRootPath;
    Repository repo;
    QTextEdit *outputTextEdit;
    QLineEdit *inputLineEdit;
    QPushButton *initButton;
    QPushButton *addFileButton;
    QPushButton *commitButton;
    QPushButton *logButton;
    QPushButton *statusButton;
    QPushButton *diffButton;
    QPushButton *checkoutButton;
    QPushButton *branchButton;
    QPushButton *mergeButton;
    QPushButton *clearButton;

    static QString resolveRepositoryRoot();
    QString captureOutput(std::function<void()> func);
};

#endif // MAINWINDOW_H
