#include <QApplication>

#include <QQuickView>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList args = QApplication::arguments();
    QString sourceFile = args.last();

    int appNameIndex = args.indexOf("--appname");
    if(appNameIndex > 0 && args.count() >= appNameIndex) {
        app.setApplicationName(args.at(appNameIndex+1));
    }

    QQuickView view;
    view.setSource(QUrl(sourceFile));

    view.show();
    app.exec();
}
