/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#ifndef TEST_DBUS_INTROSPECTION_H
#define TEST_DBUS_INTROSPECTION_H

#include <QObject>

class TestDBusIntrospection: public QObject
{
    Q_OBJECT
private slots:
    void test_simple_object();
    void test_string_props();
    void test_children();
};

class TestEmptyObject: public QObject
{
    Q_OBJECT
public:
    TestEmptyObject(QObject *p=nullptr)
    : QObject(p) {}
};

class TestObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString stringProp READ stringProp WRITE setStringProp)
public:

    QString stringProp() const { return string_prop_;}
    void setStringProp(const QString& p) { string_prop_ = p;}

private:
    QString string_prop_;
};

#endif
