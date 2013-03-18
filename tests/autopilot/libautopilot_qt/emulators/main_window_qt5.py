# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import logging
logger = logging.getLogger(__name__)


class MainWindowQt5(object):

    def __init__(self, app):
        self.app = app

    def get_qml_view(self):
        qml_view = self.app.select_single("QQuickView")
        if qml_view is None:
            logger.error("*** select_single(\"QQuickView\") failed ***")
        return qml_view

    def get_root_item(self):
        root_item = self.app.select_single("QQuickItem", objectName="rootItem")
        if root_item is None:
            logger.error("*** select_single(\"QQuickItem\", objectName=\rootItem\") failed ***")
        return root_item

    def get_test_item(self):
        test_item = self.app.select_single("QQuickItem", objectName="testItem")
        if test_item is None:
            logger.error("*** select_single(\"QQuickItem\", objectName=\"testItem\") failed ***")
        return test_item

    def get_test_item_by_objectname(self):
        test_item = self.app.select_single(objectName="testItem")
        if test_item is None:
            logger.error("*** select_single(objectName=\"testItem\") failed ***")
        return test_item

    def get_test_rectangle(self):
        rectangle = self.app.select_single("QQuickRectangle")
        if rectangle is None:
            logger.error("*** select_single(\"QQuickRectangle\") failed ***")
        return rectangle

    def get_test_rectangle_by_child_search(self):
        rectangle = self.get_root_item().get_children_by_type("QQuickRectangle")[0]
        if rectangle is None:
            logger.error("*** get_children_by_type(\"QQuickRectangle\")[0] failed ***")
        return rectangle

    def get_test_mousearea(self):
        mousearea = self.app.select_single("QQuickMouseArea")
        if mousearea is None:
            logger.error("*** select_single(\"QQuickMouseArea\") failed ***")
        return mousearea
