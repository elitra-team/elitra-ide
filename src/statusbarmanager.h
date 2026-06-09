#ifndef STATUSBARMANAGER_H
#define STATUSBARMANAGER_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class StatusBarManager : public QWidget {
    Q_OBJECT
public:
    explicit StatusBarManager(QWidget *parent = nullptr);

    void setFileName(const QString &name, bool modified);
    void setCursorPosition(int line, int column);
    void setLanguage(const QString &lang);
    void setEncoding(const QString &enc);
    void setGitBranch(const QString &branch);
    void setMode(const QString &mode);

private:
    QLabel *leftLabel;
    QLabel *gitLabel;
    QLabel *modeLabel;
    QLabel *encodingLabel;
    QLabel *langLabel;
    QLabel *positionLabel;

    void updateVisibility();
};

#endif
