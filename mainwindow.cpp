#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Сервер");

    server = new Server();

    ui->listMessage->setLineWrapMode(QTextEdit::NoWrap);
    ui->listMessage->setReadOnly(true);

    connect(server, SIGNAL(logMessage(QString)), this, SLOT(displayMessage(QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayMessage(QString msg){
//    ui->listMessage->append("Успешно подключен клиент");
    ui->listMessage->append(msg);

}


void MainWindow::on_pushMessage_clicked()
{
    if (ui->textFieldMessage->text().isEmpty()) {
        return;
    }

    QString message = "(Server)-> ";
    message.append(ui->textFieldMessage->text().trimmed());
    ui->listMessage->append(message);
    server->sendMessage(message);
    message.clear();
    ui->textFieldMessage->clear();
}

void MainWindow::setInitialConfigFile(const QString &path) {
    server->loadConfigFile(path);
}

//void MainWindow::slotLoadSettings(){
//    configIniFile->setValue("port/open", "56789");
//}

//void MainWindow::slotReadSettings(){
//    QString port = configIniFile->value("/port/open").toString();
//    qDebug() << "Вывод порта из инит файла: " << port;
//}

