
TEMPLATE = lib
TARGET = qttestability
QT = core gui dbus

QMAKE_CXXFLAGS += -std=c++0x

SOURCES = qttestability.cpp \
          dbus_adaptor.cpp \
          dbus_object.cpp \
          introspection.cpp

HEADERS = qttestability.h \
          dbus_adaptor.h \
          dbus_object.h \
          introspection.h
