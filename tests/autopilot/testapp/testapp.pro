TEMPLATE = app

contains(QT_VERSION, ^5\\..\\..*) {
    TARGET = qt5testapp
    QT += widgets quick
    qmlfile.file = qt5.qml

    DEFINES += QT5_SUPPORT
} else {
    TARGET = qt4testapp
    QT += declarative
    qmlfile.file = qt4.qml
}

SOURCES += testapp.cpp

qmlfile.path=/usr/share/libautopilot-qt/

target.path=/usr/share/libautopilot-qt/
target.file = $TARGET

INSTALLS += target qmlfile 

OTHER_FILES += \
    $$system(ls ./*.qml)
