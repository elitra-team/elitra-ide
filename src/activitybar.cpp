#include "activitybar.h"

ActivityBar::ActivityBar(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(48);
    setStyleSheet(
        "ActivityBar { background-color: #242428; }"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addSpacing(8);

    addButton("\u2261", "Explorer (Ctrl+Shift+E)", Explorer);
    addButton("\u25CB", "Search (Ctrl+Shift+F)", Search);
    addButton("\u25B6", "Run (Ctrl+Shift+D)", Run);

    layout->addStretch();

    addButton("\u2699", "Settings", Settings);

    updateActive();
}

QPushButton *ActivityBar::addButton(const QString &text, const QString &tooltip, View view)
{
    auto *btn = new QPushButton(text);
    btn->setFixedSize(48, 48);
    btn->setToolTip(tooltip);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFont(QFont("Segoe UI", 16));
    btn->setStyleSheet(
        "QPushButton { background: transparent; color: #9e9e9e; border: none; "
        "border-left: 3px solid transparent; }"
        "QPushButton:hover { background-color: #35353a; color: #d4d4d4; }"
    );

    static_cast<QVBoxLayout *>(layout())->insertWidget(
        static_cast<QVBoxLayout *>(layout())->count() - 1, btn);

    buttons.append({btn, view});

    connect(btn, &QPushButton::clicked, this, [this, view]() {
        if (active_ == view) {
            emit viewToggled(static_cast<View>(-1));
        } else {
            setActive(view);
            emit viewToggled(view);
        }
    });

    return btn;
}

void ActivityBar::setActive(View view)
{
    active_ = view;
    updateActive();
}

void ActivityBar::updateActive()
{
    for (const auto &b : buttons) {
        bool isActive = (b.view == active_);
        b.button->setStyleSheet(
            isActive
                ? "QPushButton { background: transparent; color: #d4d4d4; border: none; "
                  "border-left: 3px solid #e6a341; }"
                : "QPushButton { background: transparent; color: #9e9e9e; border: none; "
                  "border-left: 3px solid transparent; }"
            "QPushButton:hover { background-color: #35353a; color: #d4d4d4; }"
        );
    }
}
