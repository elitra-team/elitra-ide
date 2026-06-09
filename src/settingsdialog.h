#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QColor>
#include <QMap>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    int fontSize() const;
    QString fontFamily() const;
    QString themeName() const;
    int tabSize() const;
    bool showLineNumbers() const;

    void setFontSize(int size);
    void setFontFamily(const QString &family);
    void setThemeName(const QString &theme);
    void setTabSize(int size);
    void setShowLineNumbers(bool show);

signals:
    void settingsApplied();

private slots:
    void apply();
    void ok();

private:
    QTabWidget *tabs;

    QFontComboBox *fontFamilyCb;
    QSpinBox *fontSizeSp;
    QSpinBox *tabSizeSp;
    QCheckBox *lineNumbersCb;

    QComboBox *themeCb;

    QMap<QString, QString> themes;
};

#endif
