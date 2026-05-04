#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "udpserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void setInitialConfigFile(const QString &path);
//    void slotReadSettings();
//    void slotSaveSettings();
//    void slotLoadSettings();
    ~MainWindow();

//    QListView messageList;    

private slots:
    void displayMessage(QString msg);
    void on_pushMessage_clicked();

private:
    Ui::MainWindow *ui;

    Server *server;
    udpServer *udp_Server;
};
#endif // MAINWINDOW_H
