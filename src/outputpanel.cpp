#include "outputpanel.h"

OutputPanel::OutputPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *header = new QWidget;
    header->setStyleSheet("background-color: #1f2133; border-bottom: 1px solid #16171f;");
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(12, 5, 8, 5);

    titleLabel = new QLabel("Output");
    titleLabel->setStyleSheet("color: #565f89; font-weight: 600; font-size: 11px; "
                              "text-transform: uppercase; letter-spacing: 0.5px;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    stopBtn = new QPushButton(" Stop");
    stopBtn->setVisible(false);
    stopBtn->setStyleSheet(
        "QPushButton { background-color: #f7768e; color: #1a1b26; border: none; "
        "padding: 3px 12px; border-radius: 4px; font-size: 11px; font-weight: 600; }"
        "QPushButton:hover { background-color: #ff9eb0; }"
    );
    headerLayout->addWidget(stopBtn);

    clearBtn = new QPushButton("Clear");
    clearBtn->setStyleSheet(
        "QPushButton { background-color: transparent; color: #565f89; border: 1px solid #2f3346; "
        "padding: 3px 12px; border-radius: 4px; font-size: 11px; }"
        "QPushButton:hover { background-color: #2a2c3e; color: #a9b1d6; border-color: #3b4261; }"
    );
    headerLayout->addWidget(clearBtn);

    layout->addWidget(header);

    output = new QPlainTextEdit;
    output->setReadOnly(true);
    output->setStyleSheet(
        "QPlainTextEdit { background-color: #1a1b26; color: #a9b1d6; "
        "border: none; padding: 8px 12px; font-family: 'JetBrains Mono', 'Consolas', monospace; "
        "font-size: 12px; selection-background-color: #e5a06d; selection-color: #1a1b26; }"
    );
    layout->addWidget(output);

    connect(clearBtn, &QPushButton::clicked, this, &OutputPanel::clearOutput);
    connect(stopBtn, &QPushButton::clicked, this, &OutputPanel::stopRequested);
}

void OutputPanel::appendOutput(const QString &text)
{
    output->appendPlainText(text);
}

void OutputPanel::appendError(const QString &text)
{
    output->appendHtml("<span style='color:#f7768e;'>" + text.toHtmlEscaped() + "</span>");
}

void OutputPanel::clearOutput()
{
    output->clear();
}

void OutputPanel::setRunning(bool running)
{
    stopBtn->setVisible(running);
    if (!running)
        titleLabel->setText("Output");
    else
        titleLabel->setText("Running...");
}
