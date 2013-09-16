#include(../../coverage.pri)

CONFIG += testcase
TARGET = tst_introspection

QT += testlib dbus widgets quick

CONFIG += link_pkgconfig debug
PKGCONFIG += xpathselect
QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined

contains(QT_VERSION, ^5\\..\\..*) {
    DEFINES += QT5_SUPPORT
}

INCLUDEPATH += ../../driver

SOURCES += \
	tst_introspection.cpp \
    ../../driver/introspection.cpp \
    ../../driver/rootnode.cpp \
    ../../driver/qtnode.cpp

HEADERS += \
    ../../driver/introspection.h \
    ../../driver/rootnode.h \
    ../../driver/qtnode.h
