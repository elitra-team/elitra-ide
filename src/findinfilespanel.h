#ifndef FINDINFILESPANEL_H
#define FINDINFILESPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QLabel>
#include <QDirIterator>

class FindInFilesPanel : public QWidget {
    Q_OBJECT
public:
    explicit FindInFilesPanel(QWidget *parent = nullptr);

    void setSearchRoot(const QString &root);
    void focusSearch();

signals:
    void fileActivated(const QString &filePath, int line);
    void closed();

private slots:
    void startSearch();
    void onItemActivated(QTreeWidgetItem *item, int column);

private:
    QLineEdit *searchInput;
    QLineEdit *filterInput;
    QPushButton *searchBtn;
    QPushButton *closeBtn;
    QCheckBox *caseSensitive;
    QCheckBox *wholeWord;
    QTreeWidget *resultsTree;
    QLabel *statusLabel;
    QString searchRoot;
};

#endif
