#include "mainwindow.h"
#include "codeeditor.h"
#include "outputpanel.h"
#include "elitrahighlighter.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QFont>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), process(nullptr)
{
    QString home = QDir::homePath();
    elitraBinary = home + "/Projects/Elitra lang/target/release/elitra";
    if (!QFileInfo::exists(elitraBinary))
        elitraBinary = QDir::currentPath() + "/../Elitra lang/target/release/elitra";
    if (!QFileInfo::exists(elitraBinary))
        elitraBinary = "elitra";

    workspaceDir = QDir::currentPath();
    if (QFileInfo::exists(workspaceDir + "/../Elitra lang"))
        workspaceDir = QDir(workspaceDir + "/../Elitra lang").absolutePath();

    setupUI();
    setupStyleSheet();
    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    if (process) {
        process->kill();
        process->waitForFinished(3000);
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Elitra IDE");
    resize(1200, 800);

    setupMenuBar();
    setupToolBar();

    splitter = new QSplitter(Qt::Horizontal);

    setupFileBrowser();
    setupStyleSheet();

    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setDocumentMode(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::onTabCloseRequested);

    auto *rightPanel = new QWidget;
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    outputPanel = new OutputPanel;
    connect(outputPanel, &OutputPanel::stopRequested,
            this, &MainWindow::stopRun);

    auto *editorSplitter = new QSplitter(Qt::Vertical);
    editorSplitter->addWidget(tabWidget);
    editorSplitter->addWidget(outputPanel);
    editorSplitter->setStretchFactor(0, 3);
    editorSplitter->setStretchFactor(1, 1);

    rightLayout->addWidget(editorSplitter);

    splitter->addWidget(fileTree);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setSizes({250, 950});

    setCentralWidget(splitter);

    statusBar()->showMessage("Ready");
}

void MainWindow::setupMenuBar()
{
    auto *fileMenu = menuBar()->addMenu("&File");

    fileMenu->addAction("&New File", QKeySequence::New, this, &MainWindow::newFile);
    fileMenu->addAction("&Open File...", QKeySequence::Open, this, &MainWindow::openFile);
    fileMenu->addAction("&Save", QKeySequence::Save, this, &MainWindow::saveFile);
    fileMenu->addAction("Save &As...", QKeySequence("Ctrl+Shift+S"), this, &MainWindow::saveFileAs);
    fileMenu->addSeparator();
    fileMenu->addAction("Close &Tab", QKeySequence("Ctrl+W"), this, [this]() {
        int idx = tabWidget->currentIndex();
        if (idx >= 0) onTabCloseRequested(idx);
    });
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &QWidget::close);

    auto *editMenu = menuBar()->addMenu("&Edit");

    auto *undoAction = editMenu->addAction("&Undo", QKeySequence::Undo, this, [this]() {
        if (auto *e = currentEditor()) e->undo();
    });
    auto *redoAction = editMenu->addAction("&Redo", QKeySequence("Ctrl+Y"), this, [this]() {
        if (auto *e = currentEditor()) e->redo();
    });
    editMenu->addSeparator();
    auto *cutAction = editMenu->addAction("Cu&t", QKeySequence::Cut, this, [this]() {
        if (auto *e = currentEditor()) e->cut();
    });
    auto *copyAction = editMenu->addAction("&Copy", QKeySequence::Copy, this, [this]() {
        if (auto *e = currentEditor()) e->copy();
    });
    auto *pasteAction = editMenu->addAction("&Paste", QKeySequence::Paste, this, [this]() {
        if (auto *e = currentEditor()) e->paste();
    });
    editMenu->addSeparator();
    auto *selectAllAction = editMenu->addAction("Select &All", QKeySequence::SelectAll, this, [this]() {
        if (auto *e = currentEditor()) e->selectAll();
    });
    Q_UNUSED(undoAction); Q_UNUSED(redoAction); Q_UNUSED(cutAction);
    Q_UNUSED(copyAction); Q_UNUSED(pasteAction); Q_UNUSED(selectAllAction);

    auto *runMenu = menuBar()->addMenu("&Run");
    runAction = runMenu->addAction("&Run File", QKeySequence("F5"), this, &MainWindow::runFile);
    stopAction = runMenu->addAction("&Stop", QKeySequence("Shift+F5"), this, &MainWindow::stopRun);
    stopAction->setEnabled(false);
}

void MainWindow::setupToolBar()
{
    toolBar = addToolBar("Main");
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(16, 16));

    auto *newBtn = toolBar->addAction("New");
    connect(newBtn, &QAction::triggered, this, &MainWindow::newFile);

    auto *openBtn = toolBar->addAction("Open");
    connect(openBtn, &QAction::triggered, this, &MainWindow::openFile);

    auto *saveBtn = toolBar->addAction("Save");
    connect(saveBtn, &QAction::triggered, this, &MainWindow::saveFile);

    toolBar->addSeparator();

    toolBar->addAction(runAction);
    toolBar->addAction(stopAction);
}

void MainWindow::setupFileBrowser()
{
    fileModel = new QFileSystemModel(this);
    fileModel->setRootPath(workspaceDir);
    fileModel->setNameFilters({"*.eltr", "*.json", "*.md"});
    fileModel->setNameFilterDisables(false);

    fileTree = new QTreeView;
    fileTree->setModel(fileModel);
    fileTree->setRootIndex(fileModel->index(workspaceDir));
    fileTree->setAnimated(true);
    fileTree->setIndentation(16);
    fileTree->setHeaderHidden(true);
    fileTree->hideColumn(1);
    fileTree->hideColumn(2);
    fileTree->hideColumn(3);
    fileTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fileTree->setStyleSheet(
        "QTreeView { background-color: #1f2133; border: none; border-right: 1px solid #16171f; "
        "  color: #a9b1d6; font-size: 12px; }"
        "QTreeView::item { padding: 4px 8px; border-radius: 3px; }"
        "QTreeView::item:selected { background-color: #2a2c3e; color: #c0caf5; }"
        "QTreeView::item:hover { background-color: #252738; }"
        "QTreeView::branch:has-children:closed { border-image: none; }"
        "QTreeView::branch:has-children:open { border-image: none; }"
    );

    connect(fileTree, &QTreeView::doubleClicked,
            this, &MainWindow::onTreeDoubleClicked);
}

void MainWindow::setupStyleSheet()
{
    setStyleSheet(
        "QMainWindow { background-color: #1a1b26; }"

        "QMenuBar { background-color: #1f2133; color: #a9b1d6; border-bottom: 1px solid #16171f; "
        "  font-size: 13px; padding: 1px 0; }"
        "QMenuBar::item { padding: 5px 10px; background: transparent; }"
        "QMenuBar::item:selected { background-color: #2a2c3e; border-radius: 4px; }"

        "QMenu { background-color: #1f2133; color: #a9b1d6; border: 1px solid #16171f; "
        "  padding: 4px; border-radius: 6px; }"
        "QMenu::item { padding: 6px 28px 6px 12px; border-radius: 4px; }"
        "QMenu::item:selected { background-color: #2a2c3e; color: #c0caf5; }"
        "QMenu::separator { height: 1px; background: #2a2c3e; margin: 4px 8px; }"

        "QTabWidget::pane { border: none; background-color: #1a1b26; }"
        "QTabBar { background-color: #16171f; border: none; }"
        "QTabBar::tab { background-color: #16171f; color: #565f89; border: none; "
        "  padding: 7px 18px; margin-right: 1px; font-size: 12px; }"
        "QTabBar::tab:selected { background-color: #1a1b26; color: #c0caf5; "
        "  border-top: 2px solid #7aa2f7; }"
        "QTabBar::tab:hover:!selected { background-color: #1f2133; color: #a9b1d6; }"

        "QToolBar { background-color: #1f2133; border: none; border-bottom: 1px solid #16171f; "
        "  spacing: 2px; padding: 3px 6px; }"
        "QToolBar QToolButton { background-color: transparent; color: #a9b1d6; border: none; "
        "  padding: 5px 12px; border-radius: 5px; font-size: 12px; font-weight: 500; }"
        "QToolBar QToolButton:hover { background-color: #2a2c3e; color: #c0caf5; }"
        "QToolBar QToolButton:disabled { color: #3b4261; }"
        "QToolBar QToolButton:pressed { background-color: #32344a; }"
        "QToolBar::separator { width: 1px; background: #2a2c3e; margin: 4px 6px; }"

        "QStatusBar { background-color: #1f2133; color: #565f89; border-top: 1px solid #16171f; "
        "  font-size: 12px; padding: 2px 10px; }"
        "QStatusBar::item { border: none; }"

        "QSplitter::handle { background-color: #16171f; width: 1px; }"

        "QScrollBar:vertical { background: #1a1b26; width: 10px; margin: 0; }"
        "QScrollBar::handle:vertical { background: #2f3346; min-height: 30px; border-radius: 5px; }"
        "QScrollBar::handle:vertical:hover { background: #3b4261; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar:horizontal { background: #1a1b26; height: 10px; margin: 0; }"
        "QScrollBar::handle:horizontal { background: #2f3346; min-width: 30px; border-radius: 5px; }"
        "QScrollBar::handle:horizontal:hover { background: #3b4261; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }"
    );
}

void MainWindow::newFile()
{
    auto *editor = new CodeEditor;
    new ElitraHighlighter(editor->document());
    int index = tabWidget->addTab(editor, QString("untitled%1.eltr").arg(untitledCount));
    tabWidget->setCurrentIndex(index);
    filePaths[index] = "";
    untitledCount++;
    updateWindowTitle();
    statusBar()->showMessage("New file created");
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Open Elitra File", workspaceDir, "Elitra Files (*.eltr);;All Files (*)");
    if (path.isEmpty())
        return;
    loadFileToEditor(path);
}

void MainWindow::loadFileToEditor(const QString &filePath)
{
    for (auto it = filePaths.begin(); it != filePaths.end(); ++it) {
        if (it.value() == filePath) {
            tabWidget->setCurrentIndex(it.key());
            return;
        }
    }

    QString content = readFile(filePath);
    auto *editor = new CodeEditor;
    new ElitraHighlighter(editor->document());
    editor->setPlainText(content);

    QFileInfo fi(filePath);
    QString tabName = fi.fileName();
    if (tabName.length() > 20)
        tabName = tabName.left(17) + "...";

    int index = tabWidget->addTab(editor, tabName);
    tabWidget->setCurrentIndex(index);
    filePaths[index] = filePath;

    editor->document()->setModified(false);
    connect(editor->document(), &QTextDocument::modificationChanged,
            this, [this]() { updateWindowTitle(); });

    statusBar()->showMessage("Opened: " + filePath, 5000);
    updateWindowTitle();
}

void MainWindow::saveFile()
{
    int idx = tabWidget->currentIndex();
    if (idx < 0) return;

    QString path = filePaths[idx];
    if (path.isEmpty()) {
        saveFileAs();
        return;
    }
    saveEditorToFile(idx);
}

void MainWindow::saveFileAs()
{
    int idx = tabWidget->currentIndex();
    if (idx < 0) return;

    QString path = QFileDialog::getSaveFileName(
        this, "Save Elitra File", workspaceDir, "Elitra Files (*.eltr);;All Files (*)");
    if (path.isEmpty()) return;

    if (!path.endsWith(".eltr"))
        path += ".eltr";

    filePaths[idx] = path;
    QFileInfo fi(path);
    tabWidget->setTabText(idx, fi.fileName());
    saveEditorToFile(idx);
}

bool MainWindow::saveEditorToFile(int tabIndex)
{
    auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(tabIndex));
    if (!editor) return false;

    QString path = filePaths[tabIndex];
    if (path.isEmpty()) return false;

    if (writeFile(path, editor->toPlainText())) {
        editor->document()->setModified(false);
        statusBar()->showMessage("Saved: " + path, 5000);
        updateWindowTitle();
        return true;
    }
    return false;
}

bool MainWindow::maybeSave()
{
    for (auto it = filePaths.begin(); it != filePaths.end(); ++it) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(it.key()));
        if (editor && editor->document()->isModified()) {
            auto ret = QMessageBox::warning(
                this, "Unsaved Changes",
                "There are unsaved changes. Do you want to save before continuing?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            if (ret == QMessageBox::Save) {
                if (it.value().isEmpty()) {
                    int idx = it.key();
                    saveFileAs();
                    if (filePaths[idx].isEmpty()) return false;
                } else {
                    saveEditorToFile(it.key());
                }
            } else if (ret == QMessageBox::Cancel) {
                return false;
            }
        }
    }
    return true;
}

void MainWindow::runFile()
{
    if (process && process->state() != QProcess::NotRunning) {
        statusBar()->showMessage("A process is already running");
        return;
    }

    int idx = tabWidget->currentIndex();
    if (idx < 0) {
        statusBar()->showMessage("No file to run");
        return;
    }

    QString path = filePaths[idx];
    if (path.isEmpty()) {
        saveFileAs();
        path = filePaths[tabWidget->currentIndex()];
        if (path.isEmpty()) return;
    }

    if (!saveEditorToFile(idx)) return;

    if (!QFileInfo::exists(elitraBinary)) {
        QMessageBox::critical(this, "Error",
            "Elitra binary not found at:\n" + elitraBinary +
            "\n\nPlease build the Elitra language first.");
        outputPanel->appendError("Error: elitra binary not found at " + elitraBinary);
        return;
    }

    outputPanel->clearOutput();
    outputPanel->setRunning(true);
    runAction->setEnabled(false);
    stopAction->setEnabled(true);
    currentRunFile = path;

    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory(QFileInfo(path).absolutePath());

    connect(process, &QProcess::started, this, &MainWindow::onProcessStarted);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);
    connect(process, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onProcessOutput);
    connect(process, &QProcess::readyReadStandardError,
            this, &MainWindow::onProcessError);

    QStringList args = {path};
    process->start(elitraBinary, args);

    statusBar()->showMessage("Running: " + path);
}

void MainWindow::stopRun()
{
    if (process && process->state() != QProcess::NotRunning) {
        process->kill();
        outputPanel->appendOutput("\n[Execution stopped]");
        outputPanel->setRunning(false);
        runAction->setEnabled(true);
        stopAction->setEnabled(false);
        statusBar()->showMessage("Execution stopped");
    }
}

void MainWindow::onTreeDoubleClicked(const QModelIndex &index)
{
    if (fileModel->isDir(index)) return;
    QString path = fileModel->filePath(index);
    loadFileToEditor(path);
}

void MainWindow::onTabCloseRequested(int index)
{
    auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(index));
    if (editor && editor->document()->isModified()) {
        auto ret = QMessageBox::warning(
            this, "Unsaved Changes",
            "Do you want to save changes to " + tabWidget->tabText(index) + "?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            int oldIdx = tabWidget->currentIndex();
            tabWidget->setCurrentIndex(index);
            QString path = filePaths[index];
            if (path.isEmpty())
                saveFileAs();
            else
                saveEditorToFile(index);
            tabWidget->setCurrentIndex(oldIdx);
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }
    filePaths.remove(index);
    tabWidget->removeTab(index);
    updateWindowTitle();
}

void MainWindow::onProcessStarted()
{
    outputPanel->appendOutput("[Running: " + currentRunFile + "]\n");
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    outputPanel->setRunning(false);
    runAction->setEnabled(true);
    stopAction->setEnabled(false);

    if (status == QProcess::CrashExit) {
        outputPanel->appendError("\n[Process crashed]");
        statusBar()->showMessage("Process crashed");
    } else if (exitCode != 0) {
        outputPanel->appendError("\n[Exited with code: " + QString::number(exitCode) + "]");
        statusBar()->showMessage("Finished with errors (exit code: " + QString::number(exitCode) + ")");
    } else {
        outputPanel->appendOutput("\n[Finished successfully]");
        statusBar()->showMessage("Finished successfully");
    }

    process->deleteLater();
    process = nullptr;
}

void MainWindow::onProcessOutput()
{
    if (process)
        outputPanel->appendOutput(QString::fromUtf8(process->readAllStandardOutput()));
}

void MainWindow::onProcessError()
{
    if (process)
        outputPanel->appendError(QString::fromUtf8(process->readAllStandardError()));
}

CodeEditor *MainWindow::currentEditor()
{
    return qobject_cast<CodeEditor *>(tabWidget->currentWidget());
}

QString MainWindow::currentFilePath()
{
    int idx = tabWidget->currentIndex();
    if (idx < 0) return "";
    return filePaths[idx];
}

void MainWindow::updateWindowTitle()
{
    QString title = "Elitra IDE";
    int idx = tabWidget->currentIndex();
    if (idx >= 0) {
        title += " - " + tabWidget->tabText(idx);
        auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(idx));
        if (editor && editor->document()->isModified())
            title += " *";
    }
    setWindowTitle(title);
}

QString MainWindow::readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";
    QTextStream in(&file);
    return in.readAll();
}

bool MainWindow::writeFile(const QString &path, const QString &content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}
