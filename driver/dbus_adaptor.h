/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/


#ifndef DBUS_ADAPTOR_H
#define DBUS_ADAPTOR_H

#include <QObject>
#include <QtDBus>

class QString;


/*
 * Adaptor class for interface com.canonical.Autopilot.Introspection
 */
class AutopilotAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Autopilot.Introspection")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.Autopilot.Introspection\">\n"
"     <method name='GetState'>"
"       <arg type='s' name='piece' direction='in' />"
"       <arg type='a(sv)' name='state' direction='out' />"
"     </method>"
"     <method name='GetVersion'>"
"       <arg type='s' name='version' direction='out' />"
"     </method>"
"  </interface>\n"
        "")
public:
    AutopilotAdaptor(QObject *parent);
    virtual ~AutopilotAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void GetState(const QString &piece, const QDBusMessage &message);
    void GetVersion(const QDBusMessage &message);
Q_SIGNALS: // SIGNALS
};

#endif
