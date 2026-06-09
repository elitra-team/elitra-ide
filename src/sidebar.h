#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTreeView>
#include <QFileSystemModel>

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = nullptr);

    void setRootPath(const QString &path);
    QTreeView *fileTree() const { return fileTree_; }
    QFileSystemModel *fileModel() const { return fileModel_; }

    void setSearchView(QWidget *searchWidget);

signals:
    void closeRequested();

private:
    QLabel *titleLabel;
    QPushButton *closeBtn;
    QStackedWidget *stack;
    QTreeView *fileTree_;
    QFileSystemModel *fileModel_;
    QWidget *searchContainer;
};

#endif
