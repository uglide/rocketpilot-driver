/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Michael Zanetti <michael.zanetti@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QApplication>
#include <QStringList>

#ifdef QT5_SUPPORT
#include <QQuickView>
#else
#include <QDeclarativeView>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList args = QApplication::arguments();
    QString sourceFile = args.last();

    int appNameIndex = args.indexOf("--appname");
    if(appNameIndex > 0 && args.count() >= appNameIndex) {
        app.setApplicationName(args.at(appNameIndex+1));
    }

#ifdef QT5_SUPPORT
    QQuickView view;
#else
    QDeclarativeView view;
#endif

    view.setSource(QUrl(sourceFile));

    view.show();
    app.exec();
}
