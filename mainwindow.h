#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QTcpSocket>


namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:

    QTcpSocket *socket;
    bool isControllable = false;
    void moveTopOnce(int panSpeed , int tiltSpeed);
    void moveLeftOnce(int panSpeed , int tiltSpeed);
    void moveBottomOnce(int panSpeed , int tiltSpeed);
    void moveRightOnce(int panSpeed , int tiltSpeed);
    void stopMoving(int panSpeed , int tiltSpeed);
    void stopZooming(int zoomSpeed);
    void moveHome();
    void zoomInOnce(int zoomSpeed);
    void zoomOutOnce(int zoomSpeed);
    void sendPacket(char *bytes , int bytesize);

private slots:

    void onConnectToCamera();
    void onClientConnected();
    void onClientDisconnected();

    void onMoveTop();

    void onMoveLeft();

    void onMoveBottom();

    void onMoveRight();

    void onMoveHome();

    void onZoomIn();

    void onZoomOut();

    void onStopMoving();

    void onStopZooming();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
