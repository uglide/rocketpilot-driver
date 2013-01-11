
TEMPLATE = lib
#version check qt
contains(QT_VERSION, ^5\\..\\..*) {
    DEFINES += QT5_SUPPORT
    TARGET = autopilot_driver_qt5
} else {
  TARGET = autopilot_driver_qt4
}

DESTDIR=..
QT = core gui dbus quick widgets testlib
CONFIG += link_pkgconfig
PKGCONFIG += xpathselect
QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined

SOURCES = qttestability.cpp \
          dbus_adaptor.cpp \
          dbus_object.cpp \
          introspection.cpp \
    rootnode.cpp \
    qtnode.cpp \
    dbus_adaptor_qt.cpp

HEADERS = qttestability.h \
          dbus_adaptor.h \
          dbus_object.h \
          introspection.h \
    rootnode.h \
    qtnode.h \
          introspection.h \
    dbus_adaptor_qt.h

target.file = libtestability*


target.path = /usr/lib
INSTALLS += target
