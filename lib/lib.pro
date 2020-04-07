TEMPLATE = lib
TARGET = qttestability
DESTDIR=..

# disable qt includes and linkage (core and gui are enabled per default with Qt4)
QT -= core gui

win32* {
    SOURCES = $$PWD/windows/qttestability.cpp
    HEADERS = $$PWD/windows/qttestability.h
} else {
    QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined
    QMAKE_CXXFLAGS -= -pedantic
    SOURCES = $$PWD/linux/qttestability.cpp
    HEADERS = $$PWD/linux/qttestability.h
}

target.file = libtestability*
INSTALLS += target
