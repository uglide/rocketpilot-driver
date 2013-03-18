# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""libautopilot-qt autopilot tests."""

import os

from autopilot.introspection.qt import QtIntrospectionTestMixin
from autopilot.testcase import AutopilotTestCase

from libautopilot_qt.emulators.main_window_qt4 import MainWindowQt4
from libautopilot_qt.emulators.main_window_qt5 import MainWindowQt5

import logging
logger = logging.getLogger(__name__)


class AutopilotQtTestCase(AutopilotTestCase, QtIntrospectionTestMixin):
    qt_version = 0

    def setUp(self, *app_args):
        super(AutopilotQtTestCase, self).setUp()
        self.launch_test_app(app_args)

    def launch_test_app(self, *app_args):
        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            path = "/usr/share/libautopilot-qt/"
        else:
            # Load library from local build dir
            os.environ['LD_LIBRARY_PATH'] = "../../"
            path = "testapp/"

        app_name_qt4 = path + "qt4testapp"
        app_name_qt5 = path + "qt5testapp"

        qt_select = os.environ.get('QT_SELECT')

        if os.path.isfile(app_name_qt5) and not qt_select == "qt4":
            logger.info("Found Qt5 test app")
            app_name = app_name_qt5
            qml_file = path + "qt5.qml"
            self.qt_version = 5
        elif os.path.isfile(app_name_qt4) and not qt_select == "qt5":
            logger.info("Found Qt4 test app")
            app_name = app_name_qt4
            qml_file = path + "qt4.qml"
            self.qt_version = 4
        else:
            logger.error("Could not find test app.")

        args = [app_name]
        args.extend(*app_args)
        args.append(qml_file)
        self.app = self.launch_test_application(*args)

    @property
    def main_window(self):
        if self.qt_version == 4:
            return MainWindowQt4(self.app)
        if self.qt_version == 5:
            return MainWindowQt5(self.app)
