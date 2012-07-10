TEMPLATE=app
TARGET=test_runner
DESTDIR=..
QT=core testlib dbus
QMAKE_CXXFLAGS += -std=c++0x
INCLUDEPATH += ../lib
LIBS += -L.. -lqttestability
SOURCES=main.cpp \
        test_dbus_introspection.cpp

HEADERS = test_dbus_introspection.h
