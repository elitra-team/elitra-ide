#include "sidebar.h"
#include "fileiconprovider.h"
#include <QHeaderView>
#include <QDir>

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(260);
    setStyleSheet(
        "Sidebar { background-color: #242428; }"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // ── Header ──
    auto *header = new QWidget;
    header->setFixedHeight(35);
    header->setStyleSheet("background-color: #242428;");
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(14, 0, 8, 0);

    titleLabel = new QLabel("EXPLORER");
    titleLabel->setStyleSheet(
        "color: #9e9e9e; font-size: 11px; font-weight: 600; "
        "letter-spacing: 0.8px;"
    );
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    closeBtn = new QPushButton("\u2715");
    closeBtn->setFixedSize(22, 22);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #9e9e9e; border: none; font-size: 11px; }"
        "QPushButton:hover { color: #e66a41; }"
    );
    headerLayout->addWidget(closeBtn);

    layout->addWidget(header);

    // ── Separator ──
    auto *sep = new QWidget;
    sep->setFixedHeight(1);
    sep->setStyleSheet("background-color: #2d2d32;");
    layout->addWidget(sep);

    // ── Stack ──
    stack = new QStackedWidget;
    stack->setStyleSheet("background-color: #242428;");

    // File tree page
    auto *explorerPage = new QWidget;
    auto *explorerLayout = new QVBoxLayout(explorerPage);
    explorerLayout->setContentsMargins(0, 0, 0, 0);

    fileModel_ = new QFileSystemModel(this);
    fileModel_->setNameFilters({"*.eltr", "*.json", "*.md", "*.cpp", "*.h",
                                "*.py", "*.rs", "*.js", "*.css", "*.html", "*.lua"});
    fileModel_->setNameFilterDisables(false);

    fileTree_ = new QTreeView;
    fileTree_->setModel(fileModel_);
    fileTree_->setAnimated(true);
    fileTree_->setIndentation(12);
    fileTree_->setHeaderHidden(true);
    fileTree_->hideColumn(1);
    fileTree_->hideColumn(2);
    fileTree_->hideColumn(3);
    fileTree_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fileTree_->setFrameShape(QFrame::NoFrame);
    fileTree_->setStyleSheet(
        "QTreeView { background-color: #242428; border: none; color: #d4d4d4; font-size: 12px; }"
        "QTreeView::item { padding: 3px 4px; border: none; }"
        "QTreeView::item:selected { background-color: #3d3d42; color: #ffffff; }"
        "QTreeView::item:hover { background-color: #35353a; }"
        "QTreeView::branch { background: transparent; }"
    );

    explorerLayout->addWidget(fileTree_);
    stack->addWidget(explorerPage);

    // Search page (placeholder filled by MainWindow)
    searchContainer = new QWidget;
    stack->addWidget(searchContainer);

    layout->addWidget(stack, 1);

    connect(closeBtn, &QPushButton::clicked, this, &Sidebar::closeRequested);
}

void Sidebar::setRootPath(const QString &path)
{
    fileModel_->setRootPath(path);
    fileTree_->setRootIndex(fileModel_->index(path));
}

void Sidebar::setSearchView(QWidget *searchWidget)
{
    auto *layout = new QVBoxLayout(searchContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(searchWidget);
    // Switch to search page is done externally
}
