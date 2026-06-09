#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    setMinimumSize(450, 350);
    setStyleSheet(
        "QDialog { background-color: #1a1a1e; }"
        "QLabel { color: #d4d4d4; font-size: 12px; }"
        "QGroupBox { color: #d4d4d4; font-size: 13px; font-weight: 600; "
        "  border: 1px solid #3d3d42; border-radius: 6px; margin-top: 12px; padding-top: 16px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }"
    );

    auto *mainLayout = new QVBoxLayout(this);

    tabs = new QTabWidget;
    tabs->setStyleSheet(
        "QTabWidget::pane { border: none; background: transparent; }"
        "QTabBar { background-color: #242428; }"
        "QTabBar::tab { background-color: #242428; color: #9e9e9e; border: none; "
        "  padding: 7px 18px; font-size: 12px; }"
        "QTabBar::tab:selected { background-color: #1a1a1e; color: #d4d4d4; "
        "  border-top: 2px solid #e6a341; }"
        "QTabBar::tab:hover:!selected { background-color: #35353a; color: #d4d4d4; }"
    );

    /* ── Editor Tab ── */
    auto *editorTab = new QWidget;
    auto *editorForm = new QFormLayout(editorTab);
    editorForm->setSpacing(10);
    editorForm->setContentsMargins(20, 20, 20, 20);

    fontFamilyCb = new QFontComboBox;
    fontFamilyCb->setStyleSheet(
        "QFontComboBox { background-color: #2d2d32; color: #d4d4d4; border: 1px solid #3d3d42; "
        "border-radius: 4px; padding: 4px 8px; font-size: 12px; }"
    );
    editorForm->addRow("Font Family:", fontFamilyCb);

    fontSizeSp = new QSpinBox;
    fontSizeSp->setRange(8, 40);
    fontSizeSp->setValue(12);
    fontSizeSp->setStyleSheet(
        "QSpinBox { background-color: #2d2d32; color: #d4d4d4; border: 1px solid #3d3d42; "
        "border-radius: 4px; padding: 4px 8px; font-size: 12px; }"
    );
    editorForm->addRow("Font Size:", fontSizeSp);

    tabSizeSp = new QSpinBox;
    tabSizeSp->setRange(1, 8);
    tabSizeSp->setValue(4);
    tabSizeSp->setStyleSheet(fontSizeSp->styleSheet());
    editorForm->addRow("Tab Size:", tabSizeSp);

    lineNumbersCb = new QCheckBox("Show line numbers");
    lineNumbersCb->setChecked(true);
    lineNumbersCb->setStyleSheet(
        "QCheckBox { color: #d4d4d4; font-size: 12px; spacing: 6px; }"
        "QCheckBox::indicator { width: 16px; height: 16px; border: 1px solid #3d3d42; "
        "border-radius: 3px; background: #2d2d32; }"
        "QCheckBox::indicator:checked { background: #e6a341; border-color: #e6a341; }"
    );
    editorForm->addRow("", lineNumbersCb);

    tabs->addTab(editorTab, "Editor");

    /* ── Theme Tab ── */
    auto *themeTab = new QWidget;
    auto *themeForm = new QFormLayout(themeTab);
    themeForm->setSpacing(10);
    themeForm->setContentsMargins(20, 20, 20, 20);

    themes["Warm Stone"] = "#1a1a1e";
    themes["Cool Gray"] = "#242428";
    themes["Light"] = "#ffffff";
    themes["Tokyo Night"] = "#1a1b26";
    themes["Dracula"] = "#282a36";

    themeCb = new QComboBox;
    themeCb->addItems(themes.keys());
    themeCb->setStyleSheet(
        "QComboBox { background-color: #2d2d32; color: #d4d4d4; border: 1px solid #3d3d42; "
        "border-radius: 4px; padding: 4px 8px; font-size: 12px; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background-color: #2d2d32; color: #d4d4d4; "
        "border: 1px solid #3d3d42; selection-background-color: #3d3d42; }"
    );
    themeForm->addRow("Theme:", themeCb);

    auto *previewLabel = new QLabel("Preview area");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setMinimumHeight(100);
    previewLabel->setStyleSheet(
        "QLabel { background-color: #1a1a1e; color: #d4d4d4; border: 1px solid #3d3d42; "
        "border-radius: 6px; font-size: 12px; }"
    );
    themeForm->addRow(previewLabel);

    connect(themeCb, &QComboBox::currentTextChanged, this, [this, previewLabel](const QString &theme) {
        QString bg = themes.value(theme, "#1a1b26");
        previewLabel->setStyleSheet(
            QString("QLabel { background-color: %1; color: #d4d4d4; border: 1px solid #3d3d42; "
                    "border-radius: 6px; font-size: 12px; }").arg(bg));
    });

    tabs->addTab(themeTab, "Appearance");

    mainLayout->addWidget(tabs);

    auto *buttonBox = new QDialogButtonBox;
    auto *applyBtn = buttonBox->addButton("Apply", QDialogButtonBox::ApplyRole);
    auto *okBtn = buttonBox->addButton("OK", QDialogButtonBox::AcceptRole);
    auto *cancelBtn = buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);

    QString btnStyle =
        "QPushButton { background-color: #3d3d42; color: #d4d4d4; border: 1px solid #3d3d42; "
        "padding: 6px 20px; border-radius: 5px; font-size: 12px; }"
        "QPushButton:hover { background-color: #35353a; border-color: #e6a341; }";
    applyBtn->setStyleSheet(btnStyle);
    okBtn->setStyleSheet(btnStyle);
    cancelBtn->setStyleSheet(btnStyle);
    applyBtn->setStyleSheet(btnStyle.replace("#3d3d42", "#e6a341").replace("#d4d4d4", "#ffffff"));

    mainLayout->addWidget(buttonBox);

    connect(applyBtn, &QPushButton::clicked, this, &SettingsDialog::apply);
    connect(okBtn, &QPushButton::clicked, this, &SettingsDialog::ok);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

int SettingsDialog::fontSize() const { return fontSizeSp->value(); }
QString SettingsDialog::fontFamily() const { return fontFamilyCb->currentFont().family(); }
QString SettingsDialog::themeName() const { return themeCb->currentText(); }
int SettingsDialog::tabSize() const { return tabSizeSp->value(); }
bool SettingsDialog::showLineNumbers() const { return lineNumbersCb->isChecked(); }

void SettingsDialog::setFontSize(int size) { fontSizeSp->setValue(size); }
void SettingsDialog::setFontFamily(const QString &family) { fontFamilyCb->setCurrentFont(QFont(family)); }
void SettingsDialog::setThemeName(const QString &theme) { themeCb->setCurrentText(theme); }
void SettingsDialog::setTabSize(int size) { tabSizeSp->setValue(size); }
void SettingsDialog::setShowLineNumbers(bool show) { lineNumbersCb->setChecked(show); }

void SettingsDialog::apply()
{
    emit settingsApplied();
}

void SettingsDialog::ok()
{
    emit settingsApplied();
    accept();
}
