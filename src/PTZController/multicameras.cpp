#include <QSettings>
#include <QDebug>
#include <QStandardPaths>
#include "multicameras.h"
#include "ui_multicameras.h"
#include "constants.h"

MultiCameras::MultiCameras(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiCameras)
{
    ui->setupUi(this);
    adjustSize();
    connect(ui->multiDlgOK , SIGNAL(clicked()) , this , SLOT(onOKClicked()));
    connect(ui->multiDlgCancel , SIGNAL(clicked()) , this , SLOT(onCancelClicked()));

    settingsFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + DEFAULT_SETTINGS_PATH;
    loadSettings();
}

MultiCameras::~MultiCameras()
{
    delete ui;
}
/***************************************SLOTS*******************************************************************/
void MultiCameras::onOKClicked()
{
    saveSettings();
    this->close();
}
void MultiCameras::onCancelClicked()
{
    this->close();
}
/***************************************SLOTS*******************************************************************/
void MultiCameras::saveSettings()
{
//    qDebug()<<"save settings";
    QSettings settings(settingsFile , QSettings::IniFormat);
    if(settings.isWritable())
        qDebug()<<"it is writable.";
    settings.setValue(DEFAULT_CAMERA1_IP_KEY , ui->camIP1->text());
    settings.setValue(DEFAULT_CAMERA2_IP_KEY , ui->camIP2->text());
    settings.setValue(DEFAULT_CAMERA3_IP_KEY , ui->camIP3->text());
    settings.setValue(DEFAULT_CAMERA4_IP_KEY , ui->camIP4->text());
    settings.setValue(DEFAULT_CAMERA5_IP_KEY , ui->camIP5->text());
    settings.setValue(DEFAULT_CAMERA6_IP_KEY , ui->camIP6->text());
    settings.setValue(DEFAULT_CAMERA7_IP_KEY , ui->camIP7->text());
    settings.setValue(DEFAULT_CAMERA8_IP_KEY , ui->camIP8->text());

    settings.setValue(DEFAULT_CAMERA1_NAME_KEY , ui->camName1->text().left(10));
    settings.setValue(DEFAULT_CAMERA2_NAME_KEY , ui->camName2->text().left(10));
    settings.setValue(DEFAULT_CAMERA3_NAME_KEY , ui->camName3->text().left(10));
    settings.setValue(DEFAULT_CAMERA4_NAME_KEY , ui->camName4->text().left(10));
    settings.setValue(DEFAULT_CAMERA5_NAME_KEY , ui->camName5->text().left(10));
    settings.setValue(DEFAULT_CAMERA6_NAME_KEY , ui->camName6->text().left(10));
    settings.setValue(DEFAULT_CAMERA7_NAME_KEY , ui->camName7->text().left(10));
    settings.setValue(DEFAULT_CAMERA8_NAME_KEY , ui->camName8->text().left(10));

}
void MultiCameras::loadSettings()
{
    QSettings settings(settingsFile , QSettings::IniFormat);
    ui->camIP1->setText(settings.value(DEFAULT_CAMERA1_IP_KEY , DEFAULT_CAMERA_IP).toString());
    ui->camIP2->setText(settings.value(DEFAULT_CAMERA2_IP_KEY , DEFAULT_CAMERA_IP).toString());
    ui->camIP3->setText(settings.value(DEFAULT_CAMERA3_IP_KEY , DEFAULT_CAMERA_IP).toString());
    ui->camIP4->setText(settings.value(DEFAULT_CAMERA4_IP_KEY , DEFAULT_CAMERA_IP).toString());
    ui->camIP5->setText(settings.value(DEFAULT_CAMERA5_IP_KEY , DEFAULT_CAMERA_IP).toString());
    ui->camIP6->setText(settings.value(DEFAULT_CAMERA6_IP_KEY , DEFAULT_CAMERA_IP).toString());
    ui->camIP7->setText(settings.value(DEFAULT_CAMERA7_IP_KEY , DEFAULT_CAMERA_IP).toString());
    ui->camIP8->setText(settings.value(DEFAULT_CAMERA8_IP_KEY , DEFAULT_CAMERA_IP).toString());

    ui->camName1->setText(settings.value(DEFAULT_CAMERA1_NAME_KEY , DEFAULT_CAMERA1_NAME).toString());
    ui->camName2->setText(settings.value(DEFAULT_CAMERA2_NAME_KEY , DEFAULT_CAMERA2_NAME).toString());
    ui->camName3->setText(settings.value(DEFAULT_CAMERA3_NAME_KEY , DEFAULT_CAMERA3_NAME).toString());
    ui->camName4->setText(settings.value(DEFAULT_CAMERA4_NAME_KEY , DEFAULT_CAMERA4_NAME).toString());
    ui->camName5->setText(settings.value(DEFAULT_CAMERA5_NAME_KEY , DEFAULT_CAMERA5_NAME).toString());
    ui->camName6->setText(settings.value(DEFAULT_CAMERA6_NAME_KEY , DEFAULT_CAMERA6_NAME).toString());
    ui->camName7->setText(settings.value(DEFAULT_CAMERA7_NAME_KEY , DEFAULT_CAMERA7_NAME).toString());
    ui->camName8->setText(settings.value(DEFAULT_CAMERA8_NAME_KEY , DEFAULT_CAMERA8_NAME).toString());
}
