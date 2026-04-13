#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Server server;

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "projectServer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
