TEMPLATE = subdirs

SUBDIRS += lib driver tests

win32* {
    SUBDIRS += 3rdparty/xpathselect
}
