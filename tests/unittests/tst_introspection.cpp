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
    m_object->setProperty("myUInt", QVariant(quint8(5)));
    m_object->setProperty("myStringList", QVariant(QStringList() << "string1" << "string2" << "string3"));
    m_object->setProperty("myColor", QColor("red"));
    m_object->setProperty("myByteArray", QByteArray("0xDEADBEEF"));
    m_object->setProperty("myUrl", QUrl("http://www.ubuntu.com"));
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
    QTest::newRow("/") << "/" << 1 << "tst_introspection" << "Children" << QVariant(QStringList() << "QMainWindow" << "QWidgetWindow");
    QTest::newRow("//QWidget[id=6]") << "//QWidget[id=6]" << 1 << "QWidget" << "objectName" << QVariant("centralTestWidget");
    QTest::newRow("//QPushButton[id=9]") << "//QPushButton[id=9]" << 1 << "QPushButton" << "objectName" << QVariant("myButton2");
#else
    QTest::newRow("/") << "/" << 1 << "tst_introspection" << "Children" << QVariant(QStringList() << "QMainWindow");
    QTest::newRow("//QWidget[id=5]") << "//QWidget[id=5]" << 1 << "QWidget" << "objectName" << QVariant("centralTestWidget");
    QTest::newRow("//QPushButton[id=7]") << "//QPushButton[id=9]" << 1 << "QPushButton" << "objectName" << QVariant("myButton2");
#endif

    QTest::newRow("//GridLayout") << "//QGridLayout" << 1 << "QGridLayout" << "objectName" << QVariant("myTestLayout");
    QTest::newRow("//QPushButton") << "//QPushButton" << 2 << "QPushButton" << "objectName" << QVariant("myButton1");

    QTest::newRow("//QWidget/*") << "//QWidget/*" << 5 << "QGridLayout" << "objectName" << QVariant("myTestLayout");
}

void tst_Introspection::test_introspect()
{
    QFETCH(QString, xpath);
    QFETCH(int, resultCount);
    QFETCH(QString, firstResultType);
    QFETCH(QString, firstResultPropertyName);
    QFETCH(QVariant, firstResultPropertyValue);

    QList<QVariant> resultList = Introspect(xpath);

    QCOMPARE(resultList.count(), resultCount);

    if (resultCount > 0) {
        QVariant firstResult = resultList.first();
        QVariantMap firstResultProperties = firstResult.toList().last().toMap();

        QCOMPARE(firstResult.toList().first().toString(), firstResultType);
        QCOMPARE(firstResultProperties.value(firstResultPropertyName), firstResultPropertyValue);
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
    QList<QVariant> result = Introspect("//QWidgetWindow");
#else
    QList<QVariant> result = Introspect("//QMainWindow");
#endif

    QVERIFY(!result.isEmpty());
}

void tst_Introspection::test_properties_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("propertyValue");
    QTest::addColumn<bool>("fuzzyCompare");

    QTest::newRow("static property") << "objectName" << QVariant(m_object->objectName()) << false;
    QTest::newRow("dynamic property") << "dynamicTestProperty" << m_object->property("dynamicTestProperty") << false;

    QTest::newRow("int") << "width" << QVariant(m_object->width()) << false;
    QTest::newRow("uint") << "myUInt" << m_object->property("myUInt") << false;
    QTest::newRow("bool") << "visible" << QVariant(m_object->isVisible()) << false;
    QTest::newRow("double") << "windowOpacity" << QVariant(m_object->windowOpacity()) << true;

    QTest::newRow("QString") << "objectName" << QVariant(m_object->objectName()) << false;
    QTest::newRow("QStringList") << "myStringList" << m_object->property("myStringList") << false;
    QTest::newRow("QSize") << "maximumSize" << QVariant(QList<QVariant>() << m_object->maximumWidth() << m_object->maximumHeight()) << false;
    QTest::newRow("QPoint") << "pos" << QVariant(QList<QVariant>() << m_object->x() << m_object->y()) << false;
    QTest::newRow("QRect") << "geometry" << QVariant(QList<QVariant>() << m_object->geometry().x() << m_object->geometry().y() << m_object->geometry().width() << m_object->geometry().height()) << false;
    QTest::newRow("QColor") << "myColor" << QVariant(QList<QVariant>() << 255 << 0 << 0 << 255) << false;
    QTest::newRow("QByteArray") << "myByteArray" << m_object->property("myByteArray") << false;
    QTest::newRow("QUrl") << "myUrl" << m_object->property("myUrl") << false;
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

QTEST_MAIN(tst_Introspection)

#include "tst_introspection.moc"
