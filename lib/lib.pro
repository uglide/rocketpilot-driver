TEMPLATE = lib
TARGET = qttestability
DESTDIR=..

# disable qt includes and linkage (core and gui are enabled per default with Qt4)
QT -= core gui

QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined
QMAKE_CXXFLAGS -= -pedantic

SOURCES = qttestability.cpp
HEADERS = qttestability.h

target.file = libtestability*
INSTALLS += target
