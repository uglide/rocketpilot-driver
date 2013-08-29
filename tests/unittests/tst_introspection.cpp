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

#include <QStringList>
#include <QtTest>
#include <QMainWindow>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>

#include "introspection.h"
#include "qtnode.h"

QVariant IntrospectNode(QObject* obj);

class tst_Introspection : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_introspect_data();
    void test_introspect();

    void test_application_names_data();
    void test_application_names();

    void test_properties_data();
    void test_properties();

    void test_property_matching();

private:
    QMainWindow *m_object;
};


void tst_Introspection::initTestCase()
{
    QApplication::setApplicationName("tst_introspection");

    m_object = new QMainWindow();
    QWidget *centralWidget = new QWidget();
    centralWidget->setObjectName("centralTestWidget");
    m_object->setCentralWidget(centralWidget);

    QGridLayout *layout = new QGridLayout();
    layout->setObjectName("myTestLayout");
    centralWidget->setLayout(layout);

    QPushButton *button = new QPushButton("MyButton1");
    button->setObjectName("myButton1");
    layout->addWidget(button);

    button = new QPushButton("MyButton2");
    button->setObjectName("myButton2");
    layout->addWidget(button);

    m_object->setObjectName("testWindow");
    m_object->setProperty("dynamicTestProperty", "testValue");
    m_object->setProperty("dynamicStringProperty", QString("testValue"));
    m_object->setProperty("myUInt", QVariant(quint8(5)));
    m_object->setProperty("myStringList", QVariant(QStringList() << "string1" << "string2" << "string3"));
    m_object->setProperty("myColor", QColor("red"));
    m_object->setProperty("myByteArray", QByteArray("0xDEADBEEF"));
    m_object->setProperty("myUrl", QUrl("http://www.ubuntu.com"));
    m_object->setProperty("myDateTime", QDateTime::currentDateTime());
    m_object->setProperty("myDate", QDateTime::currentDateTime().date());
    m_object->setProperty("myTime", QTime::currentTime());
    m_object->setMaximumSize(1234, 4321);
    m_object->resize(123, 321);
    m_object->move(333, 444);
    m_object->setVisible(false);
    m_object->setWindowOpacity(0.12345);

    m_object->show();
}

void tst_Introspection::cleanupTestCase()
{
    m_object->close();
    delete m_object;
}

void tst_Introspection::test_introspect_data()
{
    // some query
    QTest::addColumn<QString>("xpath");

    // number of expected results
    QTest::addColumn<int>("resultCount");

    // first result object type. Empty string if 0 results expected
    QTest::addColumn<QString>("firstResultType");

    // Choose a property from the first result object to be compared, empty QString/QVariant if 0 results expected
    QTest::addColumn<QString>("firstResultPropertyName");
    QTest::addColumn<QVariant>("firstResultPropertyValue");

#ifdef QT5_SUPPORT
    QTest::newRow("/")
        << "/"
        << 1
        << "/tst_introspection"
        << "Children"
        << QVariant(
            QVariantList()
                << 0
                << QVariant(
                    QStringList()
                        << "QMainWindow"
                        << "QWidgetWindow"
                    )
            );

    QTest::newRow("//QWidget[id=6]")
        << "//QWidget[id=6]"
        << 1
        << "/tst_introspection/QMainWindow/QWidget"
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << "centralTestWidget"
            );

    QTest::newRow("//QPushButton[id=9]")
        << "//QPushButton[id=9]"
        << 1
        << "/tst_introspection/QMainWindow/QWidget/QPushButton"
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << "myButton2"
            );
#else
    QTest::newRow("/")
        << "/"
        << 1
        << "/tst_introspection"
        << "Children"
        << QVariant(
            QVariantList()
                << 0
                << "QMainWindow"
            );

    QTest::newRow("//QWidget[id=5]")
        << "//QWidget[id=5]"
        << 1
        << "/tst_introspection/QMainWindow/QWidget"
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << "centralTestWidget"
            );

    // Depending on the environment, Qt4 could add a second QWidget at position 6. That moves other items down by one.
    if (Introspect("//QWidget[id=6]").count() > 0)
    {
        QTest::newRow("//QPushButton[id=9]")
            << "//QPushButton[id=9]"
            << 1
            << "/tst_introspection/QMainWindow/QWidget/QPushButton"
            << "objectName"
            << QVariant(
                QVariantList()
                    << 0
                    << "myButton2"
                );
    }
    else
    {
        QTest::newRow("//QPushButton[id=8]")
            << "//QPushButton[id=8]"
            << 1
            << "/tst_introspection/QMainWindow/QWidget/QPushButton"
            << "objectName"
            << QVariant(
                QVariantList()
                    << 0
                    << "myButton2"
                );
    }
#endif

    QTest::newRow("//GridLayout")
        << "//QGridLayout"
        << 1
        << "/tst_introspection/QMainWindow/QWidget/QGridLayout"
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << "myTestLayout"
            );

    QTest::newRow("//QPushButton")
        << "//QPushButton"
        << 2
        << "/tst_introspection/QMainWindow/QWidget/QPushButton"
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << "myButton1"
            );

    QTest::newRow("//QWidget/*")
        << "//QWidget/*"
        << 5
        << "/tst_introspection/QMainWindow/QWidget/QGridLayout"
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << "myTestLayout"
            );

    QTest::newRow("broken query")
        << "broken query"
        << 0
        << QString()
        << QString()
        << QVariant();
}

void tst_Introspection::test_introspect()
{
    QFETCH(QString, xpath);
    QFETCH(int, resultCount);
    QFETCH(QString, firstResultType);
    QFETCH(QString, firstResultPropertyName);
    QFETCH(QVariant, firstResultPropertyValue);

    QList<NodeIntrospectionData> resultList = Introspect(xpath);

    QCOMPARE(resultList.count(), resultCount);

    if (resultCount > 0) {
        NodeIntrospectionData first_object = resultList.first();

        QCOMPARE(first_object.object_path, firstResultType);
        QCOMPARE(first_object.state.value(firstResultPropertyName), firstResultPropertyValue);
    }
}

void tst_Introspection::test_application_names_data()
{
    QTest::addColumn<QString>("app_name");

    QTest::newRow("Unset") << "untitled1";
    QTest::newRow("Tech") << "autopilot-qt";
    QTest::newRow("Userfriendly") << "Autopilot Qt Driver";
    QTest::newRow("Fqdn name") << "com.canonical.Autopilot.Qt";
}

void tst_Introspection::test_application_names()
{
    QFETCH(QString, app_name);
    qApp->setApplicationName(app_name);

#ifdef QT5_SUPPORT
    QList<NodeIntrospectionData> result = Introspect("//QWidgetWindow");
#else
    QList<NodeIntrospectionData> result = Introspect("//QMainWindow");
#endif

    QVERIFY(!result.isEmpty());
}

void tst_Introspection::test_properties_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("propertyValue");
    QTest::addColumn<bool>("fuzzyCompare");

    QTest::newRow("static property")
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << m_object->objectName()
            )
        << false;

    QTest::newRow("dynamic property")
        << "dynamicTestProperty"
        << QVariant(
            QVariantList()
                << 0
                << m_object->property("dynamicTestProperty")
            )
        << false;

    QTest::newRow("int")
    << "width"
    << QVariant(
        QVariantList()
            << 0
            << m_object->width()
        )
        << false;

    QTest::newRow("uint")
        << "myUInt"
        << QVariant(
            QVariantList()
                << 0
                << m_object->property("myUInt")
            )
        << false;

    QTest::newRow("bool")
        << "visible"
        << QVariant(
            QVariantList()
                << 0
                << m_object->isVisible()
            )
        << false;

    QTest::newRow("double")
        << "windowOpacity"
        << QVariant(
            QVariantList()
                << 0
                << m_object->windowOpacity()
            )
        << true;

    QTest::newRow("QString")
        << "objectName"
        << QVariant(
            QVariantList()
                << 0
                << m_object->objectName()
            )
        << false;

    QTest::newRow("QStringList")
        << "myStringList"
        << QVariant(
            QVariantList()
                << 0
                << m_object->property("myStringList")
            )
        << false;

    QTest::newRow("QSize")
        << "maximumSize"
        << QVariant(
            QVariantList()
                << 3
                << m_object->maximumWidth()
                << m_object->maximumHeight()
            )
        << false;

    QTest::newRow("QPoint")
        << "pos"
        << QVariant(
            QVariantList()
                << 2
                << m_object->x()
                << m_object->y()
            )
        << false;

    QTest::newRow("QRect")
        << "geometry"
        << QVariant(
            QVariantList()
                << 1
                << m_object->geometry().x()
                << m_object->geometry().y()
                << m_object->geometry().width()
                << m_object->geometry().height()
            )
        << false;

    QTest::newRow("QColor")
        << "myColor"
        << QVariant(
            QVariantList()
                << 4
                << qvariant_cast<QColor>(m_object->property("myColor")).red()
                << qvariant_cast<QColor>(m_object->property("myColor")).green()
                << qvariant_cast<QColor>(m_object->property("myColor")).blue()
                << qvariant_cast<QColor>(m_object->property("myColor")).alpha()
            )
        << false;

    QTest::newRow("QByteArray")
        << "myByteArray"
        << QVariant(
            QVariantList()
                << 0
                << m_object->property("myByteArray")
            )
        << false;

    QTest::newRow("QUrl")
        << "myUrl"
        << QVariant(
            QVariantList()
                << 0
                << m_object->property("myUrl")
            )
        << false;

    QTest::newRow("QDateTime")
        << "myDateTime"
        << QVariant(
            QVariantList()
                << 5
                << m_object->property("myDateTime").toDateTime().toTime_t()
            )
        << false;

    QTest::newRow("QDate")
        << "myDate"
        << QVariant(
            QVariantList()
                << 5
                << m_object->property("myDate").toDateTime().toTime_t()
            )
        << false;

    QTest::newRow("QTime")
        << "myTime"
        << QVariant(
            QVariantList()
                << 6
                << m_object->property("myTime").toTime().hour()
                << m_object->property("myTime").toTime().minute()
                << m_object->property("myTime").toTime().second()
                << m_object->property("myTime").toTime().msec()
            )
        << false;
}

void tst_Introspection::test_properties()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, propertyValue);
    QFETCH(bool, fuzzyCompare);

    QVariant result = IntrospectNode(m_object);

    QCOMPARE(result.toList().count(), 2);
    QVariantMap properties = result.toList().at(1).toMap();

    if (fuzzyCompare) {
        qFuzzyCompare(properties.value(propertyName).toDouble(), propertyValue.toDouble());
    } else {
        QCOMPARE(properties.value(propertyName), propertyValue);
    }
}

void tst_Introspection::test_property_matching()
{
    QtNode n(m_object, "");

    QVERIFY(n.MatchStringProperty("dynamicStringProperty", "testValue") == true);
    QVERIFY(n.MatchStringProperty("dynamicTestProperty", "testValue") == true);
    QVERIFY(n.MatchIntegerProperty("myUInt", 5) == true);
    QVERIFY(n.MatchBooleanProperty("visible", true) == true);
}

QTEST_MAIN(tst_Introspection)

#include "tst_introspection.moc"
