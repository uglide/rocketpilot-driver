TEMPLATE = app

contains(QT_VERSION, ^5\\..\\..*) {
    TARGET = qt5testapp

    QT += widgets quick

    DEFINES += QT5_SUPPORT
} else {
    TARGET = qt4testapp

    QT += declarative
}

SOURCES += testapp.cpp
