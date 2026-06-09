#include "outputpanel.h"
#include "colors.h"
#include <QHeaderView>
#include <QFileInfo>

OutputPanel::OutputPanel(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(
        "OutputPanel { background-color: #1a1a1e; }"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // ── Tab Bar ──
    tabBar = new QWidget;
    tabBar->setFixedHeight(28);
    tabBar->setStyleSheet("background-color: #242428;");
    auto *tabLayout = new QHBoxLayout(tabBar);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(0);

    tabLayout->addWidget(createTabBtn("OUTPUT", 0));
    tabLayout->addWidget(createTabBtn("PROBLEMS", 1));
    tabLayout->addStretch();

    clearBtn = new QPushButton("\u2715");
    clearBtn->setFixedSize(22, 22);
    clearBtn->setToolTip("Clear");
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #9e9e9e; border: none; font-size: 11px; }"
        "QPushButton:hover { color: #d4d4d4; }"
    );
    tabLayout->addWidget(clearBtn);

    layout->addWidget(tabBar);

    // ── Stack ──
    stack = new QStackedWidget;

    // Output page
    output = new QPlainTextEdit;
    output->setReadOnly(true);
    output->setFrameShape(QFrame::NoFrame);
    output->setStyleSheet(
        "QPlainTextEdit { background-color: #1a1a1e; color: #d4d4d4; "
        "border: none; padding: 6px 10px; font-family: 'JetBrains Mono', monospace; "
        "font-size: 12px; selection-background-color: #3d3d42; selection-color: #ffffff; }"
    );
    stack->addWidget(output);

    // Problems page
    problemTree = new QTreeWidget;
    problemTree->setFrameShape(QFrame::NoFrame);
    problemTree->setHeaderHidden(true);
    problemTree->setRootIsDecorated(false);
    problemTree->setAlternatingRowColors(false);
    problemTree->setIndentation(16);
    problemTree->setStyleSheet(
        "QTreeWidget { background-color: #1a1a1e; color: #d4d4d4; border: none; "
        "font-size: 12px; font-family: 'JetBrains Mono'; }"
        "QTreeWidget::item { padding: 2px 8px; }"
        "QTreeWidget::item:selected { background-color: #3d3d42; color: #ffffff; }"
    );
    stack->addWidget(problemTree);

    layout->addWidget(stack, 1);

    connect(clearBtn, &QPushButton::clicked, this, [this]() {
        if (activeTab == 0) clearOutput();
        else clearProblems();
    });

    connect(problemTree, &QTreeWidget::itemActivated,
            this, &OutputPanel::onProblemItemActivated);

    setActiveTab(0);
}

QPushButton *OutputPanel::createTabBtn(const QString &text, int index)
{
    auto *btn = new QPushButton(text);
    btn->setFixedHeight(28);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(
        "QPushButton { background: transparent; color: #9e9e9e; border: none; "
        "border-bottom: 2px solid transparent; padding: 0 14px; font-size: 11px; "
        "font-weight: 600; letter-spacing: 0.3px; }"
        "QPushButton:hover { color: #d4d4d4; }"
    );

    connect(btn, &QPushButton::clicked, this, [this, index]() {
        setActiveTab(index);
    });

    tabs.append({btn, index});
    return btn;
}

void OutputPanel::setActiveTab(int index)
{
    activeTab = index;
    stack->setCurrentIndex(index);

    for (const auto &t : tabs) {
        bool isActive = (t.index == index);
        t.btn->setStyleSheet(
            isActive
                ? "QPushButton { background: transparent; color: #d4d4d4; border: none; "
                  "border-bottom: 2px solid #e6a341; padding: 0 14px; font-size: 11px; "
                  "font-weight: 600; letter-spacing: 0.3px; }"
                : "QPushButton { background: transparent; color: #9e9e9e; border: none; "
                  "border-bottom: 2px solid transparent; padding: 0 14px; font-size: 11px; "
                  "font-weight: 600; letter-spacing: 0.3px; }"
            "QPushButton:hover { color: #d4d4d4; }"
        );
    }
}

void OutputPanel::appendOutput(const QString &text)
{
    output->appendPlainText(text);
}

void OutputPanel::appendError(const QString &text)
{
    output->appendHtml("<span style='color:#e66a41;'>" + text.toHtmlEscaped() + "</span>");
}

void OutputPanel::clearOutput()
{
    output->clear();
}

void OutputPanel::setRunning(bool running)
{
    if (running) {
        titleLabel->setText("RUNNING");
    }
}

void OutputPanel::addProblem(const QString &file, int line, int col,
                             const QString &message, const QString &severity)
{
    QColor color;
    if (severity == "1" || severity == "error") color = QColor(Colors::diagError);
    else if (severity == "2" || severity == "warning") color = QColor(Colors::diagWarning);
    else color = QColor(Colors::diagInfo);

    auto *item = new QTreeWidgetItem;
    item->setText(0, QString("[%1] %2:%3  %4")
                      .arg(severity.left(1).toUpper())
                      .arg(QFileInfo(file).fileName())
                      .arg(line + 1)
                      .arg(message));
    item->setForeground(0, color);
    item->setData(0, Qt::UserRole, file);
    item->setData(0, Qt::UserRole + 1, line);
    problemTree->addTopLevelItem(item);
}

void OutputPanel::clearProblems()
{
    problemTree->clear();
}

void OutputPanel::onProblemItemActivated(QTreeWidgetItem *item, int)
{
    QString file = item->data(0, Qt::UserRole).toString();
    int line = item->data(0, Qt::UserRole + 1).toInt();
    emit problemActivated(file, line);
}

void OutputPanel::onTabClicked(int)
{
}
