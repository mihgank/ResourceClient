#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket;
    socket->connectToHost("127.0.0.1", 8080);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::ReadMessage);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SendMessage(QJsonObject message){
    if (message.isEmpty())
           return;
    QDataStream clientStream(socket);
    clientStream.setVersion(QDataStream::Qt_6_4);
    qDebug() << "sending object:" << message;
    clientStream << QJsonDocument(message).toJson();
}

void MainWindow::ReadMessage(){
        QByteArray jsonData;
        QDataStream socketStream(socket);
        socketStream.setVersion(QDataStream::Qt_6_4);
        while(true) {
            socketStream.startTransaction();
            socketStream >> jsonData;
            if (socketStream.commitTransaction()) {
                QJsonParseError parseError;
                const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
                if (parseError.error == QJsonParseError::NoError) {
                    if (jsonDoc.isObject()){
                        qDebug() << "jsonReceived" << jsonDoc.object(); // parse the JSON
                        ui->textBrowser->append(jsonDoc.toJson(QJsonDocument::Compact));
                    }
                }
            } else {
                // the read failed, the socket goes automatically back to the state it was in before the transaction started
                // we just exit the loop and wait for more data to become available
                break;
            }
        }
}



void MainWindow::on_requestButton_clicked()
{
    QJsonObject message;
    message[QStringLiteral("username")] = ui->usernameEdit->text();
    message[QStringLiteral("time")] = ui->timeEdit->text();
    //TODO: fix to bool states
    message[QStringLiteral("resources")] = QString::number(ui->resurceBox_1->isChecked())
            + QString::number(ui->resurceBox_2->isChecked())
            + QString::number(ui->resurceBox_3->isChecked())
            + QString::number(ui->resurceBox_4->isChecked());

    SendMessage(message);

    ui->timeEdit->clear();
    ui->resurceBox_1->clearMask();
    ui->resurceBox_2->clearMask();
    ui->resurceBox_3->clearMask();
    ui->resurceBox_4->clearMask();

}


