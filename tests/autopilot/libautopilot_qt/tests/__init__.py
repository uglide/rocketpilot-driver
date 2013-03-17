# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""libautopilot-qt autopilot tests."""

from os import remove
import os

from autopilot.introspection.qt import QtIntrospectionTestMixin
from autopilot.testcase import AutopilotTestCase

from libautopilot_qt.emulators.main_window import MainWindow

class AutopilotQtTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    def setUp(self, *app_args):
        super(AutopilotQtTestCase, self).setUp()
        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            self.launch_test_installed(app_args)
        else:
            self.launch_test_local(app_args)

    def launch_test_local(self, *app_args):
        # Load library from build dir
        os.environ['LD_LIBRARY_PATH'] = "../../"

        args = ["testapps/qt5testapp"]
        args.extend(*app_args)
        args.append("testapps/qt5.qml")
        print("blabla {}".format(args))
        self.app = self.launch_test_application(*args)

    def launch_test_installed(self, *app_args):
        args = ["/usr/share/libautopilot-qt/tests/qt5testapp"]
        args.extend(*app_args)
        args.append("/usr/share/libautopilot-qt/tests/qt5.qml")
        self.app = self.launch_test_application(*args)

    @property
    def main_window(self):
        return MainWindow(self.app)

