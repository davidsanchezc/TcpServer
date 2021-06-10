#include "tcpserver.h"
#include "ui_tcpserver.h"

#include <QNetworkInterface>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QObject>
#include <QAbstractSocket>



using namespace std;
TcpServer::TcpServer(QWidget *parent): QDialog(parent), ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    initServer();
    //Random Message
    for(int i = 1;i<=20;i++){
        randomMessages << "Mensaje aleatorio" + QString::number(i);
    }
    //Connects
    connect(ui->btn_close,&QAbstractButton::clicked,this,&QWidget::close);
    //Connect for the server socket
    connect(qTcpServer,&QTcpServer::newConnection,this,&TcpServer::sendRandomMessage);
}

TcpServer::~TcpServer()
{
    delete ui;
}
/**
 * @brief TcpServer::initServer
 */
void TcpServer::initServer()
{
    qInfo("Init Server");
    qTcpServer = new QTcpServer(this);//incialización
    if (!qTcpServer->listen()){
        qInfo()<<"Server error: "+ qTcpServer->errorString();
        return;
    }
    QString port = QString::number(qTcpServer->serverPort(),10);
    QString ipAddress;
    QList<QHostAddress> ipAddressList = QNetworkInterface::allAddresses();
    for (int i=0;ipAddressList.size(); i++ ) {
        if(ipAddressList.at(i)!=QHostAddress::LocalHost && ipAddressList.at(i).toIPv4Address()){
            ipAddress = ipAddressList.at(i).toString();
            qInfo()<<"ip: "+ ipAddress;
            break;
        }
    }
    if(ipAddressList.isEmpty()){
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    QString info = "ip: "+ ipAddress + ", puerto: "+ port;
    ui->lbl_info->setText(info);

}

void TcpServer::sendRandomMessage()
{
    qInfo("Send Random Message");
    QByteArray qByteArray;
    QDataStream qDataStream(&qByteArray,QIODevice::WriteOnly);
    QString randomMessage = randomMessages[QRandomGenerator::global()->bounded(randomMessages.size())];
    qDataStream << randomMessage;
    QTcpSocket *clientConnection = qTcpServer->nextPendingConnection();
    connect(clientConnection,&QAbstractSocket::disconnect, clientConnection,&QObject::deleteLater);
    clientConnection->write(qByteArray);
    clientConnection->disconnectFromHost();
    ui->lbl_message->setText(randomMessage);
}
