CONFIG += debug
TEMPLATE = subdirs

# only build the main lib once, with the qt5 driver
contains(QT_VERSION, ^5\\..\\..*) {
    SUBDIRS += lib
}

SUBDIRS += driver tests
