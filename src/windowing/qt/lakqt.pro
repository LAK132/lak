TEMPLATE = lib

DESTDIR = bin/
DLLDESTDIR = bin/
OBJECTS_DIR = obj/
INCLUDEPATH += ../../inc/

TARGET = lakqt

CONFIG += c++17 staticlib
DEFINES += LAK_USE_QT

SOURCES += events.cpp platform.cpp window.cpp

QT += widgets

QMAKE_CXXFLAGS += -Wall -Werror -Wfatal-errors
