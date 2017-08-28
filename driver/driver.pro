TEMPLATE = lib

DEFINES += QT5_SUPPORT
TARGET = autopilot_driver_qt5

DESTDIR=..
QT = core gui dbus quick quickwidgets widgets testlib
QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined

if (unix:!macx) {
    CONFIG += link_pkgconfig
    PKGCONFIG += xpathselect
} else {
    INCLUDEPATH += $$PWD/../3rdparty/
    LIBS += $$PWD/../3rdparty/libxpathselect.a
}

message($$INCLUDEPATH)

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
          dbus_adaptor_qt.h \
          autopilot_types.h

target.file = libtestability*


target.path = /usr/lib
INSTALLS += target
