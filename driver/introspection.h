/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#ifndef INTROSPECTION_H
#define INTROSPECTION_H

#include "qtnode.h"

#include <QVariantMap>

/// Introspect 'obj' and return it's properties in a QVariantMap.
QList<NodeIntrospectionData> Introspect(const QString& query_string);

/// Get a list of QtNode pointers that match the given query.
QList<QtNode::Ptr> GetNodesThatMatchQuery(QString const& query_string);

/// Return true if 't' is a type that we can marshall over DBus
QVariant PackProperty(QVariant const& prop);

/// Return a QVariantMap containing all the properties for the
/// given QObject.
QVariantMap GetNodeProperties(QObject* obj);

/// Return a QVariant containing the requested property
/// "property_name" or an invalid QVariant if the property is not
/// found.
QVariant GetNodeProperty(QObject* obj, const std::string& property_name);


#endif
