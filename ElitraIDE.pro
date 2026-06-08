QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = ElitraIDE
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/elitrahighlighter.cpp \
    src/codeeditor.cpp \
    src/outputpanel.cpp

HEADERS += \
    src/mainwindow.h \
    src/elitrahighlighter.h \
    src/codeeditor.h \
    src/outputpanel.h

INCLUDEPATH += src
