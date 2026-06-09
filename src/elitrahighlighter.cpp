#include "elitrahighlighter.h"
#include "colors.h"

ElitraHighlighter::ElitraHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    keywordFormat.setForeground(QColor(Colors::synKeyword));
    keywordFormat.setFontWeight(QFont::Bold);

    typeFormat.setForeground(QColor(Colors::synType));
    typeFormat.setFontWeight(QFont::Bold);

    numberFormat.setForeground(QColor(Colors::synNumber));

    builtinFormat.setForeground(QColor(Colors::synBuiltin));

    singleLineCommentFormat.setForeground(QColor(Colors::synComment));
    singleLineCommentFormat.setFontItalic(true);

    interpolationFormat.setForeground(QColor(Colors::synString));

    HighlightRule rule;

    QStringList keywords = {
        "\\blet\\b", "\\bfn\\b", "\\bif\\b", "\\belse\\b",
        "\\bwhile\\b", "\\bfor\\b", "\\bin\\b", "\\breturn\\b",
        "\\bnil\\b", "\\bbreak\\b", "\\bcontinue\\b", "\\bimport\\b",
        "\\bmatch\\b", "\\btry\\b", "\\bcatch\\b",
        "\\bprint\\b", "\\bprintln\\b"
    };
    for (const QString &kw : keywords) {
        rule.pattern = QRegularExpression(kw);
        rule.format = keywordFormat;
        rules.append(rule);
    }

    auto boolFormat = keywordFormat;
    boolFormat.setForeground(QColor(Colors::synNumber));
    rule.pattern = QRegularExpression("\\btrue\\b|\\bfalse\\b");
    rule.format = boolFormat;
    rules.append(rule);

    QStringList types = {
        "\\bint\\b", "\\bfloat\\b", "\\bstring\\b", "\\bbool\\b",
        "\\blist\\b", "\\bdict\\b"
    };
    for (const QString &t : types) {
        rule.pattern = QRegularExpression(t);
        rule.format = typeFormat;
        rules.append(rule);
    }

    rule.pattern = QRegularExpression("\\b-?\\d+\\.?\\d*\\b");
    rule.format = numberFormat;
    rules.append(rule);

    auto fnCallFormat = QTextCharFormat();
    fnCallFormat.setForeground(QColor(Colors::synFunction));
    rule.pattern = QRegularExpression(
        "\\b(?!let\\b|fn\\b|if\\b|else\\b|while\\b|for\\b|in\\b|return\\b"
        "|nil\\b|break\\b|continue\\b|import\\b|match\\b|try\\b|catch\\b"
        "|print\\b|println\\b"
        "|true\\b|false\\b|int\\b|float\\b|string\\b|bool\\b|list\\b|dict\\b)"
        "[a-zA-Z_][a-zA-Z0-9_]*\\s*(?=\\()"
    );
    rule.format = fnCallFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    rules.append(rule);

    multiLineCommentFormat.setForeground(QColor(Colors::synComment));
    multiLineCommentFormat.setFontItalic(true);
    commentStartExpr = QRegularExpression("/\\*");
    commentEndExpr = QRegularExpression("\\*/");

    stringFormat.setForeground(QColor(Colors::synString));
    stringStartExpr = QRegularExpression("\"");
    stringEndExpr = QRegularExpression("\"");
}

void ElitraHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightRule &rule : rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        QRegularExpressionMatch startMatch = commentStartExpr.match(text);
        startIndex = startMatch.hasMatch() ? startMatch.capturedStart() : -1;
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = commentEndExpr.match(text, startIndex + 2);
        int endIndex = endMatch.hasMatch() ? endMatch.capturedStart() : -1;
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);

        QRegularExpressionMatch nextStart = commentStartExpr.match(text, startIndex + commentLength);
        startIndex = nextStart.hasMatch() ? nextStart.capturedStart() : -1;
    }

    int i = 0;
    bool inString = false;
    while (i < text.length()) {
        if (text[i] == '\\' && inString) {
            i += 2;
            continue;
        }
        if (text[i] == '"') {
            if (!inString) {
                inString = true;
                int start = i;
                i++;
                while (i < text.length()) {
                    if (text[i] == '\\') {
                        i += 2;
                        continue;
                    }
                    if (text[i] == '"') {
                        auto fmt = stringFormat;
                        if (text.mid(start + 1, i - start - 1).contains('{'))
                            fmt.setForeground(QColor(Colors::synAttribute));
                        setFormat(start, i - start + 1, fmt);
                        inString = false;
                        i++;
                        break;
                    }
                    i++;
                }
                if (inString) {
                    setFormat(start, text.length() - start, stringFormat);
                    break;
                }
            }
        } else {
            i++;
        }
    }
}
