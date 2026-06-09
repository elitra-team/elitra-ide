#ifndef FILEICONPROVIDER_H
#define FILEICONPROVIDER_H

#include <QString>
#include <QColor>

class FileIconProvider {
public:
    static QColor colorForFile(const QString &filePath);
    static QString iconLetterForFile(const QString &filePath);
    static QString fileNameForTab(const QString &filePath);
};

#endif
