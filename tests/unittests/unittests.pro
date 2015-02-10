#include(../../coverage.pri)

CONFIG += testcase
TARGET = tst_libautopilot-qt

QT += testlib dbus widgets quick quickwidgets

CONFIG += link_pkgconfig debug
PKGCONFIG += xpathselect
QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined

contains(QT_VERSION, ^5\\..\\..*) {
    DEFINES += QT5_SUPPORT
}

INCLUDEPATH += ../../driver

SOURCES += \
	tst_main.cpp \
	tst_qtnode.cpp \
	tst_introspection.cpp \
    ../../driver/introspection.cpp \
    ../../driver/rootnode.cpp \
    ../../driver/qtnode.cpp

HEADERS += \
    tst_qtnode.h \
    tst_introspection.h \
    ../../driver/introspection.h \
    ../../driver/rootnode.h \
    ../../driver/qtnode.h
