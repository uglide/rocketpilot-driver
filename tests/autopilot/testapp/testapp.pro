TEMPLATE = app

TARGET = qt5testapp
QT += widgets quick quickwidgets
qmlfile.file = qt5.qml

DEFINES += QT5_SUPPORT

SOURCES += testapp.cpp

qmlfile.path=/usr/share/libautopilot-qt/

target.path=/usr/share/libautopilot-qt/
target.file = $TARGET

INSTALLS += target qmlfile 

OTHER_FILES += \
    $$system(ls ./*.qml)
