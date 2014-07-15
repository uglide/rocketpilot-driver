#include(../../../coverage.pri)

CONFIG += testcase
TARGET = tst_qtnode

QT += testlib dbus widgets quick

CONFIG += link_pkgconfig debug
PKGCONFIG += xpathselect
QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined

contains(QT_VERSION, ^5\\..\\..*) {
    DEFINES += QT5_SUPPORT
}

INCLUDEPATH += ../../../driver

SOURCES += \
	tst_qtnode.cpp \
    ../../../driver/introspection.cpp \
    ../../../driver/rootnode.cpp \
    ../../../driver/qtnode.cpp

HEADERS += \
    ../../../driver/introspection.h \
    ../../../driver/rootnode.h \
    ../../../driver/qtnode.h
