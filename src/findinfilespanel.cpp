#include "findinfilespanel.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QRegularExpression>
#include <QHeaderView>
#include <QCoreApplication>

FindInFilesPanel::FindInFilesPanel(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(
        "FindInFilesPanel { background-color: #242428; border-bottom: 1px solid #2d2d32; }"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(4);

    auto *topRow = new QHBoxLayout;

    searchInput = new QLineEdit;
    searchInput->setPlaceholderText("Search in files...");
    searchInput->setStyleSheet(
        "QLineEdit { background-color: #1a1a1e; color: #d4d4d4; border: 1px solid #3d3d42; "
        "padding: 4px 8px; border-radius: 4px; font-size: 12px; }"
        "QLineEdit:focus { border-color: #e6a341; }"
    );

    searchBtn = new QPushButton("Search");
    searchBtn->setStyleSheet(
        "QPushButton { background-color: #e6a341; color: #ffffff; border: none; "
        "padding: 4px 14px; border-radius: 4px; font-size: 11px; font-weight: 600; }"
        "QPushButton:hover { background-color: #f0b84d; }"
    );

    closeBtn = new QPushButton("\u2715");
    closeBtn->setFixedSize(22, 22);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: transparent; color: #9e9e9e; border: none; "
        "font-size: 13px; } QPushButton:hover { color: #e66a41; }");

    topRow->addWidget(searchInput, 1);
    topRow->addWidget(searchBtn);
    topRow->addWidget(closeBtn);
    layout->addLayout(topRow);

    auto *optRow = new QHBoxLayout;
    filterInput = new QLineEdit;
    filterInput->setPlaceholderText("File filter (e.g. *.eltr,*.cpp)");
    filterInput->setStyleSheet(
        "QLineEdit { background-color: #1a1a1e; color: #d4d4d4; border: 1px solid #3d3d42; "
        "padding: 3px 8px; border-radius: 4px; font-size: 11px; }"
    );

    caseSensitive = new QCheckBox("Aa");
    wholeWord = new QCheckBox("W");
    QString cbStyle =
        "QCheckBox { color: #9e9e9e; font-size: 11px; font-weight: 600; spacing: 3px; }"
        "QCheckBox::indicator { width: 14px; height: 14px; border: 1px solid #3d3d42; "
        "border-radius: 3px; background: #1a1a1e; }"
        "QCheckBox::indicator:checked { background: #e6a341; border-color: #e6a341; }";
    caseSensitive->setStyleSheet(cbStyle);
    wholeWord->setStyleSheet(cbStyle);

    optRow->addWidget(filterInput, 1);
    optRow->addWidget(caseSensitive);
    optRow->addWidget(wholeWord);
    layout->addLayout(optRow);

    resultsTree = new QTreeWidget;
    resultsTree->setHeaderLabels({"File", "Line", "Content"});
    resultsTree->setRootIsDecorated(false);
    resultsTree->setAlternatingRowColors(true);
    resultsTree->header()->setStretchLastSection(true);
    resultsTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    resultsTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    resultsTree->setStyleSheet(
        "QTreeWidget { background-color: #1a1a1e; color: #d4d4d4; border: 1px solid #3d3d42; "
        "border-radius: 4px; font-size: 12px; font-family: 'JetBrains Mono'; }"
        "QTreeWidget::item { padding: 2px 4px; }"
        "QTreeWidget::item:selected { background-color: #3d3d42; color: #ffffff; }"
        "QTreeWidget::item:alternate { background-color: #242428; }"
        "QHeaderView::section { background-color: #242428; color: #9e9e9e; "
        "border: none; padding: 4px; font-size: 11px; }"
    );

    statusLabel = new QLabel;
    statusLabel->setStyleSheet("color: #9e9e9e; font-size: 11px; padding: 2px 0;");

    layout->addWidget(resultsTree, 1);
    layout->addWidget(statusLabel);

    connect(searchInput, &QLineEdit::returnPressed, this, &FindInFilesPanel::startSearch);
    connect(searchBtn, &QPushButton::clicked, this, &FindInFilesPanel::startSearch);
    connect(closeBtn, &QPushButton::clicked, this, &FindInFilesPanel::closed);
    connect(resultsTree, &QTreeWidget::itemActivated,
            this, &FindInFilesPanel::onItemActivated);
}

void FindInFilesPanel::setSearchRoot(const QString &root)
{
    searchRoot = root;
}

void FindInFilesPanel::focusSearch()
{
    searchInput->setFocus();
    searchInput->selectAll();
}

void FindInFilesPanel::startSearch()
{
    QString query = searchInput->text();
    if (query.isEmpty()) return;

    resultsTree->clear();
    statusLabel->setText("Searching...");
    QCoreApplication::processEvents();

    Qt::CaseSensitivity cs = caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    QStringList nameFilters;
    QString filter = filterInput->text().trimmed();
    if (!filter.isEmpty()) {
        nameFilters = filter.split(";", Qt::SkipEmptyParts);
    } else {
        nameFilters << "*.eltr" << "*.cpp" << "*.h" << "*.json" << "*.md";
    }

    QDirIterator it(searchRoot, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    int totalResults = 0;
    int maxResults = 500;

    while (it.hasNext() && totalResults < maxResults) {
        QString filePath = it.next();
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

        QTextStream in(&file);
        int lineNum = 0;
        while (!in.atEnd() && totalResults < maxResults) {
            QString line = in.readLine();
            lineNum++;

            bool match;
            if (wholeWord->isChecked()) {
                QRegularExpression re("\\b" + QRegularExpression::escape(query) + "\\b");
                if (cs == Qt::CaseInsensitive)
                    re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
                match = re.match(line).hasMatch();
            } else {
                if (cs == Qt::CaseSensitive)
                    match = line.contains(query);
                else
                    match = line.contains(query, Qt::CaseInsensitive);
            }

            if (match) {
                auto *item = new QTreeWidgetItem;
                QFileInfo fi(filePath);
                item->setText(0, fi.fileName());
                item->setText(1, QString::number(lineNum));
                QString display = line.trimmed().left(120);
                if (line.trimmed().length() > 120) display += "...";

                int idx = -1;
                if (cs == Qt::CaseSensitive)
                    idx = display.indexOf(query);
                else
                    idx = display.indexOf(query, 0, Qt::CaseInsensitive);

                if (idx >= 0) {
                    item->setText(2, display);
                    // Highlight would require custom delegate, skip for now
                } else {
                    item->setText(2, display);
                }

                item->setData(0, Qt::UserRole, filePath);
                item->setData(0, Qt::UserRole + 1, lineNum);
                resultsTree->addTopLevelItem(item);
                totalResults++;
            }
        }
        file.close();
    }

    statusLabel->setText(QString("Found %1 results").arg(totalResults));
}

void FindInFilesPanel::onItemActivated(QTreeWidgetItem *item, int)
{
    QString filePath = item->data(0, Qt::UserRole).toString();
    int line = item->data(0, Qt::UserRole + 1).toInt();
    emit fileActivated(filePath, line);
}
