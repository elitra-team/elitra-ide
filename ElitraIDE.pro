QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = ElitraIDE
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/elitrahighlighter.cpp \
    src/languagehighlighter.cpp \
    src/codeeditor.cpp \
    src/outputpanel.cpp \
    src/findreplacepanel.cpp \
    src/findinfilespanel.cpp \
    src/lspclient.cpp \
    src/settingsdialog.cpp \
    src/fileiconprovider.cpp \
    src/activitybar.cpp \
    src/sidebar.cpp \
    src/statusbarmanager.cpp

HEADERS += \
    src/mainwindow.h \
    src/elitrahighlighter.h \
    src/languagehighlighter.h \
    src/codeeditor.h \
    src/outputpanel.h \
    src/findreplacepanel.h \
    src/findinfilespanel.h \
    src/lspclient.h \
    src/settingsdialog.h \
    src/fileiconprovider.h \
    src/activitybar.h \
    src/sidebar.h \
    src/statusbarmanager.h \
    src/colors.h

INCLUDEPATH += src
