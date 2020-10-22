#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T12:31:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myMaze
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    maze.cpp \
    interface.cpp

HEADERS  += mainwindow.h \
    maze.h \
    interface.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
RC_ICONS = down.ico
