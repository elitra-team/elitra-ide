#ifndef FINDREPLACEPANEL_H
#define FINDREPLACEPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>

class CodeEditor;

class FindReplacePanel : public QWidget {
    Q_OBJECT
public:
    explicit FindReplacePanel(QWidget *parent = nullptr);

    void setEditor(CodeEditor *editor);
    void showFind();
    void showReplace();
    void focusFindInput();

signals:
    void closed();

public slots:
    void findNext();
    void findPrev();

private slots:
    void onTextChanged(const QString &text);
    void replace();
    void replaceAll();

private:
    CodeEditor *editor = nullptr;

    QLineEdit *findInput;
    QLineEdit *replaceInput;
    QPushButton *prevBtn;
    QPushButton *nextBtn;
    QPushButton *replaceBtn;
    QPushButton *replaceAllBtn;
    QPushButton *closeBtn;
    QCheckBox *caseSensitive;
    QCheckBox *wholeWord;
    QCheckBox *regexMode;
    QWidget *replaceRow;
    QLabel *matchCount;
};

#endif
