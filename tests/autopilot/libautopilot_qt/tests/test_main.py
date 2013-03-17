# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals, GreaterThan
from autopilot.matchers import Eventually

from libautopilot_qt.tests import AutopilotQtTestCase

import unittest
import time
import os
from os import path

class TestQueries(AutopilotQtTestCase):

    def setUp(self):
        super(TestQueries, self).setUp()
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

    def tearDown(self):
        super(TestQueries, self).tearDown()

    def test_find_select_single(self):
        root_item = self.main_window.get_root_item()
        self.assertThat(root_item, NotEquals(None))

    def test_find_by_objectnane(self):
        test_item = self.main_window.get_test_item_by_objectname()
        self.assertThat(test_item, NotEquals(None))

    def test_find_by_child_search(self):
        rectangle = self.main_window.get_test_rectangle_by_child_search()
        self.assertThat(rectangle, NotEquals(None))


class TestProperties(AutopilotQtTestCase):

    def setUp(self):
        super(TestProperties, self).setUp()
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

    def tearDown(self):
        super(TestProperties, self).tearDown()

    def test_basic_properties(self):
        test_item = self.main_window.get_test_item()

        self.assertThat(test_item.stringProperty, Equals("Testing rocks, debugging sucks!"))
        self.assertThat(test_item.intProperty, Equals(42))
        self.assertThat(test_item.boolProperty, Equals(False))
        self.assertThat(test_item.realProperty, Equals(0.42))
        self.assertThat(test_item.doubleProperty, Equals(0.42))

        rectangle = self.main_window.get_test_rectangle()
        self.assertThat(rectangle.color, Equals([0,0,255,255]))

    def test_mouse_interaction(self):
        rectangle = self.main_window.get_test_rectangle()
        self.assertThat(rectangle.color, Equals([0,0,255,255]))

        mousearea = self.main_window.get_test_mousearea()
        self.pointing_device.move_to_object(mousearea)
        self.pointing_device.click()

        self.assertThat(rectangle.color, Eventually(Equals([255,0,0,255])))

class TestAppNameQtDefault(AutopilotQtTestCase):

    def setUp(self):
        super(TestAppNameQtDefault, self).setUp("--appname", "untitled1")

    def test_connection(self):
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

class TestAppNameTech(AutopilotQtTestCase):

    def setUp(self):
        super(TestAppNameTech, self).setUp("--appname", "qt5-test-app")

    def test_connection(self):
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

class TestAppNameUserfriendly(AutopilotQtTestCase):

    def setUp(self):
        super(TestAppNameUserfriendly, self).setUp("--appname", "Qt5 Test App")

    def test_connection(self):
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

class TestAppNameFqdn(AutopilotQtTestCase):

    def setUp(self):
        super(TestAppNameFqdn, self).setUp("--appname", "com.ubuntu.qt5testapp")

    def test_connection(self):
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))
