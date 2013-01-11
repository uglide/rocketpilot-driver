
TEMPLATE = lib
TARGET = qttestability
DESTDIR=..
QT =
QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined
QMAKE_CXXFLAGS -= -pedantic

SOURCES = qttestability.cpp
HEADERS = qttestability.h

target.file = libtestability*

#version check qt
#contains(QT_VERSION, ^5\\..\\..*) {
#    DEFINES += QT5_SUPPORT
#    target.path = /opt/qt5/lib
#} else {
#    target.path = /usr/lib
#}

INSTALLS += target
