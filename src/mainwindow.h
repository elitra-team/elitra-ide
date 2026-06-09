#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QProcess>
#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QMap>
#include <QLineEdit>

class CodeEditor;
class OutputPanel;
class FindReplacePanel;
class FindInFilesPanel;
class LspClient;
class Sidebar;
class StatusBarManager;

#include "activitybar.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void newFile();

private slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void runFile();
    void stopRun();
    void onTreeDoubleClicked(const QModelIndex &index);
    void onTabCloseRequested(int index);
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessOutput();
    void onProcessError();

    void showFind();
    void showReplace();
    void showFindInFiles();
    void showGoToLine();
    void showSettings();
    void applySettings();

    void onViewToggled(ActivityBar::View view);
    void onCursorMoved(int line, int col);
    void updateStatusBar();

private:
    void setupUI();
    void setupMenuBar();
    void setupActions();
    void setupStyleSheet();
    void loadFileToEditor(const QString &filePath);
    bool saveEditorToFile(int tabIndex);
    bool maybeSave();
    void updateWindowTitle();
    CodeEditor *currentEditor();
    QString currentFilePath();
    QString readFile(const QString &path);
    bool writeFile(const QString &path, const QString &content);

    void setupFindReplace();
    void setupFindInFiles();
    void setupLsp();
    QString editorUri(const QString &filePath);
    void updateLspDiagnostics(const QString &uri, const QJsonArray &diagnostics);

    // Layout
    ActivityBar *activityBar;
    Sidebar *sidebar;
    QSplitter *editorSplitter;
    QTabWidget *tabWidget;
    OutputPanel *outputPanel;
    FindReplacePanel *findReplacePanel;
    FindInFilesPanel *findInFilesPanel;
    StatusBarManager *statusBarManager;

    // Actions
    QAction *runAction;
    QAction *stopAction;

    // Process
    QProcess *process;
    QString currentRunFile;

    // Files
    QMap<int, QString> filePaths;
    int untitledCount = 1;

    // Paths
    QString elitraBinary;
    QString workspaceDir;

    // LSP
    LspClient *lspClient;
    bool lspEnabled = false;
};

#endif
