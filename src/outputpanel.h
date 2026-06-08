#ifndef OUTPUTPANEL_H
#define OUTPUTPANEL_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class OutputPanel : public QWidget {
    Q_OBJECT
public:
    explicit OutputPanel(QWidget *parent = nullptr);

    void appendOutput(const QString &text);
    void appendError(const QString &text);
    void clearOutput();
    void setRunning(bool running);

signals:
    void stopRequested();

private:
    QPlainTextEdit *output;
    QPushButton *clearBtn;
    QPushButton *stopBtn;
    QLabel *titleLabel;
};

#endif
