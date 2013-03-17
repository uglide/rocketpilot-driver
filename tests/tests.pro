TEMPLATE = subdirs
SUBDIRS += autopilot

#contains(QT_VERSION, ^5\\..\\..*) {
    SUBDIRS += unittests
#}
