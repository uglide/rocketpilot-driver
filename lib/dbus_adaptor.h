/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/


#ifndef DBUS_ADAPTOR_H_1337917721
#define DBUS_ADAPTOR_H_1337917721

#include <QObject>
#include <QtDBus>

class QString;


/*
 * Adaptor class for interface com.canonical.Unity.Debug.Introspection
 */
class AutopilotAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity.Debug.Introspection")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.Unity.Debug.Introspection\">\n"
"    <method name=\"GetState\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"piece\"/>\n"
//"      <arg direction=\"out\" type=\"aa{sv}\" name=\"state\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"state\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    AutopilotAdaptor(QObject *parent);
    virtual ~AutopilotAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void GetState(const QString &piece, const QDBusMessage &message);
Q_SIGNALS: // SIGNALS
};

#endif
