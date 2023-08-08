#include "pantiltlimitdlg.h"
#include "ui_pantiltlimitdlg.h"
#include "constants.h"
#include "mainwindow.h"
#include <QSettings>
PanTiltLimitDlg::PanTiltLimitDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PanTiltLimitDlg)
{
    ui->setupUi(this);
    this->parent = parent;
    QString curIp = ((MainWindow*)(parent))->getCurrentCameraIp();
    if(((MainWindow*)parent)->getIsControllable())
    {
        ui->panLeftEdit->setText(QString::number(getCameraLimit(curIp, 0)));
        ui->panRightEdit->setText(QString::number(getCameraLimit(curIp, 1)));
        ui->tiltDownEdit->setText(QString::number(getCameraLimit(curIp, 2)));
        ui->tiltUpEdit->setText(QString::number(getCameraLimit(curIp, 3)));
    }
    connect(ui->applyBtn, SIGNAL(clicked(bool)), this, SLOT(applyLimits()));
    connect(ui->cancelBtn, SIGNAL(clicked(bool)), this, SLOT(cancel()));
    connect(ui->clearUpRightBtn, SIGNAL(clicked(bool)), this, SLOT(clearUpRight()));
    connect(ui->clearDownLeftBtn, SIGNAL(clicked(bool)), this, SLOT(clearDownLeft()));
}

PanTiltLimitDlg::~PanTiltLimitDlg()
{
    delete ui;
}

void PanTiltLimitDlg::applyLimits()
{
    if(((MainWindow*)parent)->getIsControllable())
    {
        int panLeftLimit = checkLimitRage(ui->panLeftEdit->text().toInt());
        int panRightLimit = checkLimitRage(ui->panRightEdit->text().toInt());
        int tiltDownLimit = checkLimitRage(ui->tiltDownEdit->text().toInt());
        int tiltUpLimit = checkLimitRage(ui->tiltUpEdit->text().toInt());
        setCameraPanTiltLimit(((MainWindow*)parent)->getCurrentCameraIp(), panLeftLimit, panRightLimit, tiltDownLimit, tiltUpLimit);
        ((MainWindow*)parent)->setPanTiltLimit(panLeftLimit,
                                panRightLimit,
                                tiltDownLimit,
                                tiltUpLimit);
    }
    close();
}

int PanTiltLimitDlg::checkLimitRage(int val)
{
    if(val < PANTILTLIMITMIN)
        val = PANTILTLIMITMIN;
    else if(val > PANTILTLIMITMAX)
        val = PANTILTLIMITMAX;
    return val;

}

void PanTiltLimitDlg::cancel()
{
    close();
}

void PanTiltLimitDlg::clearUpRight()
{
    if(((MainWindow*)parent)->getIsControllable())
    {
        ((MainWindow*)parent)->clearLimits(true);
        ((MainWindow*)parent)->showMessage("Up Right limit cleared!");
    }
}

void PanTiltLimitDlg::clearDownLeft()
{
    if(((MainWindow*)parent)->getIsControllable())
    {
        ((MainWindow*)parent)->clearLimits(false);
        ((MainWindow*)parent)->showMessage("Down Let limit cleared!");
    }
}
void PanTiltLimitDlg::setCameraPanTiltLimit(QString cameraId, int panLeft, int panRight, int tiltDown, int tiltUp)
{
    ((MainWindow*)parent)->getAppSettings()->setValue("cameralimit_0_" + cameraId, panLeft);
    ((MainWindow*)parent)->getAppSettings()->setValue("cameralimit_1_" + cameraId, panRight);
    ((MainWindow*)parent)->getAppSettings()->setValue("cameralimit_2_" + cameraId, tiltDown);
    ((MainWindow*)parent)->getAppSettings()->setValue("cameralimit_3_" + cameraId, tiltUp);
}
int PanTiltLimitDlg::getCameraLimit(QString cameraId, int side)
{
    qDebug()<<((MainWindow*)parent)->getAppSettings()->value("cameralimit_" + QString::number(side) + "_" + cameraId, PANTILTLIMITMAX).toInt();
    return ((MainWindow*)parent)->getAppSettings()->value("cameralimit_" + QString::number(side) + "_" + cameraId, PANTILTLIMITMAX).toInt();
}
