#include "findreplacepanel.h"
#include "codeeditor.h"
#include <QRegularExpression>

FindReplacePanel::FindReplacePanel(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(
        "FindReplacePanel { background-color: #242428; border-bottom: 1px solid #2d2d32; }"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(4);

    auto *findRow = new QHBoxLayout;
    findInput = new QLineEdit;
    findInput->setPlaceholderText("Find...");
    findInput->setStyleSheet(
        "QLineEdit { background-color: #1a1a1e; color: #d4d4d4; border: 1px solid #3d3d42; "
        "padding: 4px 8px; border-radius: 4px; font-size: 12px; }"
        "QLineEdit:focus { border-color: #e6a341; }"
    );

    matchCount = new QLabel;
    matchCount->setStyleSheet("color: #9e9e9e; font-size: 11px; padding: 0 6px;");

    prevBtn = new QPushButton("\u25B2");
    prevBtn->setFixedSize(26, 26);
    prevBtn->setToolTip("Previous (Shift+Enter)");
    nextBtn = new QPushButton("\u25BC");
    nextBtn->setFixedSize(26, 26);
    nextBtn->setToolTip("Next (Enter)");
    closeBtn = new QPushButton("\u2715");
    closeBtn->setFixedSize(22, 22);
    closeBtn->setToolTip("Close (Esc)");
    QString btnStyle =
        "QPushButton { background-color: transparent; color: #d4d4d4; border: 1px solid #3d3d42; "
        "border-radius: 4px; font-size: 11px; }"
        "QPushButton:hover { background-color: #3d3d42; border-color: #e6a341; }";
    prevBtn->setStyleSheet(btnStyle);
    nextBtn->setStyleSheet(btnStyle);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: transparent; color: #9e9e9e; border: none; "
        "font-size: 13px; } QPushButton:hover { color: #e66a41; }");

    findRow->addWidget(findInput, 1);
    findRow->addWidget(matchCount);
    findRow->addWidget(prevBtn);
    findRow->addWidget(nextBtn);
    findRow->addWidget(closeBtn);
    layout->addLayout(findRow);

    replaceRow = new QWidget;
    auto *repRowL = new QHBoxLayout(replaceRow);
    repRowL->setContentsMargins(0, 0, 0, 0);

    replaceInput = new QLineEdit;
    replaceInput->setPlaceholderText("Replace...");
    replaceInput->setStyleSheet(findInput->styleSheet());

    replaceBtn = new QPushButton("Replace");
    replaceAllBtn = new QPushButton("Replace All");
    replaceBtn->setStyleSheet(btnStyle);
    replaceAllBtn->setStyleSheet(btnStyle);

    repRowL->addWidget(replaceInput, 1);
    repRowL->addWidget(replaceBtn);
    repRowL->addWidget(replaceAllBtn);
    replaceRow->setVisible(false);
    layout->addWidget(replaceRow);

    auto *optRow = new QHBoxLayout;
    caseSensitive = new QCheckBox("Aa");
    caseSensitive->setToolTip("Case sensitive");
    wholeWord = new QCheckBox("W");
    wholeWord->setToolTip("Whole word");
    regexMode = new QCheckBox(".*");
    regexMode->setToolTip("Regular expression");
    QString cbStyle =
        "QCheckBox { color: #9e9e9e; font-size: 11px; font-weight: 600; spacing: 3px; }"
        "QCheckBox::indicator { width: 14px; height: 14px; border: 1px solid #3d3d42; "
        "border-radius: 3px; background: #1a1a1e; }"
        "QCheckBox::indicator:checked { background: #e6a341; border-color: #e6a341; }";
    caseSensitive->setStyleSheet(cbStyle);
    wholeWord->setStyleSheet(cbStyle);
    regexMode->setStyleSheet(cbStyle);

    optRow->addWidget(caseSensitive);
    optRow->addWidget(wholeWord);
    optRow->addWidget(regexMode);
    optRow->addStretch();
    layout->addLayout(optRow);

    connect(findInput, &QLineEdit::textChanged, this, &FindReplacePanel::onTextChanged);
    connect(findInput, &QLineEdit::returnPressed, this, &FindReplacePanel::findNext);
    connect(prevBtn, &QPushButton::clicked, this, &FindReplacePanel::findPrev);
    connect(nextBtn, &QPushButton::clicked, this, &FindReplacePanel::findNext);
    connect(closeBtn, &QPushButton::clicked, this, &FindReplacePanel::closed);
    connect(replaceBtn, &QPushButton::clicked, this, &FindReplacePanel::replace);
    connect(replaceAllBtn, &QPushButton::clicked, this, &FindReplacePanel::replaceAll);
    connect(replaceInput, &QLineEdit::returnPressed, this, &FindReplacePanel::replace);
}

void FindReplacePanel::setEditor(CodeEditor *e) { editor = e; }
void FindReplacePanel::showFind() { replaceRow->setVisible(false); show(); focusFindInput(); }
void FindReplacePanel::showReplace() { replaceRow->setVisible(true); show(); focusFindInput(); }
void FindReplacePanel::focusFindInput() { findInput->setFocus(); findInput->selectAll(); }

void FindReplacePanel::onTextChanged(const QString &)
{
    if (editor) findNext();
}

void FindReplacePanel::findNext()
{
    if (!editor || findInput->text().isEmpty()) return;
    QString search = findInput->text();
    Qt::CaseSensitivity cs = caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    if (regexMode->isChecked()) {
        QRegularExpression re(search);
        if (wholeWord->isChecked())
            re.setPattern("\\b" + re.pattern() + "\\b");
        if (!caseSensitive->isChecked())
            re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QTextDocument *doc = editor->document();
        QTextCursor cursor = editor->textCursor();
        cursor = doc->find(re, cursor.position(), QTextDocument::FindFlags());
        if (cursor.isNull())
            cursor = doc->find(re, 0, QTextDocument::FindFlags());
        if (!cursor.isNull()) {
            editor->setTextCursor(cursor);
            matchCount->setText("found");
        } else {
            matchCount->setText("no results");
        }
        return;
    }

    QTextDocument::FindFlags flags;
    if (cs == Qt::CaseSensitive) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWord->isChecked()) flags |= QTextDocument::FindWholeWords;

    QTextDocument *doc = editor->document();
    QTextCursor cursor = editor->textCursor();
    cursor = doc->find(search, cursor, flags);
    if (cursor.isNull())
        cursor = doc->find(search, 0, flags);

    if (!cursor.isNull()) {
        editor->setTextCursor(cursor);
        matchCount->setText("found");
    } else {
        matchCount->setText("no results");
    }
}

void FindReplacePanel::findPrev()
{
    if (!editor || findInput->text().isEmpty()) return;
    QString search = findInput->text();
    Qt::CaseSensitivity cs = caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (cs == Qt::CaseSensitive) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWord->isChecked()) flags |= QTextDocument::FindWholeWords;

    QTextCursor cursor = editor->document()->find(search, editor->textCursor(), flags);
    if (cursor.isNull()) {
        cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::End);
        cursor = editor->document()->find(search, cursor, flags);
    }
    if (!cursor.isNull()) {
        editor->setTextCursor(cursor);
        matchCount->setText("found");
    } else {
        matchCount->setText("no results");
    }
}

void FindReplacePanel::replace()
{
    if (!editor) return;
    QTextCursor cursor = editor->textCursor();
    QString sel = cursor.selectedText();
    if (sel == findInput->text() || (!caseSensitive->isChecked() &&
        sel.compare(findInput->text(), Qt::CaseInsensitive) == 0)) {
        cursor.insertText(replaceInput->text());
    }
    findNext();
}

void FindReplacePanel::replaceAll()
{
    if (!editor || findInput->text().isEmpty()) return;
    QString find = findInput->text();
    QString rep = replaceInput->text();
    Qt::CaseSensitivity cs = caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QTextDocument::FindFlags flags;
    if (cs == Qt::CaseSensitive) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWord->isChecked()) flags |= QTextDocument::FindWholeWords;

    QTextCursor cursor(editor->document());
    cursor.beginEditBlock();
    int count = 0;
    while (true) {
        cursor = editor->document()->find(find, cursor, flags);
        if (cursor.isNull()) break;
        cursor.insertText(rep);
        count++;
    }
    cursor.endEditBlock();
    matchCount->setText(QString("replaced %1").arg(count));
}
