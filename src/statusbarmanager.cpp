#include "statusbarmanager.h"
#include "fileiconprovider.h"
#include <QFileInfo>

StatusBarManager::StatusBarManager(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(22);
    setStyleSheet(
        "StatusBarManager { background-color: #2d2d32; }"
    );

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(12);

    QString labelStyle =
        "QLabel { color: #9e9e9e; font-size: 11px; padding: 0 2px; }";

    leftLabel = new QLabel;
    leftLabel->setStyleSheet(labelStyle);
    layout->addWidget(leftLabel);

    gitLabel = new QLabel;
    gitLabel->setStyleSheet(labelStyle);
    layout->addWidget(gitLabel);

    layout->addStretch();

    modeLabel = new QLabel("MODE: NORMAL");
    modeLabel->setStyleSheet(labelStyle);
    layout->addWidget(modeLabel);

    encodingLabel = new QLabel("UTF-8");
    encodingLabel->setStyleSheet(labelStyle);
    layout->addWidget(encodingLabel);

    langLabel = new QLabel;
    langLabel->setStyleSheet(labelStyle);
    layout->addWidget(langLabel);

    positionLabel = new QLabel("Ln 1, Col 1");
    positionLabel->setStyleSheet(
        "QLabel { color: #9e9e9e; font-size: 11px; padding: 0 2px; }"
    );
    layout->addWidget(positionLabel);

    updateVisibility();
}

void StatusBarManager::setFileName(const QString &name, bool modified)
{
    if (name.isEmpty()) {
        leftLabel->clear();
        return;
    }

    QColor c = FileIconProvider::colorForFile(name);
    QString icon = FileIconProvider::iconLetterForFile(name);
    QString mod = modified ? " \u25CF" : "";

    leftLabel->setText(
        QString("<span style='color:%1; font-weight:600;'>%2</span> "
                "<span style='color:#d4d4d4;'>%3</span>"
                "<span style='color:#e6c341;'>%4</span>")
            .arg(c.name(), icon, QFileInfo(name).fileName(), mod));
}

void StatusBarManager::setCursorPosition(int line, int column)
{
    positionLabel->setText(QString("Ln %1, Col %2").arg(line + 1).arg(column + 1));
}

void StatusBarManager::setLanguage(const QString &lang)
{
    langLabel->setText(lang.toUpper());
}

void StatusBarManager::setEncoding(const QString &enc)
{
    encodingLabel->setText(enc);
}

void StatusBarManager::setGitBranch(const QString &branch)
{
    if (branch.isEmpty()) {
        gitLabel->clear();
    } else {
        gitLabel->setText(QString("\u2387 %1").arg(branch));
    }
    updateVisibility();
}

void StatusBarManager::setMode(const QString &mode)
{
    modeLabel->setText(mode.toUpper());
}

void StatusBarManager::updateVisibility()
{
    gitLabel->setVisible(!gitLabel->text().isEmpty());
}
