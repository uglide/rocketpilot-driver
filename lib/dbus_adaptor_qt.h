#ifndef DBUS_ADAPTOR_QT_H
#define DBUS_ADAPTOR_QT_H

#include <QObject>
#include <QtDBus>

class AutopilotQtSpecificAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Autopilot.Qt")
    Q_CLASSINFO("D-Bus Introspection", ""
                "  <interface name=\"com.canonical.Autopilot.Qt\">\n"
                "     <method name='RegisterSignalInterest'>"
                "       <arg type='i' name='object_id' direction='in' />"
                "       <arg type='s' name='signal_name' direction='in' />"
                "     </method>"
                "     <method name='GetSignalEmissions'>"
                "       <arg type='i' name='object_id' direction='in' />"
                "       <arg type='s' name='signal_name' direction='in' />"
                "       <arg type='i' name='sigs' direction='out' />"
                "    </method>"
                "     <method name='ListSignals'>"
                "       <arg type='i' name='object_id' direction='in' />"
                "       <arg type='as' name='signals' direction='out' />"
                "     </method>"
                ""
                "    <method name='ListMethods'>"
                "      <arg type='i' name='object_id' direction='in' />"
                "       <arg type='as' name='methods' direction='out' />"
                "    </method>"
                ""
                "  </interface>\n"
        "")
public:
    AutopilotQtSpecificAdaptor(QObject *parent = 0);
    
signals:
    
public slots:
    void RegisterSignalInterest(int object_id, QString signal_name);
    void GetSignalEmissions(int object_id, QString signal_name, const QDBusMessage& message);
    void ListSignals(int object_id, const QDBusMessage& message);

    void ListMethods(int object_id, const QDBusMessage& message);
    
};

#endif // DBUS_ADAPTOR_QT_H
