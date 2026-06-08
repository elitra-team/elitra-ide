#ifndef ELITRAHIGHLIGHTER_H
#define ELITRAHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class ElitraHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit ElitraHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> rules;

    QRegularExpression commentStartExpr;
    QRegularExpression commentEndExpr;
    QTextCharFormat multiLineCommentFormat;

    QRegularExpression stringStartExpr;
    QRegularExpression stringEndExpr;
    QTextCharFormat stringFormat;

    QTextCharFormat keywordFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat builtinFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat interpolationFormat;
};

#endif
