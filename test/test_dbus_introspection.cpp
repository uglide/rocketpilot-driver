/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "test_dbus_introspection.h"
#include "introspection.h"

#include <QTest>
#include <QDebug>
#include <QDBusArgument>


/// A simple QObject with no properties must return an empty map.
void TestDBusIntrospection::test_simple_object()
{
    TestEmptyObject obj;
    QVariantMap state = Introspect(&obj);
    QVERIFY(state.count() == 0);
}

void TestDBusIntrospection::test_string_props()
{
    TestObject obj;
    QVariantMap state = Introspect(&obj);
    QVERIFY(state.count() >= 1);
    QVERIFY(state.count("stringProp") == 1);
    QVERIFY(state.value("stringProp") == "");
    obj.setStringProp("FooBar");
    state = Introspect(&obj);
    QVERIFY(state.count("stringProp") == 1);
    QVERIFY(state.value("stringProp") == "FooBar");
}

void TestDBusIntrospection::test_children()
{
    QVariantMap state;
    TestObject obj;
    {
        TestEmptyObject obj2(&obj);
        state = Introspect(&obj);
    }
    qDebug() << state;
    QVERIFY(state.count("Children") == 1);


}
