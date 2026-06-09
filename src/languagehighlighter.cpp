#include "languagehighlighter.h"
#include "colors.h"
#include <QFileInfo>

/* ══════════════════════════════════════════════
   LanguageHighlighterBase
   ══════════════════════════════════════════════ */

LanguageHighlighterBase::LanguageHighlighterBase(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    keywordFormat.setForeground(QColor(Colors::synKeyword));
    keywordFormat.setFontWeight(QFont::Bold);

    typeFormat.setForeground(QColor(Colors::synType));
    typeFormat.setFontWeight(QFont::Bold);

    numberFormat.setForeground(QColor(Colors::synNumber));

    builtinFormat.setForeground(QColor(Colors::synBuiltin));

    stringFormat.setForeground(QColor(Colors::synString));

    commentFormat.setForeground(QColor(Colors::synComment));
    commentFormat.setFontItalic(true);

    functionFormat.setForeground(QColor(Colors::synFunction));

    preprocessorFormat.setForeground(QColor(Colors::synPreproc));

    propertyFormat.setForeground(QColor(Colors::synProperty));

    tagFormat.setForeground(QColor(Colors::synTag));

    attributeFormat.setForeground(QColor(Colors::synAttribute));

    punctuationFormat.setForeground(QColor(Colors::synPunct));

    addRules();
}

void LanguageHighlighterBase::addKeywords(const QStringList &words, const QTextCharFormat &fmt)
{
    HighlightRule rule;
    rule.format = fmt;
    for (const QString &w : words)
        rule.pattern = QRegularExpression("\\b" + w + "\\b");
}

void LanguageHighlighterBase::addTypes(const QStringList &words, const QTextCharFormat &fmt)
{
    HighlightRule rule;
    rule.format = fmt;
    for (const QString &w : words)
        rule.pattern = QRegularExpression("\\b" + w + "\\b");
}

void LanguageHighlighterBase::addRules()
{
    // Default highlights — subclasses override extraHighlight()
    HighlightRule r;

    r.pattern = QRegularExpression("\\b-?\\d+\\.?\\d*[fFlLuU]?\\b");
    r.format = numberFormat;
    rules.append(r);
}

void LanguageHighlighterBase::highlightBlock(const QString &text)
{
    for (const HighlightRule &rule : rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    extraHighlight(text);
}

void LanguageHighlighterBase::extraHighlight(const QString &)
{
}

/* ══════════════════════════════════════════════
   CSS
   ══════════════════════════════════════════════ */

CssHighlighter::CssHighlighter(QTextDocument *parent)
    : LanguageHighlighterBase(parent)
{
    selectorFormat.setForeground(QColor(Colors::synTag));
    valueFormat.setForeground(QColor(Colors::synString));
    unitFormat.setForeground(QColor(Colors::synNumber));
    hexColorFormat.setForeground(QColor(Colors::synPreproc));

    QStringList cssKeywords = {
        "import", "media", "keyframes", "font-face", "supports",
        "include", "mixin", "extend"
    };

    rules.clear();
    keywordFormat.setForeground(QColor(Colors::synKeyword));
    for (const QString &kw : cssKeywords) {
        HighlightRule r;
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFormat;
        rules.append(r);
    }

    HighlightRule r;

    // Properties
    r.pattern = QRegularExpression("[a-z-]+(?=\\s*:)");
    r.format = propertyFormat;
    rules.append(r);

    // Values (after colon, before semicolon)
    // handled in extraHighlight

    // Units
    r.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?(px|em|rem|vh|vw|vmin|vmax|%|s|ms|deg|rad|cm|mm|in|pt|pc|ex|ch)");
    r.format = unitFormat;
    rules.append(r);

    // Hex colors
    r.pattern = QRegularExpression("#[0-9a-fA-F]{3,8}\\b");
    r.format = hexColorFormat;
    rules.append(r);

    // Numbers
    r.pattern = QRegularExpression("\\b-?\\d+\\.?\\d*\\b");
    r.format = numberFormat;
    rules.append(r);

    // Comments
    r.pattern = QRegularExpression("/\\*.*?\\*/");
    r.format = commentFormat;
    rules.append(r);

    // Strings
    r.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);

    r.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)*'");
    r.format = stringFormat;
    rules.append(r);
}

void CssHighlighter::extraHighlight(const QString &text)
{
    // Match CSS selectors: anything before '{' that looks like a selector
    // This is a simplified approach
    QRegularExpression selRe("([.#]?[a-zA-Z][a-zA-Z0-9_-]*(?::[a-zA-Z-]+)?)(?=\\s*[\\{,])");
    QRegularExpressionMatchIterator it = selRe.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        setFormat(m.capturedStart(), m.capturedLength(), selectorFormat);
    }
}

/* ══════════════════════════════════════════════
   HTML
   ══════════════════════════════════════════════ */

HtmlHighlighter::HtmlHighlighter(QTextDocument *parent)
    : LanguageHighlighterBase(parent)
{
    entityFormat.setForeground(QColor(Colors::synAttribute));
    doctypeFormat.setForeground(QColor(Colors::synComment));

    HighlightRule r;

    // DOCTYPE
    r.pattern = QRegularExpression("<!DOCTYPE[^>]+>", QRegularExpression::CaseInsensitiveOption);
    r.format = doctypeFormat;
    rules.append(r);

    // Tags
    r.pattern = QRegularExpression("</?[a-zA-Z][a-zA-Z0-9]*\\b");
    r.format = tagFormat;
    rules.append(r);

    // Closing tag >
    r.pattern = QRegularExpression(">");
    r.format = tagFormat;
    rules.append(r);

    // Attributes
    r.pattern = QRegularExpression("\\b[a-zA-Z][a-zA-Z0-9-]*(?=\\s*=)");
    r.format = attributeFormat;
    rules.append(r);

    // Attribute values (double quoted)
    r.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);

    // Attribute values (single quoted)
    r.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)*'");
    r.format = stringFormat;
    rules.append(r);

    // HTML entities
    r.pattern = QRegularExpression("&[a-zA-Z]+;|&#\\d+;|&#x[0-9a-fA-F]+;");
    r.format = entityFormat;
    rules.append(r);

    // Comments
    r.pattern = QRegularExpression("<!--.*?-->");
    r.format = commentFormat;
    rules.append(r);
}

void HtmlHighlighter::extraHighlight(const QString &)
{
}

/* ══════════════════════════════════════════════
   JavaScript
   ══════════════════════════════════════════════ */

JsHighlighter::JsHighlighter(QTextDocument *parent)
    : LanguageHighlighterBase(parent)
{
    keywordFormat.setFontWeight(QFont::Bold);

    typeFormat.setForeground(QColor(Colors::synType));
    builtinFormat.setForeground(QColor(Colors::synBuiltin));

    HighlightRule r;

    QStringList jsKeywords = {
        "let", "const", "var", "function", "if", "else", "for", "while",
        "do", "switch", "case", "break", "continue", "return", "throw",
        "try", "catch", "finally", "new", "delete", "typeof", "instanceof",
        "in", "of", "class", "extends", "super", "import", "export",
        "default", "from", "as", "yield", "async", "await", "this",
        "null", "undefined", "true", "false", "void"
    };
    for (const QString &kw : jsKeywords) {
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFormat;
        rules.append(r);
    }

    QStringList jsTypes = {"number", "string", "boolean", "object", "Array", "Map", "Set",
                          "Promise", "Symbol", "BigInt", "Error", "Date", "RegExp"};
    for (const QString &t : jsTypes) {
        r.pattern = QRegularExpression("\\b" + t + "\\b");
        r.format = typeFormat;
        rules.append(r);
    }

    QStringList jsBuiltins = {
        "console", "Math", "JSON", "parseInt", "parseFloat", "isNaN",
        "setTimeout", "setInterval", "clearTimeout", "clearInterval",
        "require", "module", "process", "Buffer", "__dirname", "__filename",
        "document", "window", "fetch", "localStorage", "sessionStorage"
    };
    for (const QString &b : jsBuiltins) {
        r.pattern = QRegularExpression("\\b" + b + "\\b");
        r.format = builtinFormat;
        rules.append(r);
    }

    // Strings (double and single)
    r.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);
    r.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)*'");
    r.format = stringFormat;
    rules.append(r);
    // Template literals
    r.pattern = QRegularExpression("`(?:[^`\\\\]|\\\\.)*`");
    r.format = stringFormat;
    rules.append(r);

    // Comments
    r.pattern = QRegularExpression("//[^\n]*");
    r.format = commentFormat;
    rules.append(r);
    r.pattern = QRegularExpression("/\\*.*?\\*/");
    r.format = commentFormat;
    rules.append(r);

    // Numbers
    r.pattern = QRegularExpression("\\b(?:0[xX][0-9a-fA-F]+|0[bB][01]+|0[oO][0-7]+|\\d+\\.?\\d*(?:[eE][+-]?\\d+)?)\\b");
    r.format = numberFormat;
    rules.append(r);

    // Function calls
    r.pattern = QRegularExpression("\\b[a-zA-Z_$][a-zA-Z0-9_$]*\\s*(?=\\()");
    r.format = functionFormat;
    rules.append(r);
}

/* ══════════════════════════════════════════════
   Rust
   ══════════════════════════════════════════════ */

RustHighlighter::RustHighlighter(QTextDocument *parent)
    : LanguageHighlighterBase(parent)
{
    lifetimeFormat.setForeground(QColor(Colors::synString));
    macroFormat.setForeground(QColor(Colors::synBuiltin));
    attributeFormat.setForeground(QColor(Colors::synProperty));

    HighlightRule r;

    QStringList rustKeywords = {
        "fn", "let", "mut", "const", "static", "if", "else", "for", "while",
        "loop", "match", "break", "continue", "return", "in", "where",
        "struct", "enum", "trait", "impl", "type", "self", "Self",
        "pub", "use", "mod", "crate", "super", "as", "ref", "move",
        "async", "await", "unsafe", "dyn", "abstract", "become", "box",
        "do", "final", "macro", "override", "priv", "typeof", "unsized",
        "virtual", "yield", "true", "false", "Some", "None", "Ok", "Err"
    };
    for (const QString &kw : rustKeywords) {
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFormat;
        rules.append(r);
    }

    QStringList rustTypes = {
        "i8", "i16", "i32", "i64", "i128", "isize",
        "u8", "u16", "u32", "u64", "u128", "usize",
        "f32", "f64", "bool", "char", "String", "str",
        "Vec", "HashMap", "HashSet", "Option", "Result",
        "Box", "Rc", "Arc", "Mutex", "RefCell",
        "Iterator", "IntoIterator", "From", "Into",
        "ToString", "Clone", "Copy", "Debug", "Display",
        "PartialEq", "Eq", "PartialOrd", "Ord",
        "Default", "Deref", "Drop"
    };
    for (const QString &t : rustTypes) {
        r.pattern = QRegularExpression("\\b" + t + "\\b");
        r.format = typeFormat;
        rules.append(r);
    }

    // Strings
    r.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);

    // Char literals
    r.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)'");
    r.format = stringFormat;
    rules.append(r);

    // Raw strings
    r.pattern = QRegularExpression("r#\"(?:[^\"]|\"(?!#))*\"#");
    r.format = stringFormat;
    rules.append(r);

    // Numbers
    r.pattern = QRegularExpression("\\b(?:0[xX][0-9a-fA-F][0-9a-fA-F_]*"
                                   "|0[bB][01][01_]*"
                                   "|0[oO][0-7][0-7_]*"
                                   "|\\d[\\d_]*\\.?\\d*(?:[eE][+-]?\\d+)?)"
                                   "(?:f32|f64|u8|u16|u32|u64|i8|i16|i32|i64|usize|isize)?\\b");
    r.format = numberFormat;
    rules.append(r);

    // Lifetimes
    r.pattern = QRegularExpression("'[a-zA-Z_][a-zA-Z0-9_]*");
    r.format = lifetimeFormat;
    rules.append(r);

    // Macros
    r.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*!");
    r.format = macroFormat;
    rules.append(r);

    // Attributes
    r.pattern = QRegularExpression("#\\!?\\[.*?\\]");
    r.format = attributeFormat;
    rules.append(r);

    // Comments
    r.pattern = QRegularExpression("//[^\n]*");
    r.format = commentFormat;
    rules.append(r);
    r.pattern = QRegularExpression("/\\*.*?\\*/");
    r.format = commentFormat;
    rules.append(r);

    // Function calls
    r.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*\\s*(?=\\()");
    r.format = functionFormat;
    rules.append(r);
}

void RustHighlighter::extraHighlight(const QString &)
{
}

/* ══════════════════════════════════════════════
   Python
   ══════════════════════════════════════════════ */

PythonHighlighter::PythonHighlighter(QTextDocument *parent)
    : LanguageHighlighterBase(parent)
{
    decoratorFormat.setForeground(QColor(Colors::synProperty));
    selfFormat.setForeground(QColor(Colors::synPreproc));

    multilineStringFormat.setForeground(QColor(Colors::synString));
    multilineStringFormat.setFontItalic(true);
    multilineStringStart = QRegularExpression("\"\"\"|'''");
    multilineStringEnd = QRegularExpression("\"\"\"|'''");

    HighlightRule r;

    QStringList pyKeywords = {
        "def", "class", "if", "elif", "else", "for", "while", "break",
        "continue", "return", "yield", "import", "from", "as",
        "try", "except", "finally", "raise", "with", "pass",
        "in", "not", "and", "or", "is", "lambda", "global",
        "nonlocal", "assert", "del", "async", "await",
        "True", "False", "None", "self"
    };
    for (const QString &kw : pyKeywords) {
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFormat;
        rules.append(r);
    }

    QStringList pyTypes = {
        "int", "float", "bool", "str", "list", "dict", "tuple", "set",
        "frozenset", "bytes", "bytearray", "range", "complex",
        "type", "object", "Exception", "BaseException"
    };
    for (const QString &t : pyTypes) {
        r.pattern = QRegularExpression("\\b" + t + "\\b");
        r.format = typeFormat;
        rules.append(r);
    }

    QStringList pyBuiltins = {
        "print", "len", "range", "int", "str", "float", "list", "dict",
        "tuple", "set", "open", "input", "type", "super", "isinstance",
        "issubclass", "hasattr", "getattr", "setattr", "delattr",
        "enumerate", "zip", "map", "filter", "sorted", "reversed",
        "min", "max", "sum", "abs", "round", "any", "all",
        "__init__", "__str__", "__repr__", "__len__", "__call__"
    };
    for (const QString &b : pyBuiltins) {
        r.pattern = QRegularExpression("\\b" + b + "\\b");
        r.format = builtinFormat;
        rules.append(r);
    }

    // Strings (double and single)
    r.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);
    r.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)*'");
    r.format = stringFormat;
    rules.append(r);
    // f-strings and byte strings (same coloring for now)
    r.pattern = QRegularExpression("(?:f|b|r|fb|rb)\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);
    r.pattern = QRegularExpression("(?:f|b|r|fb|rb)'(?:[^'\\\\]|\\\\.)*'");
    r.format = stringFormat;
    rules.append(r);

    // Numbers
    r.pattern = QRegularExpression("\\b(?:\\d+\\.?\\d*(?:[eE][+-]?\\d+)?"
                                   "|0[xX][0-9a-fA-F]+"
                                   "|0[bB][01]+"
                                   "|0[oO][0-7]+)\\b");
    r.format = numberFormat;
    rules.append(r);

    // Decorators
    r.pattern = QRegularExpression("@[a-zA-Z_][a-zA-Z0-9_.]*");
    r.format = decoratorFormat;
    rules.append(r);

    // Comments
    r.pattern = QRegularExpression("#[^\n]*");
    r.format = commentFormat;
    rules.append(r);

    // Function calls
    r.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*\\s*(?=\\()");
    r.format = functionFormat;
    rules.append(r);
}

void PythonHighlighter::extraHighlight(const QString &text)
{
    setCurrentBlockState(0);

    // Multiline strings (triple-quoted)
    int startIndex = 0;
    if (previousBlockState() != 1) {
        QRegularExpressionMatch startMatch = multilineStringStart.match(text);
        startIndex = startMatch.hasMatch() ? startMatch.capturedStart() : -1;
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = multilineStringEnd.match(text, startIndex + 3);
        int endIndex = endMatch.hasMatch() ? endMatch.capturedStart() : -1;
        int length;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            length = text.length() - startIndex;
        } else {
            length = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, length, multilineStringFormat);

        QRegularExpressionMatch nextStart = multilineStringStart.match(text, startIndex + length);
        startIndex = nextStart.hasMatch() ? nextStart.capturedStart() : -1;
    }
}

/* ══════════════════════════════════════════════
   Lua
   ══════════════════════════════════════════════ */

LuaHighlighter::LuaHighlighter(QTextDocument *parent)
    : LanguageHighlighterBase(parent)
{
    HighlightRule r;

    QStringList luaKeywords = {
        "and", "break", "do", "else", "elseif", "end", "false",
        "for", "function", "goto", "if", "in", "local", "nil",
        "not", "or", "repeat", "return", "then", "true", "until", "while"
    };
    for (const QString &kw : luaKeywords) {
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFormat;
        rules.append(r);
    }

    QStringList luaBuiltins = {
        "print", "require", "type", "pairs", "ipairs", "next",
        "tonumber", "tostring", "unpack", "select", "pcall", "xpcall",
        "error", "assert", "rawget", "rawset", "getmetatable", "setmetatable",
        "coroutine", "string", "table", "math", "io", "os", "debug",
        "string\\.byte", "string\\.char", "string\\.find", "string\\.format",
        "string\\.gmatch", "string\\.gsub", "string\\.len", "string\\.lower",
        "string\\.match", "string\\.rep", "string\\.reverse", "string\\.sub",
        "string\\.upper",
        "table\\.concat", "table\\.insert", "table\\.remove", "table\\.sort",
        "math\\.abs", "math\\.ceil", "math\\.floor", "math\\.max", "math\\.min",
        "math\\.random", "math\\.sqrt", "math\\.sin", "math\\.cos",
        "io\\.open", "io\\.close", "io\\.write", "io\\.read",
        "os\\.clock", "os\\.date", "os\\.time"
    };
    for (const QString &b : luaBuiltins) {
        r.pattern = QRegularExpression("\\b" + b + "\\b");
        r.format = builtinFormat;
        rules.append(r);
    }

    // Strings
    r.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);
    r.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)*'");
    r.format = stringFormat;
    rules.append(r);
    // Long brackets (simplified)
    r.pattern = QRegularExpression("\\[=*\\[.*?\\]=*\\]");
    r.format = stringFormat;
    rules.append(r);

    // Numbers
    r.pattern = QRegularExpression("\\b(?:0[xX][0-9a-fA-F]+|\\d+\\.?\\d*(?:[eE][+-]?\\d+)?)\\b");
    r.format = numberFormat;
    rules.append(r);

    // Comments
    r.pattern = QRegularExpression("--\\[=*\\[.*?\\]=*\\]");
    r.format = commentFormat;
    rules.append(r);
    r.pattern = QRegularExpression("--[^\n]*");
    r.format = commentFormat;
    rules.append(r);

    // Function calls
    r.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*\\s*(?=\\()");
    r.format = functionFormat;
    rules.append(r);

    // Keywords made identifiers (like `function` already handled above)
}

/* ══════════════════════════════════════════════
   C / C++
   ══════════════════════════════════════════════ */

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : LanguageHighlighterBase(parent)
{
    includeFormat.setForeground(QColor(Colors::synPreproc));

    HighlightRule r;

    QStringList cppKeywords = {
        "auto", "break", "case", "catch", "class", "const", "constexpr",
        "continue", "decltype", "default", "delete", "do", "else", "enum",
        "explicit", "export", "extern", "false", "for", "friend", "goto",
        "if", "inline", "mutable", "namespace", "new", "noexcept", "nullptr",
        "operator", "override", "private", "protected", "public", "register",
        "return", "signed", "sizeof", "static", "static_cast", "struct",
        "switch", "template", "this", "throw", "true", "try", "typedef",
        "typeid", "typename", "union", "unsigned", "using", "virtual",
        "volatile", "while", "ifdef", "ifndef", "endif", "define", "undef",
        "pragma", "include", "import", "module", "concept", "requires",
        "co_await", "co_return", "co_yield"
    };
    for (const QString &kw : cppKeywords) {
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFormat;
        rules.append(r);
    }

    // Also add C keywords
    QStringList cKeywords = {
        "_Bool", "_Complex", "_Imaginary", "restrict", "_Alignas",
        "_Alignof", "_Atomic", "_Generic", "_Noreturn", "_Static_assert",
        "_Thread_local", "size_t", "NULL"
    };
    for (const QString &kw : cKeywords) {
        r.pattern = QRegularExpression("\\b" + kw + "\\b");
        r.format = keywordFormat;
        rules.append(r);
    }

    // Types
    QStringList cppTypes = {
        "int", "long", "short", "char", "float", "double", "void",
        "bool", "wchar_t", "char16_t", "char32_t", "int8_t", "int16_t",
        "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "size_t", "ptrdiff_t", "intptr_t", "uintptr_t",
        "string", "vector", "map", "set", "list", "deque", "array",
        "pair", "tuple", "optional", "variant", "any",
        "shared_ptr", "unique_ptr", "weak_ptr",
        "iostream", "fstream", "stringstream",
        "ostream", "istream", "ofstream", "ifstream",
        "iterator", "const_iterator",
        "std::string", "std::vector", "std::map", "std::set",
        "std::cout", "std::cin", "std::endl",
        "FILE", "va_list", "time_t",
        "uint32_t", "uint16_t", "uint8_t", "int32_t", "int16_t", "int8_t"
    };
    for (const QString &t : cppTypes) {
        r.pattern = QRegularExpression("\\b" + t + "\\b");
        r.format = typeFormat;
        rules.append(r);
    }

    // Preprocessor directives
    r.pattern = QRegularExpression("^\\s*#\\s*(include|define|undef|if|ifdef|ifndef|else|elif|endif|pragma|error|warning)\\b");
    r.format = preprocessorFormat;
    rules.append(r);

    // Includes
    r.pattern = QRegularExpression("#\\s*include\\s*<[^>]+>");
    r.format = includeFormat;
    rules.append(r);
    r.pattern = QRegularExpression("#\\s*include\\s*\"[^\"]+\"");
    r.format = includeFormat;
    rules.append(r);

    // Strings
    r.pattern = QRegularExpression("L?\"(?:[^\"\\\\]|\\\\.)*\"");
    r.format = stringFormat;
    rules.append(r);
    r.pattern = QRegularExpression("L?'(?:[^'\\\\]|\\\\.)'");
    r.format = stringFormat;
    rules.append(r);
    // Raw strings (C++11)
    r.pattern = QRegularExpression("R\"\\(.*?\\)\"");
    r.format = stringFormat;
    rules.append(r);

    // Numbers
    r.pattern = QRegularExpression("\\b(?:0[xX][0-9a-fA-F][0-9a-fA_F]*"
                                   "|0[bB][01][01_]*"
                                   "|\\d+\\.?\\d*(?:[eE][+-]?\\d+)?)"
                                   "(?:[uUlLfF]+)?\\b");
    r.format = numberFormat;
    rules.append(r);

    // Comments
    r.pattern = QRegularExpression("//[^\n]*");
    r.format = commentFormat;
    rules.append(r);
    r.pattern = QRegularExpression("/\\*.*?\\*/");
    r.format = commentFormat;
    rules.append(r);

    // Function calls
    r.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*\\s*(?=\\()");
    r.format = functionFormat;
    rules.append(r);
}

void CppHighlighter::extraHighlight(const QString &)
{
}

/* ══════════════════════════════════════════════
   Factory
   ══════════════════════════════════════════════ */

LanguageHighlighterBase *createHighlighterForFile(
    const QString &filePath, QTextDocument *doc)
{
    QString ext = QFileInfo(filePath).suffix().toLower();
    QString name = QFileInfo(filePath).fileName().toLower();

    if (ext == "css")
        return new CssHighlighter(doc);
    if (ext == "html" || ext == "htm" || name == "html" || name == "htm")
        return new HtmlHighlighter(doc);
    if (ext == "js" || ext == "jsx" || ext == "mjs" || ext == "cjs")
        return new JsHighlighter(doc);
    if (ext == "ts" || ext == "tsx")
        return new JsHighlighter(doc); // TS uses same highlighter for now
    if (ext == "rs")
        return new RustHighlighter(doc);
    if (ext == "py")
        return new PythonHighlighter(doc);
    if (ext == "lua")
        return new LuaHighlighter(doc);
    if (ext == "c" || ext == "cpp" || ext == "cxx" || ext == "cc" ||
        ext == "h" || ext == "hpp" || ext == "hxx" || ext == "hh" ||
        ext == "c++" || ext == "h++")
        return new CppHighlighter(doc);

    return nullptr;
}
