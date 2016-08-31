QT += core
QT += network
QT += gui
QT += widgets
QT += sql

CONFIG += c++11

TARGET = Project
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    settingswindow.cpp \
    requestsender.cpp \
    replyhandler.cpp

HEADERS += \
    settingswindow.h \
    requestsender.h \
    replyhandler.h
