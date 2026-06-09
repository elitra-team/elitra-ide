#ifndef LANGUAGEHIGHLIGHTER_H
#define LANGUAGEHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QVector>

class QTextDocument;

struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

// ── Base with shared helpers ──
class LanguageHighlighterBase : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit LanguageHighlighterBase(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;
    virtual void extraHighlight(const QString &text);

    QVector<HighlightRule> rules;
    QTextCharFormat keywordFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat propertyFormat;
    QTextCharFormat tagFormat;
    QTextCharFormat attributeFormat;
    QTextCharFormat punctuationFormat;
    QTextCharFormat builtinFormat;

    void addKeywords(const QStringList &words, const QTextCharFormat &fmt);
    void addTypes(const QStringList &words, const QTextCharFormat &fmt);
    void addRules();
};

// ── CSS ──
class CssHighlighter : public LanguageHighlighterBase {
    Q_OBJECT
public:
    explicit CssHighlighter(QTextDocument *parent = nullptr);
protected:
    void extraHighlight(const QString &text) override;
private:
    QTextCharFormat selectorFormat;
    QTextCharFormat valueFormat;
    QTextCharFormat unitFormat;
    QTextCharFormat hexColorFormat;
};

// ── HTML ──
class HtmlHighlighter : public LanguageHighlighterBase {
    Q_OBJECT
public:
    explicit HtmlHighlighter(QTextDocument *parent = nullptr);
protected:
    void extraHighlight(const QString &text) override;
private:
    QTextCharFormat entityFormat;
    QTextCharFormat doctypeFormat;
};

// ── JavaScript ──
class JsHighlighter : public LanguageHighlighterBase {
    Q_OBJECT
public:
    explicit JsHighlighter(QTextDocument *parent = nullptr);
};

// ── Rust ──
class RustHighlighter : public LanguageHighlighterBase {
    Q_OBJECT
public:
    explicit RustHighlighter(QTextDocument *parent = nullptr);
protected:
    void extraHighlight(const QString &text) override;
private:
    QTextCharFormat lifetimeFormat;
    QTextCharFormat macroFormat;
    QTextCharFormat attributeFormat;
};

// ── Python ──
class PythonHighlighter : public LanguageHighlighterBase {
    Q_OBJECT
public:
    explicit PythonHighlighter(QTextDocument *parent = nullptr);
protected:
    void extraHighlight(const QString &text) override;
private:
    QTextCharFormat decoratorFormat;
    QTextCharFormat selfFormat;
    QRegularExpression multilineStringStart;
    QRegularExpression multilineStringEnd;
    QTextCharFormat multilineStringFormat;
};

// ── Lua ──
class LuaHighlighter : public LanguageHighlighterBase {
    Q_OBJECT
public:
    explicit LuaHighlighter(QTextDocument *parent = nullptr);
};

// ── C / C++ ──
class CppHighlighter : public LanguageHighlighterBase {
    Q_OBJECT
public:
    explicit CppHighlighter(QTextDocument *parent = nullptr);
protected:
    void extraHighlight(const QString &text) override;
private:
    QTextCharFormat includeFormat;
};

// ── Factory ──
LanguageHighlighterBase *createHighlighterForFile(
    const QString &filePath, QTextDocument *doc);

#endif
