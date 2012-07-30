
TEMPLATE = lib
TARGET = qttestability
DESTDIR=..
QT = core gui dbus
CONFIG += link_pkgconfig
PKGCONFIG += indicate-qt
QMAKE_CXXFLAGS += -std=c++0x

SOURCES = qttestability.cpp \
          dbus_adaptor.cpp \
          dbus_object.cpp \
          introspection.cpp

HEADERS = qttestability.h \
          dbus_adaptor.h \
          dbus_object.h \
          introspection.h

target.file = libtestability*
target.path = /usr/lib
INSTALLS += target
