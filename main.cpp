#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QDebug>
#include "server.h"

#include <iostream>

// void parseComandLine(QApplication &app){
// //    QString dirPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");
//     QString dirPath;
//     QCommandLineParser parser;

//     parser.setApplicationDescription("Моя программа сервер");
// //    parser.addPositionalArgument("files", "Список файлов: ", "[serverInit.ini, infoJsonData.json, clientYamlFile.yaml]");
// //    parser.addPositionalArgument("files", "Список файлов: ", "[files...]");
//     parser.addHelpOption();
//     parser.addVersionOption();

//     QCommandLineOption dirOptional(
//                 {"d", "dir"},
//                 "Путь к папке для поиска  конфигурационных файлов",
//                 "directory");

//     QCommandLineOption configOption(
//                 {"c", "config"},
//                 "Путь к файлу конфигурации",
//                 "path"
//                 );

//     parser.addOption(dirOptional);
//     parser.addOption(configOption);
//     parser.process(app);

//     if (parser.isSet(configOption)) {
//         QString configPath = parser.value(configOption);
//         qDebug() << "Вывод  configPah с командной строки: " << configPath;
//     }

//     if (parser.isSet(dirOptional)) {
//         dirPath = parser.value(dirOptional);

//         if (dirPath.isEmpty()) {
//             qCritical() << "Ошибка: использован флаг -d, но не указан путь к папке!";
//             parser.showHelp(1);
//         }

//         QDir directory(dirPath);
//         if (!directory.exists()) {
//             qCritical() << "Ошибка: такого пути не существует! " << dirPath;
//             return;
//         }
//     }else{
//         dirPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");
//     }

//     qDebug() << "Путь к файлам: " << dirPath;
// }

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);

//     // QFile qss(":/styles.qss");
//     // if (qss.open(QFile::ReadOnly)) {
//     //     a.setStyleSheet(qss.readAll());
//     //     qss.close();
//     // }else{
//     //     qDebug() << "ERROR: Не удалось открыть файл стилей для чтения";
//     // }

//     parseComandLine(a);

//     QTranslator translator;
//     const QStringList uiLanguages = QLocale::system().uiLanguages();
//     for (const QString &locale : uiLanguages) {
//         const QString baseName = "projectServer_" + QLocale(locale).name();
//         if (translator.load(":/i18n/" + baseName)) {
//             a.installTranslator(&translator);
//             break;
//         }
//     }
//     MainWindow w;
//     w.show();
//     return a.exec();
// }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile qss(":/styles.qss");
    if (qss.open(QFile::ReadOnly)) {
        a.setStyleSheet(qss.readAll());
        qss.close();
    } else {
        qDebug() << "ERROR: Не удалось открыть файл стилей";
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("Мой сервер");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption dirOptional({
                                    {"d", "dir"},
                                    "Путь к папке конфигураций",
                                    "directory"
    });
    QCommandLineOption configOption({
                                     {"c", "config"},
                                     "Путь к файлу конфигурации",
                                     "path"
    });
    QCommandLineOption consoleOnlyOption(
        "console-only",
        "Выполнить задачу в консоли и выйти"
        );

    QCommandLineOption consoleListFileOption({
        {"l", "list"},
        "Список файлов в папке"
    });

    parser.addOption(dirOptional);
    parser.addOption(configOption);
    parser.addOption(consoleOnlyOption);
    parser.addOption(consoleListFileOption);
    parser.process(a);

    // QString dirPath;
    // if (parser.isSet(dirOptional)) {
    //     dirPath = parser.value(dirOptional);
    //     if (dirPath.isEmpty()) {
    //         qCritical() << "Ошибка: ключ -d без значения";
    //         parser.showHelp(1);
    //     }
    //     QDir directory(dirPath);
    //     if (!directory.exists()) {
    //         qCritical() << "Ошибка: пути не существует!" << dirPath;
    //         return 1;
    //     }
    // } else {
    //     dirPath = QString("%1/%2").arg(a.applicationDirPath(), "data");
    // }
    // qDebug() << "Путь к файлам:" << dirPath;

    QString configPath;

    if (parser.isSet(configOption)) {
        configPath = parser.value(configOption);
        qDebug() << "Путь к файлам:" << configPath;
    } else {

        configPath = QString("%1/%2").arg(a.applicationDirPath(), "data/serverInit.ini");
    }

    if (parser.isSet(consoleListFileOption)) {
        std::cout << "Выводим список всех файлов:\n";

        QDir dir(configPath);

        QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);

        foreach (QString file, files) {
            std::cout << file.toStdString() << std::endl;
        }
    }

    if (parser.isSet(consoleOnlyOption)) {
        std::cout << "Работаем в консольном режиме.\n";
        return 0;
    }

    QFile qssFile(":/styles.qss");
    if (qssFile.open(QFile::ReadOnly)) {
        a.setStyleSheet(qssFile.readAll());
        qssFile.close();
    }else{
        qDebug() << "ERROR: Не удалось открыть файл стилей для чтения";
    }

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
    w.setInitialConfigFile(configPath);
    w.show();
    return a.exec();
}
