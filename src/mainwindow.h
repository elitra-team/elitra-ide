#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QSplitter>
#include <QProcess>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QMap>

class CodeEditor;
class OutputPanel;

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

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupFileBrowser();
    void setupStyleSheet();
    void loadFileToEditor(const QString &filePath);
    bool saveEditorToFile(int tabIndex);
    bool maybeSave();
    void updateWindowTitle();
    CodeEditor *currentEditor();
    QString currentFilePath();
    QString readFile(const QString &path);
    bool writeFile(const QString &path, const QString &content);

    QSplitter *splitter;
    QTreeView *fileTree;
    QFileSystemModel *fileModel;
    QTabWidget *tabWidget;
    OutputPanel *outputPanel;

    QToolBar *toolBar;
    QAction *runAction;
    QAction *stopAction;

    QProcess *process;
    QString currentRunFile;

    QMap<int, QString> filePaths;
    int untitledCount = 1;

    QString elitraBinary;
    QString workspaceDir;
};

#endif
