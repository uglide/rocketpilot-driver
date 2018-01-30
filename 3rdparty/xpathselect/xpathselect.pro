TEMPLATE = lib
TARGET = xpathselect
DESTDIR=..

QMAKE_CXXFLAGS += -std=c++0x -Wl,--no-undefined
QMAKE_CXXFLAGS -= -pedantic

SOURCES = *.cpp
HEADERS = *.h

target.file = libxpathselect*
INSTALLS += target

unix:macx {
  CONFIG += staticlib
  INCLUDEPATH += /usr/local/opt/boost/include/
}
