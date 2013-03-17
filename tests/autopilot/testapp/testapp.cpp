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
