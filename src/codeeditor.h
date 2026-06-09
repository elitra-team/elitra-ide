#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QTextBlock>
#include <QVector>
#include <QMap>
#include <QCompleter>
#include <QStringListModel>

class GutterArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void gutterPaintEvent(QPaintEvent *event);
    int gutterWidth() const;

    void zoomIn();
    void zoomOut();
    void resetZoom();

    int defaultFontSize() const { return defaultFontSize_; }
    void setDefaultFontSize(int size);

    struct FoldBlock {
        int fromLine;
        int toLine;
        bool collapsed;
    };

    struct BracketMatch {
        QTextCursor openPos;
        QTextCursor closePos;
        bool valid = false;
    };

    struct DiagMarker {
        int line;
        QString message;
        QColor color;
    };

    BracketMatch findMatchingBracket(QTextCursor cursor);

    void toggleFold(int line);
    bool isFolded(int line) const;
    int foldEndLine(int line) const;

    QString text() const { return toPlainText(); }
    void setText(const QString &t) { setPlainText(t); }

    void setDiagnostics(const QVector<DiagMarker> &diags);
    QVector<DiagMarker> diagnosticsList() const { return diagnostics; }

    int lineAtY(int y) const;

signals:
    void findShortcut();
    void replaceShortcut();
    void goToLineShortcut();
    void cursorMoved(int line, int column);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

private slots:
    void updateGutterWidth(int newBlockCount);
    void updateGutter(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    QWidget *gutter;
    int defaultFontSize_ = 12;
    int baseFontSize_ = 12;

    void setupCompleter();
    void updateCompletions(const QString &prefix);
    QCompleter *completer;
    QStringListModel *completionModel;
    QStringList completions;

    QVector<FoldBlock> folds;
    void recalcFolds();
    int findFoldStart(int line) const;
    int findFoldEnd(int line) const;

    QString indentOfLine(int line) const;

    QVector<DiagMarker> diagnostics;
};

class GutterArea : public QWidget {
public:
    explicit GutterArea(CodeEditor *editor) : QWidget(editor), editor(editor) {}
    QSize sizeHint() const override {
        return QSize(editor->gutterWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        editor->gutterPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event) override;

private:
    CodeEditor *editor;
};

#endif
