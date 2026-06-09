#ifndef OUTPUTPANEL_H
#define OUTPUTPANEL_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QVector>

class OutputPanel : public QWidget {
    Q_OBJECT
public:
    explicit OutputPanel(QWidget *parent = nullptr);

    void appendOutput(const QString &text);
    void appendError(const QString &text);
    void clearOutput();
    void setRunning(bool running);

    void addProblem(const QString &file, int line, int col,
                    const QString &message, const QString &severity);
    void clearProblems();

signals:
    void stopRequested();
    void problemActivated(const QString &filePath, int line);

private slots:
    void onTabClicked(int index);
    void onProblemItemActivated(QTreeWidgetItem *item, int column);

private:
    QLabel *titleLabel;
    QPushButton *clearBtn;
    QPushButton *stopBtn;

    struct Tab {
        QPushButton *btn;
        int index;
    };
    QVector<Tab> tabs;
    QWidget *tabBar;
    int activeTab = 0;

    QStackedWidget *stack;
    QPlainTextEdit *output;
    QTreeWidget *problemTree;

    void setActiveTab(int index);
    QPushButton *createTabBtn(const QString &text, int index);
};

#endif
