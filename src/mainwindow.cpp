#include "mainwindow.h"
#include "codeeditor.h"
#include "outputpanel.h"
#include "elitrahighlighter.h"
#include "languagehighlighter.h"
#include "findreplacepanel.h"
#include "findinfilespanel.h"
#include "lspclient.h"
#include "settingsdialog.h"
#include "activitybar.h"
#include "sidebar.h"
#include "statusbarmanager.h"
#include "fileiconprovider.h"
#include "colors.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QFont>
#include <QShortcut>
#include <QJsonArray>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), process(nullptr), lspClient(nullptr)
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

    lspClient = new LspClient(this);

    setupUI();
    setupFindReplace();
    setupFindInFiles();
    setupLsp();
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

    // ── Central Widget Layout ──
    auto *central = new QWidget;
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Activity Bar
    activityBar = new ActivityBar;
    mainLayout->addWidget(activityBar);

    // Sidebar
    sidebar = new Sidebar;
    sidebar->setRootPath(workspaceDir);
    connect(sidebar->fileTree(), &QTreeView::doubleClicked,
            this, &MainWindow::onTreeDoubleClicked);
    connect(sidebar, &Sidebar::closeRequested, this, [this]() {
        sidebar->setVisible(false);
    });
    mainLayout->addWidget(sidebar);

    // Editor + Panel area
    auto *editorArea = new QWidget;
    auto *editorAreaLayout = new QVBoxLayout(editorArea);
    editorAreaLayout->setContentsMargins(0, 0, 0, 0);
    editorAreaLayout->setSpacing(0);

    // Find/Replace panels (hidden by default)
    findReplacePanel = new FindReplacePanel;
    findReplacePanel->setVisible(false);
    editorAreaLayout->addWidget(findReplacePanel);

    findInFilesPanel = new FindInFilesPanel;
    findInFilesPanel->setVisible(false);
    editorAreaLayout->addWidget(findInFilesPanel);

    // Tab widget
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setDocumentMode(true);
    tabWidget->setElideMode(Qt::ElideRight);
    connect(tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::onTabCloseRequested);

    // Output panel with tabs
    outputPanel = new OutputPanel;
    connect(outputPanel, &OutputPanel::stopRequested,
            this, &MainWindow::stopRun);
    connect(outputPanel, &OutputPanel::problemActivated,
            this, [this](const QString &path, int line) {
        loadFileToEditor(path);
        auto *editor = currentEditor();
        if (editor) {
            QTextCursor c(editor->document()->findBlockByNumber(line));
            editor->setTextCursor(c);
            editor->centerCursor();
        }
    });

    // Vertical splitter: editor on top, output on bottom
    auto *editorSplitter = new QSplitter(Qt::Vertical);
    editorSplitter->addWidget(tabWidget);
    editorSplitter->addWidget(outputPanel);
    editorSplitter->setStretchFactor(0, 3);
    editorSplitter->setStretchFactor(1, 1);
    editorSplitter->setHandleWidth(1);

    editorAreaLayout->addWidget(editorSplitter, 1);
    mainLayout->addWidget(editorArea, 1);

    setCentralWidget(central);

    // ── Status Bar ──
    statusBarManager = new StatusBarManager;
    statusBar()->addPermanentWidget(statusBarManager, 1);
    statusBar()->setStyleSheet(
        "QStatusBar { background-color: #2d2d32; border: none; padding: 0; }"
        "QStatusBar::item { border: none; }"
    );
    statusBar()->setSizeGripEnabled(true);

    // ── Connect Activity Bar ──
    connect(activityBar, &ActivityBar::viewToggled,
            this, &MainWindow::onViewToggled);

    // ── Actions ──
    setupMenuBar();
    setupActions();

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
    editMenu->addAction("&Undo", QKeySequence::Undo, this, [this]() {
        if (auto *e = currentEditor()) e->undo();
    });
    editMenu->addAction("&Redo", QKeySequence("Ctrl+Y"), this, [this]() {
        if (auto *e = currentEditor()) e->redo();
    });
    editMenu->addSeparator();
    editMenu->addAction("Cu&t", QKeySequence::Cut, this, [this]() {
        if (auto *e = currentEditor()) e->cut();
    });
    editMenu->addAction("&Copy", QKeySequence::Copy, this, [this]() {
        if (auto *e = currentEditor()) e->copy();
    });
    editMenu->addAction("&Paste", QKeySequence::Paste, this, [this]() {
        if (auto *e = currentEditor()) e->paste();
    });
    editMenu->addSeparator();
    editMenu->addAction("Select &All", QKeySequence::SelectAll, this, [this]() {
        if (auto *e = currentEditor()) e->selectAll();
    });
    editMenu->addSeparator();
    editMenu->addAction("&Find", QKeySequence::Find, this, &MainWindow::showFind);
    editMenu->addAction("F&ind/Replace", QKeySequence("Ctrl+H"), this, &MainWindow::showReplace);
    editMenu->addAction("Find in &Files", QKeySequence("Ctrl+Shift+F"), this, &MainWindow::showFindInFiles);
    editMenu->addAction("&Go to Line...", QKeySequence("Ctrl+G"), this, &MainWindow::showGoToLine);

    auto *runMenu = menuBar()->addMenu("&Run");
    runAction = runMenu->addAction("&Run File", QKeySequence("F5"), this, &MainWindow::runFile);
    stopAction = runMenu->addAction("&Stop", QKeySequence("Shift+F5"), this, &MainWindow::stopRun);
    stopAction->setEnabled(false);
    runMenu->addSeparator();
    runMenu->addAction("&Restart LSP Server", QKeySequence("Ctrl+Shift+L"), this, [this]() {
        setupLsp();
    });

    auto *toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction("&Settings...", QKeySequence("Ctrl+,"), this, &MainWindow::showSettings);
}

void MainWindow::setupActions()
{
    // Keyboard shortcuts that weren't in menus
    auto *focusExplorer = new QShortcut(QKeySequence("Ctrl+Shift+E"), this);
    connect(focusExplorer, &QShortcut::activated, this, [this]() {
        activityBar->setActive(ActivityBar::Explorer);
        sidebar->setVisible(true);
        sidebar->fileTree()->setFocus();
    });

    auto *toggleSidebar = new QShortcut(QKeySequence("Ctrl+B"), this);
    connect(toggleSidebar, &QShortcut::activated, this, [this]() {
        sidebar->setVisible(!sidebar->isVisible());
    });
}

void MainWindow::setupStyleSheet()
{
    setStyleSheet(
        "QMainWindow { background-color: #1a1a1e; }"

        "QMenuBar { background-color: #242428; color: #d4d4d4; border-bottom: 1px solid #2d2d32; "
        "  font-size: 12px; padding: 1px 0; }"
        "QMenuBar::item { padding: 4px 10px; background: transparent; }"
        "QMenuBar::item:selected { background-color: #3d3d42; }"

        "QMenu { background-color: #2d2d32; color: #d4d4d4; border: 1px solid #3d3d42; "
        "  padding: 4px; }"
        "QMenu::item { padding: 5px 28px 5px 12px; }"
        "QMenu::item:selected { background-color: #3d3d42; color: #ffffff; }"
        "QMenu::separator { height: 1px; background: #3d3d42; margin: 4px 8px; }"

        "QTabWidget::pane { border: none; background-color: #1a1a1e; }"
        "QTabBar { background-color: #242428; border: none; }"
        "QTabBar::tab { background-color: #242428; color: #9e9e9e; border: none; "
        "  padding: 6px 14px; font-size: 12px; border-right: 1px solid #2d2d32; }"
        "QTabBar::tab:selected { background-color: #1a1a1e; color: #d4d4d4; "
        "  border-top: none; }"
        "QTabBar::tab:hover:!selected { background-color: #35353a; color: #d4d4d4; }"
        "QTabBar::close-button { image: none; width: 18px; }"
        "QTabBar QToolButton { background: transparent; color: #9e9e9e; border: none; }"

        "QSplitter::handle { background-color: #2d2d32; }"

        "QScrollBar:vertical { background: #1a1a1e; width: 8px; margin: 0; }"
        "QScrollBar::handle:vertical { background: #3d3d42; min-height: 30px; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #e6a341; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar:horizontal { background: #1a1a1e; height: 8px; margin: 0; }"
        "QScrollBar::handle:horizontal { background: #3d3d42; min-width: 30px; border-radius: 4px; }"
        "QScrollBar::handle:horizontal:hover { background: #e6a341; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }"
    );
}

void MainWindow::setupFindReplace()
{
    connect(findReplacePanel, &FindReplacePanel::closed, this, [this]() {
        findReplacePanel->setVisible(false);
        if (auto *e = currentEditor()) e->setFocus();
    });
}

void MainWindow::setupFindInFiles()
{
    findInFilesPanel->setSearchRoot(workspaceDir);
    connect(findInFilesPanel, &FindInFilesPanel::closed, this, [this]() {
        findInFilesPanel->setVisible(false);
        if (auto *e = currentEditor()) e->setFocus();
    });
    connect(findInFilesPanel, &FindInFilesPanel::fileActivated,
            this, [this](const QString &path, int line) {
        loadFileToEditor(path);
        auto *editor = currentEditor();
        if (editor) {
            QTextCursor c(editor->document()->findBlockByNumber(line - 1));
            editor->setTextCursor(c);
            editor->centerCursor();
        }
    });
}

void MainWindow::setupLsp()
{
    if (!lspClient) lspClient = new LspClient(this);

    QString lspPath = elitraBinary;
    if (QFileInfo::exists(lspPath + "-lsp"))
        lspPath += "-lsp";
    else if (QFileInfo::exists(QDir::homePath() + "/.local/bin/elitra-lsp"))
        lspPath = QDir::homePath() + "/.local/bin/elitra-lsp";
    else
        lspPath.clear();

    if (lspPath.isEmpty()) {
        statusBar()->showMessage("LSP server not found");
        lspEnabled = false;
        return;
    }

    bool ok = lspClient->start(lspPath, workspaceDir);
    if (ok) {
        lspEnabled = true;
        outputPanel->appendOutput("[LSP] Server started: " + lspPath + "\n");

        connect(lspClient, &LspClient::diagnosticsReady, this,
            &MainWindow::updateLspDiagnostics);

        connect(lspClient, &LspClient::serverError, this,
            [this](const QString &err) {
                outputPanel->appendError("[LSP] " + err);
            });

        for (auto it = filePaths.begin(); it != filePaths.end(); ++it) {
            QString path = it.value();
            if (!path.isEmpty()) {
                auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(it.key()));
                if (editor)
                    lspClient->openDocument(editorUri(path), "elitra", editor->toPlainText());
            }
            if (lspClient)
                lspClient->setEditor(currentEditor());
        }
    } else {
        lspEnabled = false;
        statusBar()->showMessage("Failed to start LSP server");
    }
}

QString MainWindow::editorUri(const QString &filePath)
{
    return "file://" + filePath;
}

void MainWindow::updateLspDiagnostics(const QString &uri, const QJsonArray &diagnostics)
{
    outputPanel->clearProblems();
    QString filePath = uri.mid(7);
    for (auto it = filePaths.begin(); it != filePaths.end(); ++it) {
        if (it.value() == filePath) {
            auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(it.key()));
            if (!editor) return;

            QVector<CodeEditor::DiagMarker> markers;
            for (const QJsonValue &v : diagnostics) {
                QJsonObject d = v.toObject();
                QJsonObject range = d["range"].toObject();
                QJsonObject start = range["start"].toObject();
                int line = start["line"].toInt();
                int col = start["character"].toInt();
                QString msg = d["message"].toString();
                QString sev = QString::number(d["severity"].toInt());
                QColor color = sev == "1" ? QColor(Colors::diagError)
                             : sev == "2" ? QColor(Colors::diagWarning)
                             : QColor(Colors::diagInfo);
                markers.append({line, msg, color});
                outputPanel->addProblem(filePath, line, col, msg, sev);
            }
            editor->setDiagnostics(markers);
            return;
        }
    }
}

/* ── Activity Bar ── */

void MainWindow::onViewToggled(ActivityBar::View view)
{
    switch (view) {
    case ActivityBar::Explorer:
        sidebar->setVisible(!sidebar->isVisible());
        if (sidebar->isVisible()) {
            activityBar->setActive(ActivityBar::Explorer);
        }
        break;
    case ActivityBar::Search:
        sidebar->setVisible(true);
        showFindInFiles();
        break;
    case ActivityBar::Run:
        runFile();
        break;
    case ActivityBar::Settings:
        showSettings();
        break;
    default:
        break;
    }
}

/* ── File Operations ── */

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

    connect(editor, &CodeEditor::findShortcut, this, &MainWindow::showFind);
    connect(editor, &CodeEditor::replaceShortcut, this, &MainWindow::showReplace);
    connect(editor, &CodeEditor::goToLineShortcut, this, &MainWindow::showGoToLine);
    connect(editor, &CodeEditor::cursorMoved, this, &MainWindow::onCursorMoved);
    updateStatusBar();
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Open File", workspaceDir,
        "All Supported (*.eltr *.cpp *.h *.rs *.py *.js *.css *.html *.lua *.json *.md);;All Files (*)");
    if (path.isEmpty()) return;
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
    {
        auto *hl = createHighlighterForFile(filePath, editor->document());
        if (!hl) new ElitraHighlighter(editor->document());
    }
    editor->setPlainText(content);

    QFileInfo fi(filePath);
    QString tabName = fi.fileName();
    QString tooltip = filePath;

    // Use icon color for tab marker
    QColor iconColor = FileIconProvider::colorForFile(filePath);

    int index = tabWidget->addTab(editor, tabName);
    tabWidget->setTabToolTip(index, tooltip);
    tabWidget->setCurrentIndex(index);
    filePaths[index] = filePath;

    editor->document()->setModified(false);
    connect(editor->document(), &QTextDocument::modificationChanged,
            this, [this]() { updateWindowTitle(); updateStatusBar(); });
    connect(editor, &CodeEditor::findShortcut, this, &MainWindow::showFind);
    connect(editor, &CodeEditor::replaceShortcut, this, &MainWindow::showReplace);
    connect(editor, &CodeEditor::goToLineShortcut, this, &MainWindow::showGoToLine);
    connect(editor, &CodeEditor::cursorMoved, this, &MainWindow::onCursorMoved);

    if (lspEnabled && lspClient) {
        lspClient->openDocument(editorUri(filePath), "elitra", content);
        lspClient->setEditor(editor);
    }

    statusBar()->showMessage("Opened: " + filePath, 5000);
    updateWindowTitle();
    updateStatusBar();
}

void MainWindow::saveFile()
{
    int idx = tabWidget->currentIndex();
    if (idx < 0) return;
    QString path = filePaths[idx];
    if (path.isEmpty()) { saveFileAs(); return; }
    saveEditorToFile(idx);
}

void MainWindow::saveFileAs()
{
    int idx = tabWidget->currentIndex();
    if (idx < 0) return;
    QString path = QFileDialog::getSaveFileName(this, "Save File", workspaceDir,
        "Elitra Files (*.eltr);;All Files (*)");
    if (path.isEmpty()) return;
    if (!path.endsWith(".eltr")) path += ".eltr";
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
        updateStatusBar();
        if (lspEnabled && lspClient)
            lspClient->changeDocument(editorUri(path), editor->toPlainText(), 1);
        return true;
    }
    return false;
}

bool MainWindow::maybeSave()
{
    for (auto it = filePaths.begin(); it != filePaths.end(); ++it) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(it.key()));
        if (editor && editor->document()->isModified()) {
            auto ret = QMessageBox::warning(this, "Unsaved Changes",
                "There are unsaved changes. Do you want to save before continuing?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            if (ret == QMessageBox::Save) {
                if (it.value().isEmpty()) { saveFileAs(); if (filePaths[it.key()].isEmpty()) return false; }
                else { saveEditorToFile(it.key()); }
            } else if (ret == QMessageBox::Cancel) { return false; }
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
    if (idx < 0) { statusBar()->showMessage("No file to run"); return; }
    QString path = filePaths[idx];
    if (path.isEmpty()) { saveFileAs(); path = filePaths[tabWidget->currentIndex()]; if (path.isEmpty()) return; }
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
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::onProcessOutput);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::onProcessError);

    process->start(elitraBinary, {path});
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
    if (sidebar->fileModel()->isDir(index)) return;
    loadFileToEditor(sidebar->fileModel()->filePath(index));
}

void MainWindow::onTabCloseRequested(int index)
{
    auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(index));
    if (editor && editor->document()->isModified()) {
        auto ret = QMessageBox::warning(this, "Unsaved Changes",
            "Do you want to save changes to " + tabWidget->tabText(index) + "?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            int oldIdx = tabWidget->currentIndex();
            tabWidget->setCurrentIndex(index);
            QString path = filePaths[index];
            if (path.isEmpty()) saveFileAs(); else saveEditorToFile(index);
            tabWidget->setCurrentIndex(oldIdx);
        } else if (ret == QMessageBox::Cancel) { return; }
    }

    if (lspEnabled && lspClient && !filePaths[index].isEmpty())
        lspClient->closeDocument(editorUri(filePaths[index]));

    filePaths.remove(index);
    tabWidget->removeTab(index);
    updateWindowTitle();
    updateStatusBar();
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
    if (process) outputPanel->appendOutput(QString::fromUtf8(process->readAllStandardOutput()));
}

void MainWindow::onProcessError()
{
    if (process) outputPanel->appendError(QString::fromUtf8(process->readAllStandardError()));
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
        if (editor && editor->document()->isModified()) title += " *";
    }
    setWindowTitle(title);
}

void MainWindow::updateStatusBar()
{
    auto *editor = currentEditor();
    if (!editor) { statusBarManager->setFileName("", false); return; }

    QString path = currentFilePath();
    if (path.isEmpty()) {
        statusBarManager->setFileName(tabWidget->tabText(tabWidget->currentIndex()), false);
        statusBarManager->setLanguage("Elitra");
    } else {
        statusBarManager->setFileName(path, editor->document()->isModified());
        QString ext = QFileInfo(path).suffix().toLower();
        QString lang = ext == "eltr" ? "Elitra"
                     : ext == "cpp" || ext == "c" || ext == "h" ? "C++"
                     : ext == "rs" ? "Rust"
                     : ext == "py" ? "Python"
                     : ext == "js" ? "JavaScript"
                     : ext == "css" ? "CSS"
                     : ext == "html" ? "HTML"
                     : ext == "lua" ? "Lua"
                     : "Text";
        statusBarManager->setLanguage(lang);
    }

    QTextCursor c = editor->textCursor();
    statusBarManager->setCursorPosition(c.blockNumber(), c.columnNumber());
    statusBarManager->setEncoding("UTF-8");
}

void MainWindow::onCursorMoved(int line, int col)
{
    statusBarManager->setCursorPosition(line, col);
}

/* ── Find / Replace / Go To Line / Settings ── */

void MainWindow::showFind()
{
    auto *editor = currentEditor();
    if (!editor) return;
    findReplacePanel->setEditor(editor);
    findInFilesPanel->setVisible(false);
    findReplacePanel->showFind();
}

void MainWindow::showReplace()
{
    auto *editor = currentEditor();
    if (!editor) return;
    findReplacePanel->setEditor(editor);
    findInFilesPanel->setVisible(false);
    findReplacePanel->showReplace();
}

void MainWindow::showFindInFiles()
{
    findReplacePanel->setVisible(false);
    findInFilesPanel->setVisible(true);
    findInFilesPanel->focusSearch();
}

void MainWindow::showGoToLine()
{
    auto *editor = currentEditor();
    if (!editor) return;
    bool ok;
    int line = QInputDialog::getInt(this, "Go to Line",
        "Line number (1-" + QString::number(editor->document()->blockCount()) + "):",
        1, 1, editor->document()->blockCount(), 1, &ok);
    if (!ok) return;

    QTextBlock block = editor->document()->findBlockByNumber(line - 1);
    if (block.isValid()) {
        QTextCursor cursor(block);
        editor->setTextCursor(cursor);
        editor->centerCursor();
        statusBar()->showMessage("Go to line " + QString::number(line));
    }
}

void MainWindow::showSettings()
{
    auto *dialog = new SettingsDialog(this);
    auto *editor = currentEditor();
    if (editor) {
        dialog->setFontSize(editor->defaultFontSize());
        dialog->setFontFamily(editor->font().family());
    }
    connect(dialog, &SettingsDialog::settingsApplied, this, &MainWindow::applySettings);
    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::applySettings()
{
    auto *dialog = qobject_cast<SettingsDialog *>(sender());
    if (!dialog) return;
    for (int i = 0; i < tabWidget->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget->widget(i));
        if (!editor) continue;
        QFont font(dialog->fontFamily(), dialog->fontSize());
        font.setStyleHint(QFont::Monospace);
        editor->setFont(font);
        editor->setTabStopDistance(editor->fontMetrics().horizontalAdvance(' ') * dialog->tabSize());
        editor->setDefaultFontSize(dialog->fontSize());
    }
    statusBar()->showMessage("Settings applied");
}

/* ── IO ── */

QString MainWindow::readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "";
    QTextStream in(&file);
    return in.readAll();
}

bool MainWindow::writeFile(const QString &path, const QString &content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}
