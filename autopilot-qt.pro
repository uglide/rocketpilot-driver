
TEMPLATE = lib
TARGET = qttestability
QT = core gui dbus

QMAKE_CXXFLAGS += -std=c++0x

SOURCES = src/qttestability.cpp \
            src/dbus_adaptor.cpp \
            src/dbus_object.cpp \
#            src/main.cpp \
            src/introspection.cpp

HEADERS = src/qttestability.h \
            src/dbus_adaptor.h \
            src/dbus_object.h \
            src/introspection.h
