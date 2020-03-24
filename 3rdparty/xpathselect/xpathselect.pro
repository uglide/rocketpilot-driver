TEMPLATE = lib
TARGET = xpathselect
DESTDIR=..

win32* {
   CONFIG += c++11 staticlib
   INCLUDEPATH += $$PWD/boost.1.71.0.0/lib/native/include
} else {
   QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined
   QMAKE_CXXFLAGS -= -pedantic
}

SOURCES = *.cpp
HEADERS = *.h

target.file = libxpathselect*
INSTALLS += target

unix:macx {
  CONFIG += staticlib
  INCLUDEPATH += /usr/local/opt/boost/include/
}
