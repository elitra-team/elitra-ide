#include "fileiconprovider.h"
#include "colors.h"
#include <QFileInfo>

QColor FileIconProvider::colorForFile(const QString &filePath)
{
    QString ext = QFileInfo(filePath).suffix().toLower();
    QString name = QFileInfo(filePath).fileName().toLower();

    if (ext == "eltr") return QColor(Colors::fiEltr);
    if (ext == "cpp" || ext == "cxx" || ext == "cc") return QColor(Colors::fiCpp);
    if (ext == "c" || ext == "h" || ext == "hpp" || ext == "hh") return QColor(Colors::fiC);
    if (ext == "rs") return QColor(Colors::fiRust);
    if (ext == "py") return QColor(Colors::fiPython);
    if (ext == "js" || ext == "jsx" || ext == "ts" || ext == "tsx") return QColor(Colors::fiJs);
    if (ext == "html" || ext == "htm") return QColor(Colors::fiHtml);
    if (ext == "css" || ext == "scss" || ext == "less") return QColor(Colors::fiCss);
    if (ext == "json") return QColor(Colors::fiJson);
    if (ext == "md" || ext == "markdown") return QColor(Colors::fiMd);
    if (ext == "lua") return QColor(Colors::fiLua);
    if (ext == "toml") return QColor(Colors::fiToml);
    if (ext == "yml" || ext == "yaml") return QColor(Colors::fiYaml);
    if (name == "makefile" || name == "cmakelists.txt") return QColor(Colors::fiMakefile);
    if (name == "dockerfile") return QColor(Colors::fiDockerfile);

    return QColor(Colors::fiDefault);
}

QString FileIconProvider::iconLetterForFile(const QString &filePath)
{
    QString ext = QFileInfo(filePath).suffix().toLower();
    QString name = QFileInfo(filePath).fileName().toLower();

    if (ext == "eltr") return "E";
    if (ext == "cpp" || ext == "cxx" || ext == "cc") return "C";
    if (ext == "c" || ext == "h") return "C";
    if (ext == "hpp") return "H";
    if (ext == "rs") return "R";
    if (ext == "py") return "P";
    if (ext == "js" || ext == "jsx") return "J";
    if (ext == "ts" || ext == "tsx") return "T";
    if (ext == "html" || ext == "htm") return "H";
    if (ext == "css" || ext == "scss" || ext == "less") return "#";
    if (ext == "json") return "{";
    if (ext == "md") return "M";
    if (ext == "lua") return "L";
    if (ext == "toml") return "T";
    if (ext == "yml" || ext == "yaml") return "Y";
    if (name == "makefile") return "M";
    if (name == "dockerfile") return "D";

    return "\u25CB";
}

QString FileIconProvider::fileNameForTab(const QString &filePath)
{
    QFileInfo fi(filePath);
    QString name = fi.fileName();
    if (name.length() > 25)
        name = name.left(22) + "...";
    return name;
}
