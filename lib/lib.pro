TEMPLATE = lib
TARGET = qttestability
DESTDIR=..

# disable qt includes and linkage (core and gui are enabled per default with Qt4)
QT -= core gui

QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined
QMAKE_CXXFLAGS -= -pedantic

win32* {
    SOURCES = $$PWD/windows/qttestability.cpp
    HEADERS = $$PWD/windows/qttestability.h
}

unix:!macx {
    SOURCES = $$PWD/linux/qttestability.cpp
    HEADERS = $$PWD/linux/qttestability.h
}

target.file = libtestability*
INSTALLS += target
