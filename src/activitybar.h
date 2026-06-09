#ifndef ACTIVITYBAR_H
#define ACTIVITYBAR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

class ActivityBar : public QWidget {
    Q_OBJECT
public:
    explicit ActivityBar(QWidget *parent = nullptr);

    enum View { Explorer = 0, Search, SourceControl, Run, Settings };
    void setActive(View view);
    View activeView() const { return active_; }

signals:
    void viewToggled(ActivityBar::View view);

private:
    struct Btn {
        QPushButton *button;
        View view;
    };
    QVector<Btn> buttons;
    View active_ = Explorer;

    QPushButton *addButton(const QString &text, const QString &tooltip, View view);
    void updateActive();
};

#endif
