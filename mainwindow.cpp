#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QString>





MainWindow::MainWindow(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::MainWindow)
{
	QMessageBox msgBox;
	msgBox.setText("This is controller pane");
	msgBox.exec();
    isControllable = false;
    ui->setupUi(this);
    /*connecting action listeners begin*/
    connect(ui->ConnectTo , SIGNAL(pressed()) , this , SLOT(onConnectToCamera()));
    connect(ui->MoveLeft , SIGNAL(pressed()) , this , SLOT(onMoveLeft()));
    connect(ui->MoveTop , SIGNAL(pressed()) , this , SLOT(onMoveTop()));
    connect(ui->MoveRight , SIGNAL(pressed()) , this , SLOT(onMoveRight()));
    connect(ui->MoveBottom , SIGNAL(pressed()) , this , SLOT(onMoveBottom()));
    connect(ui->MoveHome , SIGNAL(pressed()) , this , SLOT(onMoveHome()));
    connect(ui->ZoomIn , SIGNAL(pressed()) , this , SLOT(onZoomIn()));
    connect(ui->ZoomOut , SIGNAL(pressed()) , this , SLOT(onZoomOut()));

    connect(ui->MoveLeft , SIGNAL(released()) , this , SLOT(onStopMoving()));
    connect(ui->MoveTop , SIGNAL(released()) , this , SLOT(onStopMoving()));
    connect(ui->MoveRight , SIGNAL(released()) , this , SLOT(onStopMoving()));
    connect(ui->MoveBottom , SIGNAL(released()) , this , SLOT(onStopMoving()));
    connect(ui->ZoomIn , SIGNAL(released()) , this , SLOT(onStopZooming()));
    connect(ui->ZoomOut , SIGNAL(released()) , this , SLOT(onStopZooming()));
    /*connecting action listeners end*/

    /*insert into panSpeed , tiltSpeed , zoomSpeed begin*/
    for(int i = 0 ; i < 24 ; i ++)
    {
        ui->panSpeed->addItem(QString::number(i + 1));
    }
    ui->panSpeed->setCurrentIndex(4);
    for(int i = 0 ; i < 20 ; i ++)
    {
        ui->tiltSpeed->addItem(QString::number(i + 1));
    }
    ui->tiltSpeed->setCurrentIndex(4);
    for(int i = 0 ; i < 7 ; i ++)
    {
        ui->zoomSpeed->addItem(QString::number(i + 1));
    }
    ui->zoomSpeed->setCurrentIndex(4);
    /*insert into panSpeed , tiltSpeed , zoomSpeed end*/

	
	socket = new QTcpSocket(this);
	connect(socket , SIGNAL(connected()) , this , SLOT(onClientConnected()));
	connect(socket , SIGNAL(disconnected()) , this , SLOT(onClientDisconnected()));
	
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectToCamera()
{
    QString ip;
    ip = ui->CameraIP->text();
    qDebug()<<ip;
    if(!isControllable)
    {
        socket->connectToHost(ip , 5678);
        if(!socket->waitForConnected(5000))
        {
            QMessageBox msgBox;
            msgBox.setText("Can not connect to the specific camera." + socket->errorString());
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Camera already connected.");
        msgBox.exec();
    }
}
void MainWindow::onClientConnected()
{
    isControllable = true;
    QMessageBox msgBox;
    msgBox.setText("Successfully connected.");
    msgBox.exec();
}
void MainWindow::onClientDisconnected()
{
    isControllable = false;
    QMessageBox msgBox;
    msgBox.setText("Camera disconnected.");
    msgBox.exec();
}

void MainWindow::onMoveLeft()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if(isControllable)
        this->moveLeftOnce(panSpeed , tiltSpeed);
}
void MainWindow::onMoveTop()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if(isControllable)
        moveTopOnce(panSpeed , tiltSpeed);
}
void MainWindow::onMoveRight()
{

    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if(isControllable)
        moveRightOnce(panSpeed , tiltSpeed);
}
void MainWindow::onMoveBottom()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if(isControllable)
        moveBottomOnce(panSpeed , tiltSpeed);
}
void MainWindow::onMoveHome()
{
    if(isControllable)
        moveHome();
}
void MainWindow::onZoomIn()
{
    int zoomSpeed = ui->zoomSpeed->currentIndex() + 1;
    if(isControllable)
        zoomInOnce(zoomSpeed);
}
void MainWindow::onZoomOut()
{
    int zoomSpeed = ui->zoomSpeed->currentIndex() + 1;
    if(isControllable)
        zoomOutOnce(zoomSpeed);
}
void MainWindow::onStopMoving()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if(isControllable)
        stopMoving(panSpeed , tiltSpeed);
}
void MainWindow::onStopZooming()
{
    int zoomSpeed = ui->zoomSpeed->currentIndex() + 1;
    if(isControllable)
        stopZooming(zoomSpeed);
}
/*remote control functions begin*/
void MainWindow::moveTopOnce(int panSpeed , int tiltSpeed)
{
    char command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x03;
    command[7] = 0x01;
    command[8] = 0xff;
    sendPacket(command , 9);
}
void MainWindow::moveLeftOnce(int panSpeed , int tiltSpeed)
{
    char command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x01;
    command[7] = 0x03;
    command[8] = 0xff;
    sendPacket(command , 9);
}
void MainWindow::moveBottomOnce(int panSpeed , int tiltSpeed)
{
    char command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x03;
    command[7] = 0x02;
    command[8] = 0xff;
    sendPacket(command , 9);
}
void MainWindow::moveRightOnce(int panSpeed , int tiltSpeed)
{
    char command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x02;
    command[7] = 0x03;
    command[8] = 0xff;
    sendPacket(command , 9);

}
void MainWindow::moveHome()
{
    char command[5];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x04;
    command[4] = 0xff;
    sendPacket(command , 5);
}
void MainWindow::zoomInOnce(int zoomSpeed)
{
    char command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x07;
    command[4] = 0x20 + zoomSpeed;
    command[5] = 0xff;
    sendPacket(command , 6);
}
void MainWindow::zoomOutOnce(int zoomSpeed)
{
    char command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x07;
    command[4] = 0x30 + zoomSpeed;
    command[5] = 0xff;
    sendPacket(command , 6);
}
void MainWindow::stopMoving(int panSpeed , int tiltSpeed)
{
    char command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x03;
    command[7] = 0x03;
    command[8] = 0xff;
    sendPacket(command , 9);
}
void MainWindow::stopZooming(int zoomSpeed)
{
    char command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x07;
    command[4] = 0x00;
    command[5] = 0xff;
    sendPacket(command , 6);
}
/*remote control functions end*/

void MainWindow::sendPacket(char *bytes , int bytesize)
{
    socket->write(bytes , bytesize);
}
