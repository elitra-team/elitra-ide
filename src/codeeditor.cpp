#include "codeeditor.h"
#include "colors.h"
#include <QPainter>
#include <QTextBlock>
#include <QTextCursor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QAbstractItemView>
#include <QToolTip>
#include <QHelpEvent>
#include <algorithm>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    gutter = new GutterArea(this);

    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateGutterWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateGutter);
    connect(this, &CodeEditor::cursorPositionChanged,
            this, &CodeEditor::highlightCurrentLine);
    connect(this, &QPlainTextEdit::textChanged, this, [this]() {
        updateCompletions(QString());
        recalcFolds();
    });

    connect(this, &CodeEditor::cursorPositionChanged, this, [this]() {
        QTextCursor c = textCursor();
        emit cursorMoved(c.blockNumber(), c.columnNumber());
    });

    updateGutterWidth(0);
    highlightCurrentLine();

    QFont font("JetBrains Mono", 12);
    font.setStyleHint(QFont::Monospace);
    setFont(font);
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    setLineWrapMode(QPlainTextEdit::NoWrap);

    setMouseTracking(true);

    setStyleSheet(
        "QPlainTextEdit { background-color: #1a1a1e; color: #d4d4d4; border: none; "
        "  selection-background-color: #3d3d42; selection-color: #ffffff; }"
    );

    QTextOption opt = document()->defaultTextOption();
    opt.setFlags(opt.flags() | QTextOption::ShowTabsAndSpaces);
    document()->setDefaultTextOption(opt);

    setupCompleter();
}

void CodeEditor::setupCompleter()
{
    completions = {
        "let", "fn", "if", "else", "while", "for", "in", "return",
        "nil", "break", "continue", "import", "match", "try", "catch",
        "print", "println", "true", "false",
        "int", "float", "string", "bool", "list", "dict"
    };

    completionModel = new QStringListModel(completions, this);
    completer = new QCompleter(this);
    completer->setModel(completionModel);
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->popup()->setStyleSheet(
        "QAbstractItemView { background-color: #2d2d32; color: #d4d4d4; "
        "border: 1px solid #3d3d42; font-family: 'JetBrains Mono'; font-size: 12px; "
        "selection-background-color: #3d3d42; }"
    );
}

void CodeEditor::updateCompletions(const QString &)
{
    if (!completer) return;
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString prefix = cursor.selectedText();

    if (prefix.isEmpty() || !prefix[0].isLetter()) {
        completer->popup()->hide();
        return;
    }

    QStringList filtered;
    for (const QString &w : completions)
        if (w.startsWith(prefix, Qt::CaseInsensitive))
            filtered << w;
    completionModel->setStringList(filtered);

    if (completer->completionCount() > 0 && prefix.length() >= 1) {
        QRect cr = cursorRect();
        cr.setWidth(completer->popup()->sizeHintForColumn(0)
                    + completer->popup()->verticalScrollBar()->sizeHint().width() + 30);
        completer->complete(cr);
    } else {
        completer->popup()->hide();
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (completer && completer->popup()->isVisible()) {
        switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return: {
            QString completion = completer->currentCompletion();
            if (!completion.isEmpty()) {
                QTextCursor tc = textCursor();
                tc.select(QTextCursor::WordUnderCursor);
                tc.insertText(completion);
                completer->popup()->hide();
                return;
            }
            break;
        }
        case Qt::Key_Tab: {
            QString completion = completer->currentCompletion();
            if (!completion.isEmpty()) {
                QTextCursor tc = textCursor();
                tc.select(QTextCursor::WordUnderCursor);
                tc.insertText(completion);
                completer->popup()->hide();
                return;
            }
            break;
        }
        case Qt::Key_Escape:
            completer->popup()->hide();
            return;
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        case Qt::Key_Home:
        case Qt::Key_End:
            event->ignore();
            return;
        default:
            break;
        }
    }

    if (event->modifiers() == Qt::ControlModifier) {
        if (event->key() == static_cast<int>(Qt::Key_Plus) ||
            event->key() == static_cast<int>(Qt::Key_Equal)) {
            zoomIn();
            return;
        }
        if (event->key() == static_cast<int>(Qt::Key_Minus)) {
            zoomOut();
            return;
        }
        if (event->key() == static_cast<int>(Qt::Key_0)) {
            resetZoom();
            return;
        }
        if (event->key() == static_cast<int>(Qt::Key_F)) {
            emit findShortcut();
            return;
        }
        if (event->key() == static_cast<int>(Qt::Key_H)) {
            emit replaceShortcut();
            return;
        }
        if (event->key() == static_cast<int>(Qt::Key_G)) {
            emit goToLineShortcut();
            return;
        }
    }

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        QString indent = indentOfLine(cursor.blockNumber());
        QString trimmed = cursor.block().text().trimmed();
        if (trimmed.endsWith('{') || trimmed.endsWith('(') || trimmed.endsWith('['))
            indent += "    ";

        QPlainTextEdit::keyPressEvent(event);
        insertPlainText(indent);
        return;
    }

    if (event->key() == Qt::Key_Tab) {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection()) {
            int start = cursor.blockNumber();
            int end = document()->findBlock(cursor.selectionEnd()).blockNumber();
            if (start > end) std::swap(start, end);
            cursor.beginEditBlock();
            for (int i = start; i <= end; ++i) {
                QTextBlock block = document()->findBlockByNumber(i);
                QTextCursor bc(block);
                bc.movePosition(QTextCursor::StartOfBlock);
                bc.insertText("    ");
            }
            cursor.endEditBlock();
            return;
        }
    }

    if (event->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection()) {
            int start = cursor.blockNumber();
            int end = document()->findBlock(cursor.selectionEnd()).blockNumber();
            if (start > end) std::swap(start, end);
            cursor.beginEditBlock();
            for (int i = start; i <= end; ++i) {
                QTextBlock block = document()->findBlockByNumber(i);
                QTextCursor bc(block);
                bc.movePosition(QTextCursor::StartOfBlock);
                bc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);
                if (bc.selectedText().trimmed().isEmpty())
                    bc.removeSelectedText();
            }
            cursor.endEditBlock();
            return;
        }
    }

    QPlainTextEdit::keyPressEvent(event);

    if (event->text().length() == 1 && event->text()[0].isLetterOrNumber())
        updateCompletions(QString());
}

QString CodeEditor::indentOfLine(int line) const
{
    QTextBlock block = document()->findBlockByNumber(line);
    if (!block.isValid()) return {};
    QString text = block.text();
    QString indent;
    for (QChar c : text) {
        if (c.isSpace()) indent += c;
        else break;
    }
    return indent;
}

bool CodeEditor::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        auto *helpEvent = static_cast<QHelpEvent *>(event);
        QTextCursor cursor = cursorForPosition(helpEvent->pos());
        int line = cursor.blockNumber();
        for (const auto &d : diagnostics) {
            if (d.line == line) {
                QToolTip::showText(helpEvent->globalPos(),
                    QString("<span style='color:%1;'>%2</span>")
                        .arg(d.color.name(), d.message.toHtmlEscaped()));
                return true;
            }
        }
        QToolTip::hideText();
        return true;
    }
    return QPlainTextEdit::event(event);
}

void CodeEditor::setDiagnostics(const QVector<DiagMarker> &diags)
{
    diagnostics = diags;
    gutter->update();
}

void CodeEditor::zoomIn()
{
    int newSize = qMin(defaultFontSize_ + 2, 40);
    setDefaultFontSize(newSize);
}

void CodeEditor::zoomOut()
{
    int newSize = qMax(defaultFontSize_ - 2, 8);
    setDefaultFontSize(newSize);
}

void CodeEditor::resetZoom()
{
    setDefaultFontSize(baseFontSize_);
}

void CodeEditor::setDefaultFontSize(int size)
{
    defaultFontSize_ = size;
    QFont f = font();
    f.setPointSize(size);
    setFont(f);
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    updateGutterWidth(0);
}

int CodeEditor::gutterWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) { max /= 10; digits++; }
    return 14 + 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 8;
}

void CodeEditor::updateGutterWidth(int)
{
    setViewportMargins(gutterWidth(), 0, 0, 0);
}

void CodeEditor::updateGutter(const QRect &rect, int dy)
{
    if (dy)
        gutter->scroll(0, dy);
    else
        gutter->update(0, rect.y(), gutter->width(), rect.height());
    if (rect.contains(viewport()->rect()))
        updateGutterWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    gutter->setGeometry(QRect(cr.left(), cr.top(), gutterWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(Colors::bgCurrentLine));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    BracketMatch match = findMatchingBracket(textCursor());
    if (match.valid) {
        QTextEdit::ExtraSelection openSel;
        openSel.format.setBackground(QColor(Colors::bgBracket));
        openSel.format.setForeground(QColor(Colors::synBracketFg));
        openSel.cursor = match.openPos;

        QTextEdit::ExtraSelection closeSel;
        closeSel.format.setBackground(QColor(Colors::bgBracket));
        closeSel.format.setForeground(QColor(Colors::synBracketFg));
        closeSel.cursor = match.closePos;

        extraSelections.append(openSel);
        extraSelections.append(closeSel);
    }

    setExtraSelections(extraSelections);
}

CodeEditor::BracketMatch CodeEditor::findMatchingBracket(QTextCursor cursor)
{
    BracketMatch result;
    QChar forwardChar, backwardChar;
    QTextCursor::MoveOperation direction;

    QChar c = cursor.document()->characterAt(cursor.position());
    if (c == '(' || c == '{' || c == '[') {
        forwardChar = c;
        switch (c.toLatin1()) {
        case '(': backwardChar = ')'; break;
        case '{': backwardChar = '}'; break;
        case '[': backwardChar = ']'; break;
        }
        direction = QTextCursor::Right;
    } else if (c == ')' || c == '}' || c == ']') {
        backwardChar = c;
        switch (c.toLatin1()) {
        case ')': forwardChar = '('; break;
        case '}': forwardChar = '{'; break;
        case ']': forwardChar = '['; break;
        }
        direction = QTextCursor::Left;
    } else {
        return result;
    }

    QTextCursor search = cursor;
    int depth = 0;

    while (true) {
        if (direction == QTextCursor::Right) {
            if (search.atEnd()) break;
            search.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
        } else {
            if (search.atStart()) break;
            search.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
        }

        QChar ch = search.selectedText().at(0);
        if (ch == forwardChar) depth++;
        else if (ch == backwardChar) depth--;

        if (depth == 0) {
            result.valid = true;
            if (direction == QTextCursor::Right) {
                result.openPos = cursor;
                result.closePos = search;
            } else {
                result.openPos = search;
                result.closePos = cursor;
            }
            return result;
        }
    }

    return result;
}

int CodeEditor::lineAtY(int y) const
{
    QTextBlock block = QPlainTextEdit::firstVisibleBlock();
    int top = qRound(QPlainTextEdit::blockBoundingGeometry(block)
                     .translated(QPlainTextEdit::contentOffset()).top());

    while (block.isValid()) {
        int bottom = top + qRound(QPlainTextEdit::blockBoundingRect(block).height());
        if (y >= top && y < bottom)
            return block.blockNumber();
        block = block.next();
        top = bottom;
    }
    return -1;
}

void CodeEditor::gutterPaintEvent(QPaintEvent *event)
{
    QPainter painter(gutter);
    painter.fillRect(event->rect(), QColor(Colors::gutterBg));

    int foldIconWidth = 14;
    QFont iconFont = painter.font();
    iconFont.setPointSize(defaultFontSize_ - 2);
    painter.setFont(iconFont);

    QTextBlock block = QPlainTextEdit::firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(QPlainTextEdit::blockBoundingGeometry(block)
                     .translated(QPlainTextEdit::contentOffset()).top());
    int bottom = top + qRound(QPlainTextEdit::blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            int line = blockNumber;

            for (const auto &d : diagnostics) {
                if (d.line == line) {
                    painter.setBrush(QColor(d.color));
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(4, top + 5, 6, 6);
                    break;
                }
            }

            int foldEnd = findFoldEnd(line);
            if (foldEnd > line) {
                painter.setPen(QColor(Colors::foldIcon));
                if (isFolded(line)) {
                    painter.drawText(1, top, foldIconWidth - 2, fontMetrics().height(),
                                     Qt::AlignCenter, "+");
                } else {
                    painter.drawText(1, top, foldIconWidth - 2, fontMetrics().height(),
                                     Qt::AlignCenter, "-");
                }
            }

            QString number = QString::number(line + 1);
            painter.setPen(QColor(Colors::gutterText));
            painter.drawText(foldIconWidth, top,
                             gutter->width() - foldIconWidth - 6,
                             fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(QPlainTextEdit::blockBoundingRect(block).height());
        blockNumber++;
    }
}

void GutterArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int line = editor->lineAtY(event->pos().y());
        if (line >= 0 && event->pos().x() < 14) {
            if (editor->foldEndLine(line) > line) {
                editor->toggleFold(line);
                return;
            }
        }
    }
    QWidget::mousePressEvent(event);
}

/* ── Code Folding ── */

void CodeEditor::recalcFolds()
{
    folds.clear();
    QVector<int> braceStack;

    for (QTextBlock block = document()->firstBlock(); block.isValid(); block = block.next()) {
        if (!block.isVisible()) continue;
        QString text = block.text().trimmed();
        if (text.isEmpty()) continue;

        int openBraces = text.count('{') - text.count('}');
        if (openBraces > 0) {
            for (int i = 0; i < openBraces; ++i)
                braceStack.append(block.blockNumber());
        } else if (openBraces < 0) {
            int closeCount = -openBraces;
            for (int i = 0; i < closeCount && !braceStack.isEmpty(); ++i) {
                int openLine = braceStack.takeLast();
                FoldBlock fb;
                fb.fromLine = openLine;
                fb.toLine = block.blockNumber();
                fb.collapsed = false;
                folds.append(fb);
            }
        }
    }
}

int CodeEditor::findFoldStart(int line) const
{
    for (const auto &fb : folds)
        if (fb.fromLine == line) return line;
    return -1;
}

int CodeEditor::findFoldEnd(int line) const
{
    for (const auto &fb : folds)
        if (fb.fromLine == line) return fb.toLine;
    return -1;
}

int CodeEditor::foldEndLine(int line) const
{
    return findFoldEnd(line);
}

bool CodeEditor::isFolded(int line) const
{
    for (const auto &fb : folds)
        if (fb.fromLine == line && fb.collapsed) return true;
    return false;
}

void CodeEditor::toggleFold(int line)
{
    for (auto &fb : folds) {
        if (fb.fromLine == line) {
            fb.collapsed = !fb.collapsed;
            for (int i = fb.fromLine + 1; i < fb.toLine; ++i) {
                QTextBlock block = document()->findBlockByNumber(i);
                if (block.isValid())
                    block.setVisible(!fb.collapsed);
            }
            document()->markContentsDirty(0, document()->blockCount());
            updateGutterWidth(0);
            return;
        }
    }
}
