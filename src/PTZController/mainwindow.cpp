
#include <QDialog>
#include <QMessageBox>
#include <QString>
#include <QTcpSocket>
#include <QPixmap>
#include <QTimer>
#include <QScreen>
//newly added 07/19
#include <QInputDialog>

#include "mainwindow.h"
#include "aboutus.h"
//newly added 07/19
#include "xboxhelp.h"
#include "hotkeyhelp.h"
#include "pantiltlimitdlg.h"

#include "multicameras.h"
#include "joystickdlg.h"

#include "ui_mainwindow.h"


//QLabel *panStatus, *tiltStatus, *zoomStatus , *focusStatus , *shutterStatus, *irisStatus , *brightStatus , *onlineStatus , *hueStatus , *luminanceStatus , *contrastStatus /*,*rGainStatus , *bGainStatus*/;
QLabel *onlineStatus, *devNameLabel;
int getStatusStep = 0;//this variable is used only when get camera info every 1 sec...
//bulk query do not need this variable

QString currentCamIp;

QMap<QNetworkReply*, QFile*> downloadDestination;

int centerW , centerH;
int initialWidth , initialHeight;

QString aeModes[5] = {"Auto" , "Manual" , "SAE" , "AAE" , "Brightness"};
QString shutters[17] = {"1/30" , "1/60" , "1/90" , "1/100" , "1/125" , "1/180" , "1/250" , "1/350" , "1/500" , "1/725" , "1/1000" , "1/1500" , "1/2000" , "1/3000" , "1/4000" , "1/6000" , "1/10000"};
QString irises[13] = {"Close" , "F11.0" , "F9.6" , "F8.0" , "F6.8" , "F5.6" , "F4.8" , "F4.0" , "F3.4" , "F2.8" , "F2.4" , "F2.0" , "F1.8"};
QString wbModes[5] = {"Auto" , "Indoor" , "Outdoor" , "OnePush" , "Manual"};

//indicates whether new camera started or not
int startFlag = 2;//0:just connected 1:first fully received 2:already set the first value or disconnected


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    /*set setting file path begin*/

    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).exists())
    {
        qDebug()<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        bool result = QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        qDebug()<<"AppDataLocation"<<result;
    }
    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ptzoptics-controller").exists())
    {
        bool result = QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ptzoptics-controller");
        qDebug()<<"ptzoptics-controller"<<result;
    }
    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ptzoptics-controller/downloads").exists())
    {
        bool result = QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ptzoptics-controller/downloads");
        qDebug()<<"downloads"<<result;
    }
    settingsFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + DEFAULT_SETTINGS_PATH;
    settings = new QSettings(settingsFile, QSettings::IniFormat);
    /*set setting file path end*/

    joystick = new JoystickWrapper(this);
    connect(joystick , SIGNAL(backBtnTriggered()) , this , SLOT(onOsdBackClicked()));
    connect(joystick , SIGNAL(enterBtnTriggered()) , this , SLOT(onOsdEnterClicked()));
    connect(joystick , SIGNAL(osdMenuBtnTriggered()) , this , SLOT(onOsdMenuClicked()));
    connect(joystick , SIGNAL(moveLeftBtnTriggered()) , this , SLOT(onMoveLeft()));
    connect(joystick , SIGNAL(moveRightBtnTriggered()) , this , SLOT(onMoveRight()));
    connect(joystick , SIGNAL(moveTopBtnTriggered()) , this , SLOT(onMoveTop()));
    connect(joystick , SIGNAL(moveBottomBtnTriggered()) , this , SLOT(onMoveBottom()));
    connect(joystick , SIGNAL(moveTopLeftBtnTriggered()) , this , SLOT(onAngleUpLeft()));
    connect(joystick , SIGNAL(moveTopRightBtnTriggered()) , this , SLOT(onAngleUpRight()));
    connect(joystick , SIGNAL(moveBottomRightBtnTriggered()) , this , SLOT(onAngleDownRight()));
    connect(joystick , SIGNAL(moveBottomLeftBtnTriggered()) , this , SLOT(onAngleDownLeft()));
    connect(joystick , SIGNAL(moveStop()) , this , SLOT(onStopMoving()));
    connect(joystick , SIGNAL(moveHome()) , this , SLOT(onMoveHome()));
    connect(joystick , SIGNAL(zoomInBtnTriggered()) , this , SLOT(onZoomIn()));
    connect(joystick , SIGNAL(zoomOutBtnTriggered()) , this , SLOT(onZoomOut()));
    connect(joystick , SIGNAL(zoomStop()) , this , SLOT(onStopZooming()));
    connect(joystick , SIGNAL(focusInBtnTriggered()) , this , SLOT(onFocusIn()));
    connect(joystick , SIGNAL(focusOutBtnTriggered()) , this , SLOT(onFocusOut()));
    connect(joystick , SIGNAL(focusStop()) , this , SLOT(onStopFocusing()));
    connect(joystick , SIGNAL(luminanceChangeTriggered(bool)) , this , SLOT(onLuminanceChangeTriggered(bool)));
    connect(joystick , SIGNAL(contrastChangeTriggered(bool)) , this , SLOT(onContrastChangeTriggered(bool)));
    connect(joystick , SIGNAL(hueChangeTriggered(bool)) , this , SLOT(onHueChangeTriggered(bool)));
    connect(joystick , SIGNAL(focusStateChange()) , this , SLOT(autoFocusClick()));
    connect(joystick , SIGNAL(callPreset(int)) , this , SLOT(onCallPreset(int)));
    connect(joystick , SIGNAL(switchCamera(int)) , this , SLOT(onSwitchCamera(int)));



    /*connecting action listeners begin*/
    //connect(ui->ConnectTo, SIGNAL(pressed()), this, SLOT(onConnectToCamera()));
    connect(ui->MoveHome, SIGNAL(pressed()), this, SLOT(onMoveHome()));

    connect(ui->MoveLeft, SIGNAL(pressed()), this, SLOT(onMoveLeft()));
    connect(ui->MoveTop, SIGNAL(pressed()), this, SLOT(onMoveTop()));
    connect(ui->MoveRight, SIGNAL(pressed()), this, SLOT(onMoveRight()));
    connect(ui->MoveBottom, SIGNAL(pressed()), this, SLOT(onMoveBottom()));
    connect(ui->ZoomIn, SIGNAL(pressed()), this, SLOT(onZoomIn()));
    connect(ui->ZoomOut, SIGNAL(pressed()), this, SLOT(onZoomOut()));
    connect(ui->focusIn, SIGNAL(pressed()), this, SLOT(onFocusIn()));
    connect(ui->focusOut, SIGNAL(pressed()), this, SLOT(onFocusOut()));
    connect(ui->angleUpLeft, SIGNAL(pressed()), this, SLOT(onAngleUpLeft()));
    connect(ui->angleUpRight, SIGNAL(pressed()), this, SLOT(onAngleUpRight()));
    connect(ui->angleDownLeft, SIGNAL(pressed()), this, SLOT(onAngleDownLeft()));
    connect(ui->angleDownRight, SIGNAL(pressed()), this, SLOT(onAngleDownRight()));

    connect(ui->MoveLeft, SIGNAL(released()), this, SLOT(onStopMoving()));
    connect(ui->MoveTop, SIGNAL(released()), this, SLOT(onStopMoving()));
    connect(ui->MoveRight, SIGNAL(released()), this, SLOT(onStopMoving()));
    connect(ui->MoveBottom, SIGNAL(released()), this, SLOT(onStopMoving()));
    connect(ui->ZoomIn, SIGNAL(released()), this, SLOT(onStopZooming()));
    connect(ui->ZoomOut, SIGNAL(released()), this, SLOT(onStopZooming()));
    connect(ui->focusIn, SIGNAL(released()), this, SLOT(onStopFocusing()));
    connect(ui->focusOut, SIGNAL(released()), this, SLOT(onStopFocusing()));
    connect(ui->angleUpLeft, SIGNAL(released()), this, SLOT(onStopMoving()));
    connect(ui->angleUpRight, SIGNAL(released()), this, SLOT(onStopMoving()));
    connect(ui->angleDownLeft, SIGNAL(released()), this, SLOT(onStopMoving()));
    connect(ui->angleDownRight, SIGNAL(released()), this, SLOT(onStopMoving()));

    connect(ui->autoFocus, SIGNAL(clicked()), this, SLOT(onAutoFocusChanged()));

    //    connect(ui->osdMenu , SIGNAL(currentIndexChanged(int)) , this , SLOT(onOSDMenuChanged(int)));

    connect(ui->presetMode, SIGNAL(clicked()), this, SLOT(onPresetModeSet()));
    connect(ui->recallMode, SIGNAL(clicked()), this, SLOT(onRecallModeSet()));
    //    connect(ui->presetSpeed , SIGNAL(currentIndexChanged(int)) , this , SLOT(onPresetSpeedSet(int)));

    connect(ui->mem1, SIGNAL(clicked()), this, SLOT(onMem1Clicked()));
    connect(ui->mem2, SIGNAL(clicked()), this, SLOT(onMem2Clicked()));
    connect(ui->mem3, SIGNAL(clicked()), this, SLOT(onMem3Clicked()));
    connect(ui->mem4, SIGNAL(clicked()), this, SLOT(onMem4Clicked()));
    connect(ui->mem5, SIGNAL(clicked()), this, SLOT(onMem5Clicked()));
    connect(ui->mem6, SIGNAL(clicked()), this, SLOT(onMem6Clicked()));
    connect(ui->mem7, SIGNAL(clicked()), this, SLOT(onMem7Clicked()));
    connect(ui->mem8, SIGNAL(clicked()), this, SLOT(onMem8Clicked()));
    connect(ui->mem9, SIGNAL(clicked()), this, SLOT(onMem9Clicked()));
    //    connect(ui->mem10, SIGNAL(clicked()), this, SLOT(onMem10Clicked()));//newly removed

    connect(ui->connectCam1, SIGNAL(clicked()), this, SLOT(onCam1Clicked()));
    connect(ui->connectCam2, SIGNAL(clicked()), this, SLOT(onCam2Clicked()));
    connect(ui->connectCam3, SIGNAL(clicked()), this, SLOT(onCam3Clicked()));
    connect(ui->connectCam4, SIGNAL(clicked()), this, SLOT(onCam4Clicked()));
    connect(ui->connectCam5, SIGNAL(clicked()), this, SLOT(onCam5Clicked()));
    connect(ui->connectCam6, SIGNAL(clicked()), this, SLOT(onCam6Clicked()));
    connect(ui->connectCam7, SIGNAL(clicked()), this, SLOT(onCam7Clicked()));
    connect(ui->connectCam8, SIGNAL(clicked()), this, SLOT(onCam8Clicked()));

    connect(ui->irisUp, SIGNAL(clicked()), this, SLOT(onIrisUpClicked()));
    connect(ui->irisDown, SIGNAL(clicked()), this, SLOT(onIrisDownClicked()));
    //    connect(ui->irisReset, SIGNAL(clicked()), this, SLOT(onIrisResetClicked()));
    connect(ui->irisCombo , SIGNAL(currentIndexChanged(int)) , this , SLOT(onIrisComboChanged(int)));

    connect(ui->brightUp , SIGNAL(clicked()) , this , SLOT(onBrightUpClicked()));
    connect(ui->brightDown , SIGNAL(clicked()) , this , SLOT(onBrightDownClicked()));
    //    connect(ui->brightReset , SIGNAL(clicked()) , this , SLOT(onBrightResetClicked()));
    connect(ui->brightCombo , SIGNAL(currentIndexChanged(int)) , this , SLOT(onBrightComboChanged(int)));

    connect(ui->shutterUp , SIGNAL(clicked()) , this , SLOT(onShutterUpClicked()));
    connect(ui->shutterDown , SIGNAL(clicked()) , this , SLOT(onShutterDownClicked()));
    //    connect(ui->shutterReset , SIGNAL(clicked()) , this , SLOT(onShutterResetClicked()));
    connect(ui->shutterCombo , SIGNAL(currentIndexChanged(int)) , this , SLOT(onShutterComboChanged(int)));

    connect(ui->aeModeSelect , SIGNAL(currentIndexChanged(int)) , this , SLOT(onAeModeSelected(int)));
    connect(ui->wbCombo , SIGNAL(currentIndexChanged(int)) , this , SLOT(onWBModeSelected(int)));

    connect(ui->panTiltLock , SIGNAL(stateChanged(int)) , this , SLOT(onPanTiltLockClicked()));
    connect(ui->panSpeed , SIGNAL(currentIndexChanged(int)) , this , SLOT(onPanSpeedChanged(int)));
    connect(ui->tiltSpeed , SIGNAL(currentIndexChanged(int)) , this , SLOT(onTiltSpeedChanged(int)));
    //newly added
    connect(ui->zoomSpeed , SIGNAL(currentIndexChanged(int)) , this , SLOT(onZoomSpeedChanged(int)));
    connect(ui->focusSpeed , SIGNAL(currentIndexChanged(int)) , this , SLOT(onFocusSpeedChanged(int)));
    //newly added 08/19
    connect(ui->lockFocus, SIGNAL(pressed()), this, SLOT(onFocusLockClicked()));


    connect(ui->luminanceSlider , SIGNAL(valueChanged(int)) , this , SLOT(onLuminanceSliderMoved(int)));
    connect(ui->contrastSlider , SIGNAL(valueChanged(int)) , this , SLOT(onContrastSliderMoved(int)));
    connect(ui->hueSlider , SIGNAL(valueChanged(int)) , this , SLOT(onHueSliderMoved(int)));
//    connect(ui->rGainSlider , SIGNAL(valueChanged(int)) , this , SLOT(onRGainSliderMoved(int)));
//    connect(ui->bGainSlider , SIGNAL(valueChanged(int)) , this , SLOT(onBGainSliderMoved(int)));

    connect(ui->osdMenuBtn , SIGNAL(clicked()) , this , SLOT(onOsdMenuClicked()));
    connect(ui->osdEnterBtn , SIGNAL(clicked()) , this , SLOT(onOsdEnterClicked()));
    connect(ui->osdBackBtn , SIGNAL(clicked()) , this , SLOT(onOsdBackClicked()));

    connect(ui->imgFlipBtn , SIGNAL(clicked()) , this , SLOT(onImageFlip()));
    connect(ui->imgMirrorBtn  , SIGNAL(clicked()) , this , SLOT(onImageMirror()));

    connect(ui->lowLightBtn , SIGNAL(clicked(bool)) , this , SLOT(onLowLightBtnClicked()));
    connect(ui->sportsDayBtn , SIGNAL(clicked(bool)) , this , SLOT(onSportsDayBtnClicked()));
    connect(ui->sportsNightBtn , SIGNAL(clicked(bool)) , this , SLOT(onSportsNightBtnClicked()));
    connect(ui->onePushTriggerBtn , SIGNAL(clicked(bool)) , this , SLOT(onOnePushTriggerBtnClicked()));
    ui->onePushTriggerBtn->setEnabled(false);
    //connect(ui->resetBtn , SIGNAL(clicked()) , this , SLOT(onResetBtnClicked()));

    //    connect(ui->speedByZoom , SIGNAL(clicked()) , this , SLOT(onSpeedByZoomClicked()));
    //    connect(ui->focusLock , SIGNAL(clicked()) , this , SLOT(onFocusLockClicked()));
    //    connect(ui->goButton , SIGNAL(clicked()) , this , SLOT(onMorePresetBtnClicked()));
    /*connecting action listeners end*/

    /*insert into panSpeed , tiltSpeed , zoomSpeed begin ， focus Speed begin*/
    for (int i = 0; i < 24; i++)
    {
        ui->panSpeed->addItem(QString::number(i + 1));
    }
    ui->panSpeed->setCurrentIndex(4);
    for (int i = 0; i < 20; i++)
    {
        ui->tiltSpeed->addItem(QString::number(i + 1));
    }
    ui->tiltSpeed->setCurrentIndex(4);
    for (int i = 0; i < 8; i++)
    {
        ui->zoomSpeed->addItem(QString::number(i));
    }
    ui->zoomSpeed->setCurrentIndex(5);
    for (int i = 0; i < 8; i++)
    {
        ui->focusSpeed->addItem(QString::number(i));
    }
    ui->focusSpeed->setCurrentIndex(5);
    //    for(int i = 0 ; i < 24 ; i ++)
    //    {
    //        ui->presetSpeed->addItem(QString::number(i + 1));
    //    }
    //    ui->presetSpeed->setCurrentIndex(4);
    //    for(int i = 6 ; i < 50 ; i ++)
    //    {
    //        if(i != 9)
    //            ui->morePresets->addItem(QString::number(i + 1));
    //    }
    //    ui->morePresets->setCurrentIndex(0);//if index < 6 index + 3, index >= 6 then index + 4
    for(int i = 0 ; i < 5 ; i ++)
    {
        ui->aeModeSelect->addItem(aeModes[i]);
    }

    /*insert into panSpeed , tiltSpeed , zoomSpeed end ， focusSpeed end*/

    /*ui presets begin*/
    //    ui->presetMode->setChecked(false);//it is done in ui
    //ui->autoFocus->setChecked(true);
    //    ui->aeModeSelect->setCurrentIndex(0);//after loadSEttings then it will be set.
    //ui->panTiltLock->setChecked(true);
    /*ui presets end*/

    /*insert exposure mode combo begin*/
    for(int i = 0 ; i < 17 ; i ++)
    {
        ui->shutterCombo->addItem(shutters[i]);
    }

    for(int i = 0 ; i < 13 ; i ++)
    {
        ui->irisCombo->addItem(irises[i]);
    }

    for(int i = 0 ; i < 18 ; i ++)
    {
        ui->brightCombo->addItem(QString::number(i));
    }

    for(int i = 0 ; i < 5 ; i ++)
    {
        ui->wbCombo->addItem(wbModes[i]);
    }
    /*insert exposure mode combo end*/

    /*menu settings begin*/
    auto exitAction = []
    {
        exit(0);
    };
    connect(ui->actionExit, &QAction::triggered, exitAction);
    connect(ui->actionMultiCameras, SIGNAL(triggered(bool)), this, SLOT(onMultiCameraClicked()));
    connect(ui->actionJoyStick , SIGNAL(triggered(bool)) , this , SLOT(onJoyStickClicked()));
    connect(ui->actionAboutUs, SIGNAL(triggered(bool)), this, SLOT(onAboutUsClicked()));
    //newly added 08/19
    connect(ui->actionHotkey, SIGNAL(triggered(bool)), this, SLOT(onHotkeyHelpClicked()));
    connect(ui->actionXBbox, SIGNAL(triggered(bool)), this, SLOT(onXboxHelpClicked()));
    connect(ui->actionPanTiltLimitSetting, SIGNAL(triggered(bool)), this, SLOT(onPanTiltLimitSettingClicked()));
    connect(ui->actionCompactMode , SIGNAL(triggered(bool)) , this , SLOT(onCompactViewClicked()));
    connect(ui->actionAdvancedMode , SIGNAL(triggered(bool)) , this , SLOT(onAdvancedViewClicked()));
    connect(ui->actionStatusBar , SIGNAL(triggered(bool)) , this , SLOT(onStatusBarToggle()));
    /*menu settings end*/

    /*set statusbar begin*/
//    panStatus = new QLabel();
//    tiltStatus = new QLabel();
//    zoomStatus = new QLabel();
//    focusStatus = new QLabel();
//    shutterStatus = new QLabel();
//    irisStatus = new QLabel();
//    brightStatus = new QLabel();
//    onlineStatus = new QLabel();
//    luminanceStatus = new QLabel();
//    hueStatus = new QLabel();
//    contrastStatus = new QLabel();
    onlineStatus = new QLabel();
    devNameLabel = new QLabel();
//    rGainStatus = new QLabel();
//    bGainStatus = new QLabel();
//    ui->statusBar->addPermanentWidget(panStatus, 1);
//    ui->statusBar->addPermanentWidget(tiltStatus, 1);
//    ui->statusBar->addPermanentWidget(zoomStatus, 1);
//    ui->statusBar->addPermanentWidget(focusStatus , 1);
//    ui->statusBar->addPermanentWidget(shutterStatus, 1);
//    ui->statusBar->addPermanentWidget(irisStatus, 1);
//    ui->statusBar->addPermanentWidget(brightStatus , 1);
//    ui->statusBar->addPermanentWidget(luminanceStatus , 1);
//    ui->statusBar->addPermanentWidget(contrastStatus , 1);
//    ui->statusBar->addPermanentWidget(hueStatus , 1);
//    ui->statusBar->addPermanentWidget(rGainStatus , 1);
//    ui->statusBar->addPermanentWidget(bGainStatus , 1);
    ui->statusBar->addPermanentWidget(devNameLabel, 3);
    ui->statusBar->addPermanentWidget(onlineStatus, 1);

    ui->statusBar->hide();
    devNameLabel->setText("Getting device name...");
    devNameLabel->setAlignment(Qt::AlignRight);
    onlineStatus->setText("offline");
    onlineStatus->setAlignment(Qt::AlignRight);
    /*set statusbar end*/

    /*initializing camera variable begin*/
    panPos[0] = 0x00;
    panPos[1] = 0x00;
    panPos[2] = 0x00;
    panPos[3] = 0x00;
    tiltPos[0] = 0x00;
    tiltPos[1] = 0x00;
    tiltPos[2] = 0x00;
    tiltPos[3] = 0x00;
    zoomLevel[0] = 0x00;
    zoomLevel[1] = 0x00;
    zoomLevel[2] = 0x00;
    zoomLevel[3] = 0x00;
    focusLevel[0] = 0x00;
    focusLevel[1] = 0x00;
    focusLevel[2] = 0x00;
    focusLevel[3] = 0x00;

    irisLevel = 0x00;
    shutterLevel = 0x01;
    brightLevel = 0x00;
    aeMode = 0x00;
    luminanceLevel = 0x00;
    contrastLevel = 0x00;
    hueLevel = 0x00;
    wbMode = 0x00;
//    rGain = 0x00;
//    bGain = 0x00;

    //    irisLevel[0] = 0x00;
    //    irisLevel[1] = 0x00;
    //    shutterLevel[0] = 0x00;
    //    shutterLevel[1] = 0x00;
    //    brightLevel[0] = 0x00;
    //    brightLevel[1] = 0x00;
    //    luminanceLevel[0] = 0x00;
    //    luminanceLevel[1] = 0x00;
    //    contrastLevel[0] = 0x00;
    //    contrastLevel[1] = 0x00;

    /*initializing camera variable end*/

    /*set the default ip begin*/
    currentCamIp = DEFAULT_CAMERA_IP;
    /*set the default ip end*/

    /*load camera names begin*/
    loadMultiCameraNames();
    /*load camera names end*/

    //set initial value
    isControllable = false;

    /*set the socket properties begin*/
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(onClientConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)) , this , SLOT(onWriteFinish()));
    /*set socket properties end*/

    /*automatically load camera info begin*/
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(READ_DATA_INTERVAL);
    /*automatically load camera info end*/



    /*for snapshot download begin*/
    downLoadManager = new QNetworkAccessManager();
    connect(downLoadManager , SIGNAL(finished(QNetworkReply*)) , this , SLOT(onFileDownloaded(QNetworkReply*)));
    /*for snapshot download end*/

    //newly added 08/19
    /*for get name begin*/
    devNameGetter = new QNetworkAccessManager(this);
    connect(devNameGetter , SIGNAL(finished(QNetworkReply*)) , this , SLOT(onGetName(QNetworkReply*)));
    /*for get name end*/

    initialWidth = this->width();
    initialHeight = this->height();
    //centerH = QApplication::desktop()->screenGeometry().height() / 2;
    //centerW = QApplication::desktop()->screenGeometry().width() / 2;
    showCompactView();

}

MainWindow::~MainWindow()
{
    delete socket;
    delete settings;
    delete downLoadManager;
    if(timer->isActive())
        timer->stop();
    delete timer;
    delete ui;
}

/*mainwindow instance signal begin*/
void MainWindow::onConnectToCamera()
{
    //this function for connect button action
    //    QString ip;
    //    ip = ui->CameraIP->text();
    //    connectHost(ip);
}
void MainWindow::onClientConnected()
{
    log("-----------------------------------Connected------------------------------------");
    isControllable = true;

    //disable focusin and focusout
    ui->focusIn->setEnabled(false);
    ui->focusOut->setEnabled(false);
    startFlag = 0;//this is for first recevied values

    //newly added
    loadSettings(currentCamIp);
    /*calling preset camera states*/
    //recallPreset(10);//newly removed
    /*load the camera info and display*/
    loadCameraInfo();

    /*set initial values speedByzoom , focusLock , MorePresetButton*/
    initConnection();
    onlineStatus->setText("online");
    sendDevNameRequest();
}
void MainWindow::onClientDisconnected()
{
    log("-----------------------------------Disconnected------------------------------------");
    currentCommandType = Command;
    int length = commandQue.size();
    for(int i = 0 ; i < length ; i ++)
    {
        COMMAND* command = commandQue.at(0);
        commandQue.removeAt(0);
        delete command;
    }
    commandQue.empty();

    isControllable = false;
    ui->focusIn->setEnabled(false);
    ui->focusOut->setEnabled(false);

    startFlag = 2;
    getStatusStep = 0;
    onlineStatus->setText("offline");
    devNameLabel->setText("Getting device name...");
}

void MainWindow::onMoveLeft()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        this->moveLeftOnce(panSpeed, tiltSpeed);
}
void MainWindow::onMoveTop()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        moveTopOnce(panSpeed, tiltSpeed);
}
void MainWindow::onMoveRight()
{

    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        moveRightOnce(panSpeed, tiltSpeed);
}
void MainWindow::onMoveBottom()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        moveBottomOnce(panSpeed, tiltSpeed);
}
void MainWindow::onMoveHome()
{
    if (isControllable)
        moveHome();
}
void MainWindow::onZoomIn()
{
    int zoomSpeed = ui->zoomSpeed->currentIndex();
    if (isControllable)
        zoomInOnce(zoomSpeed);
}
void MainWindow::onZoomOut()
{
    int zoomSpeed = ui->zoomSpeed->currentIndex();
    if (isControllable)
        zoomOutOnce(zoomSpeed);
}
void MainWindow::onStopMoving()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        stopMoving(panSpeed, tiltSpeed);
}
void MainWindow::onStopZooming()
{
    if (isControllable)
        stopZooming();
}
void MainWindow::onOSDMenuChanged(int index)
{
    if (isControllable)
        osdMenuChange(index);
}
void MainWindow::onAngleUpLeft()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        angleUpLeft(panSpeed, tiltSpeed);
}
void MainWindow::onAngleUpRight()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        angleUpRight(panSpeed, tiltSpeed);

}
void MainWindow::onAngleDownRight()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        angleDownRight(panSpeed, tiltSpeed);
}
void MainWindow::onAngleDownLeft()
{
    int panSpeed = ui->panSpeed->currentIndex() + 1;
    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
    if (isControllable)
        angleDownLeft(panSpeed, tiltSpeed);
}
void MainWindow::onAutoFocusChanged()
{
    if (isControllable && startFlag != 1)
        autoFocusChange(ui->autoFocus->isChecked());
}
void MainWindow::onFocusIn()
{
    int focusSpeed = ui->focusSpeed->currentIndex();
    if (isControllable)
        focusInOnce(focusSpeed);
}
void MainWindow::onFocusOut()
{
    int focusSpeed = ui->focusSpeed->currentIndex();
    if (isControllable)
        focusOutOnce(focusSpeed);
}
void MainWindow::onStopFocusing()
{
    if (isControllable)
        stopFocusing();
}
void MainWindow::onPresetModeSet()
{
    if (isControllable)
        isPresetMode = true;
}
void MainWindow::onRecallModeSet()
{
    if (isControllable)
        isPresetMode = false;
}
void MainWindow::onMem1Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {

            QNetworkReply *reply = downLoadManager->get(QNetworkRequest(QUrl("http://" + currentCamIp + DOWNLOAD_FILE_URI)));
            downloadDestination[reply] = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString::asprintf("%s" , (DOWNLOAD_FILE_DEST_PREFIX)) + currentCamIp + QString::number(1) + ".jpg");
            setPreset(1);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #1 Name:"), QLineEdit::Normal,
                                                    ui->mem1->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem1->text();
            ui->mem1->setText(presetText);
            savePresetSettings(1, presetText);
            returnRecallMode();

        }
        else
        {

            recallPreset(1);
            loadPresetSettings(1 , 0);
        }
    }
}
void MainWindow::onMem2Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {

            QNetworkReply *reply = downLoadManager->get(QNetworkRequest(QUrl("http://" + currentCamIp + DOWNLOAD_FILE_URI)));
            downloadDestination[reply] = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString::asprintf("%s" , (DOWNLOAD_FILE_DEST_PREFIX)) + currentCamIp + QString::number(2) + ".jpg");
            setPreset(2);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #2 Name:"), QLineEdit::Normal,
                                                    ui->mem2->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem2->text();
            ui->mem2->setText(presetText);
            savePresetSettings(2, presetText);
            returnRecallMode();

        }
        else
        {
            recallPreset(2);
            loadPresetSettings(2 , 0);
        }
    }
}
void MainWindow::onMem3Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {
            QNetworkReply *reply = downLoadManager->get(QNetworkRequest(QUrl("http://" + currentCamIp + DOWNLOAD_FILE_URI)));
            downloadDestination[reply] = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString::asprintf("%s" , (DOWNLOAD_FILE_DEST_PREFIX)) + currentCamIp + QString::number(3) + ".jpg");
            setPreset(3);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #3 Name:"), QLineEdit::Normal,
                                                    ui->mem3->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem3->text();
            ui->mem3->setText(presetText);
            savePresetSettings(3 , presetText);

            returnRecallMode();

        }
        else
        {
            recallPreset(3);
            loadPresetSettings(3 , 0);
        }
    }
}
void MainWindow::onMem4Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {
            QNetworkReply *reply = downLoadManager->get(QNetworkRequest(QUrl("http://" + currentCamIp + DOWNLOAD_FILE_URI)));
            downloadDestination[reply] = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString::asprintf("%s" , (DOWNLOAD_FILE_DEST_PREFIX)) + currentCamIp + QString::number(4) + ".jpg");
            setPreset(4);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #4 Name:"), QLineEdit::Normal,
                                                    ui->mem1->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem4->text();
            ui->mem4->setText(presetText);
            savePresetSettings(4 , presetText);

            returnRecallMode();

        }
        else
        {
            recallPreset(4);
            loadPresetSettings(4 , 0);
        }
    }
}
void MainWindow::onMem5Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {
            QNetworkReply *reply = downLoadManager->get(QNetworkRequest(QUrl("http://" + currentCamIp + DOWNLOAD_FILE_URI)));
            downloadDestination[reply] = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString::asprintf("%s" , (DOWNLOAD_FILE_DEST_PREFIX)) + currentCamIp + QString::number(5) + ".jpg");
            setPreset(5);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #5 Name:"), QLineEdit::Normal,
                                                    ui->mem5->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem5->text();
            ui->mem5->setText(presetText);
            savePresetSettings(5 , presetText);

            returnRecallMode();

        }
        else
        {
            recallPreset(5);
            loadPresetSettings(5 , 0);
        }
    }
}
void MainWindow::onMem6Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {
            QNetworkReply *reply = downLoadManager->get(QNetworkRequest(QUrl("http://" + currentCamIp + DOWNLOAD_FILE_URI)));
            downloadDestination[reply] = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString::asprintf("%s" , (DOWNLOAD_FILE_DEST_PREFIX)) + currentCamIp + QString::number(6) + ".jpg");
            setPreset(6);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #6 Name:"), QLineEdit::Normal,
                                                    ui->mem6->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem6->text();
            ui->mem6->setText(presetText);
            savePresetSettings(6 , presetText);

            returnRecallMode();

        }
        else
        {
            recallPreset(6);
            loadPresetSettings(6 , 0);
        }
    }
}
void MainWindow::onMem7Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {
            setPreset(7);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #7 Name:"), QLineEdit::Normal,
                                                    ui->mem7->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem7->text();
            ui->mem7->setText(presetText);
            savePresetSettings(7 , presetText);

            returnRecallMode();

        }
        else
        {
            recallPreset(7);
            loadPresetSettings(7 , 0);
        }
    }
}
void MainWindow::onMem8Clicked()
{
    if (isControllable)
    {
        if (isPresetMode)
        {
            setPreset(8);
            //newly added: 07/2019
            bool ok;
            QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                     tr("Preset #8 Name:"), QLineEdit::Normal,
                                                    ui->mem8->text(), &ok);
            if (!ok || presetText.isEmpty())
                presetText = ui->mem8->text();
            ui->mem8->setText(presetText);
            savePresetSettings(8 , presetText);

            returnRecallMode();

        }
        else
        {
            recallPreset(8);
            loadPresetSettings(8 , 0);
        }
    }
}
void MainWindow::onMem9Clicked()
{
    if (isPresetMode)
    {
        setPreset(9);
        //newly added: 07/2019
        bool ok;
        QString presetText = QInputDialog::getText(nullptr, tr("PTZOptics"),
                                                 tr("Preset #9 Name:"), QLineEdit::Normal,
                                                ui->mem9->text(), &ok);
        if (!ok || presetText.isEmpty())
            presetText = ui->mem9->text();
        ui->mem9->setText(presetText);
        savePresetSettings(9 , presetText);

        returnRecallMode();

    }
    else
    {
        recallPreset(9);
        loadPresetSettings(9 , 0);
    }
}
//void MainWindow::onMem10Clicked()
//{
//    if (isPresetMode)
//    {
//        //            ui->mem9->setText(ui->presetText->text().left(10));
//        setPreset(11);
//    }
//    else
//    {
//        recallPreset(11);
//    }
//}
void MainWindow::onAboutUsClicked()
{
    showAboutUs();
}
//newly added 08/19
void MainWindow::onPanTiltLimitSettingClicked()
{
    if(!isControllable)
    {
        showMessage("No camera connected");
        return;
    }
    PanTiltLimitDlg panTiltLimitDlg(this);
    panTiltLimitDlg.exec();
}
void MainWindow::onXboxHelpClicked()
{
// TODO: fix and uncomment
#if 0
    XBoxHelp xboxHelp;
    xboxHelp.exec();
    #endif //0
    
}
void MainWindow::onHotkeyHelpClicked()
{
    HotkeyHelp hotkeyHelp;
    hotkeyHelp.exec();
}
void MainWindow::onFocusLockClicked()
{
    if(!isControllable)
        return;
    qDebug()<<"Focuslock";
    if(!ui->lockFocus->isChecked())
    {
        focusLock(true);
        ui->autoFocus->setEnabled(false);
        ui->focusIn->setEnabled(false);
        ui->focusOut->setEnabled(false);
    }
    else
    {
        focusLock(false);
        ui->autoFocus->setEnabled(true);
        ui->focusIn->setEnabled(!ui->autoFocus->isChecked());
        ui->focusOut->setEnabled(!ui->autoFocus->isChecked());
    }

}
QString MainWindow::getCurrentCameraIp()
{
    return currentCamIp;
}
bool MainWindow::getIsControllable()
{
    return isControllable;
}



void MainWindow::onMultiCameraClicked()
{
    showMultiCameraSetting();
}
void MainWindow::onJoyStickClicked()
{
    showJoyStickSetting();
}
void MainWindow::setDefaultColor()
{
    ui->connectCam1->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
    ui->connectCam2->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
    ui->connectCam3->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
    ui->connectCam4->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
    ui->connectCam5->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
    ui->connectCam6->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
    ui->connectCam7->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
    ui->connectCam8->setStyleSheet("QPushButton{background : grey;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");
}
void MainWindow::onCam1Clicked()
{
    setDefaultColor();
    ui->connectCam1->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA1_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);
    currentCamIp = ip;

    connectHost(ip);
}
void MainWindow::onCam2Clicked()
{
    setDefaultColor();
    ui->connectCam2->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA2_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);

    currentCamIp = ip;

    connectHost(ip);
}
void MainWindow::onCam3Clicked()
{
    setDefaultColor();
    ui->connectCam3->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA3_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);

    currentCamIp = ip;

    connectHost(ip);
}
void MainWindow::onCam4Clicked()
{
    setDefaultColor();
    ui->connectCam4->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA4_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);

    currentCamIp = ip;

    connectHost(ip);
}
void MainWindow::onCam5Clicked()
{
    setDefaultColor();
    ui->connectCam5->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA5_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);

    currentCamIp = ip;

    connectHost(ip);
}
void MainWindow::onCam6Clicked()
{
    setDefaultColor();
    ui->connectCam6->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA6_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);

    currentCamIp = ip;

    connectHost(ip);
}
void MainWindow::onCam7Clicked()
{
    setDefaultColor();
    ui->connectCam7->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA7_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);

    currentCamIp = ip;

    connectHost(ip);
}
void MainWindow::onCam8Clicked()
{
    setDefaultColor();
    ui->connectCam8->setStyleSheet("QPushButton{background : #4f4f4f;padding : 5px;border-radius : 3px;}QPushButton:pressed{background : #474747;}QPushButton:hover{background : #c9c9c9;}");

    QString ip = settings->value(DEFAULT_CAMERA8_IP_KEY, DEFAULT_CAMERA_IP).toString();

    loadPresetTitleByCamera(ip);

    currentCamIp = ip;

    connectHost(ip);
}

//void MainWindow::keyPressEvent(QKeyEvent *ev)
//{
//    if(ev->isAutoRepeat())
//        return;
//    int key = ev->key();
//    int panSpeed = ui->panSpeed->currentIndex() + 1;
//    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
//    switch (key)
//    {
//    case Qt::Key_W:
//        if (isControllable)
//            moveTopOnce(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_Up:
//        if (isControllable)
//            moveTopOnce(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_S:
//        if (isControllable)
//            moveBottomOnce(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_Down:
//        if (isControllable)
//            moveBottomOnce(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_A:
//        if (isControllable)
//            moveLeftOnce(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_Left:
//        if (isControllable)
//            moveLeftOnce(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_D:
//        if (isControllable)
//            moveRightOnce(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_Right:
//        if (isControllable)
//            moveRightOnce(panSpeed, tiltSpeed);
//        break;
//    default:
//        break;
//    }

//}
//void MainWindow::keyReleaseEvent(QKeyEvent *ev)
//{
//    if(ev->isAutoRepeat())
//        return;
//    int key = ev->key();
//    int panSpeed = ui->panSpeed->currentIndex() + 1;
//    int tiltSpeed = ui->tiltSpeed->currentIndex() + 1;
//    switch (key)
//    {
//    case Qt::Key_W:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_Up:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_S:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_Down:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_A:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_Left:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_D:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//    case Qt::Key_Right:
//        if (isControllable)
//            stopMoving(panSpeed, tiltSpeed);
//        break;
//    case Qt::Key_1:
//    case Qt::Key_2:
//    case Qt::Key_3:
//    case Qt::Key_4:
//    case Qt::Key_5:
//    case Qt::Key_6:
//    case Qt::Key_7:
//    case Qt::Key_8:
//    case Qt::Key_9:
//        if(settings->value(SETTING_HOTKEY_AVAILABLE , false).toBool())
//            onCallPreset(key - 0x30);
//    default:
//        break;
//    }
//}

void MainWindow::onReadyRead()
{
    readBytes();
}
void MainWindow::onIrisUpClicked()
{
    if (isControllable)
        irisUpClicked();
}
void MainWindow::onIrisDownClicked()
{
    if (isControllable)
        irisDownClicked();
}
void MainWindow::onIrisResetClicked()
{
    if (isControllable)
        irisResetClicked();
}
void MainWindow::onIrisComboChanged(int index)
{
    if(isControllable && startFlag != 1)
        irisComboChanged(index);
}

void MainWindow::onShutterUpClicked()
{
    if (isControllable)
        shutterUpClicked();
}
void MainWindow::onShutterDownClicked()
{
    if (isControllable)
        shutterDownClicked();
}
void MainWindow::onShutterResetClicked()
{
    if (isControllable)
        shutterResetClicked();
}
void MainWindow::onShutterComboChanged(int index)
{
    if(isControllable && startFlag != 1)
        shutterComboChanged(index);
}
void MainWindow::onBrightUpClicked()
{
    if(isControllable)
        brightUpClicked();
}
void MainWindow::onBrightDownClicked()
{
    if(isControllable)
        brightDownClicked();
}
void MainWindow::onBrightResetClicked()
{
    if(isControllable)
        brightResetClicked();
}
void MainWindow::onBrightComboChanged(int index)
{
    if(isControllable && startFlag != 1)
        brightComboChanged(index);
}
void MainWindow::onPanTiltLockClicked()
{
    if(ui->panTiltLock->isChecked())
    {
        ui->panSpeed->setCurrentIndex(6);
        ui->tiltSpeed->setCurrentIndex(6);
    }
    //newly added
    setSettings(currentCamIp , SETTING_KEY_PANTILTLOCK , ui->panTiltLock->isChecked());
}
//newly added
void MainWindow::onFocusSpeedChanged(int index)
{
    setSettings(currentCamIp , SETTING_KEY_FOCUSSPEED , index);
}
void MainWindow::onZoomSpeedChanged(int index)
{
    setSettings(currentCamIp , SETTING_KEY_ZOOMSPEED , index);
}
void MainWindow::onPanSpeedChanged(int index)
{
    if(ui->panTiltLock->isChecked())
    {
        if(index > 19)
            index = 19;
        ui->panSpeed->setCurrentIndex(index);
        ui->tiltSpeed->setCurrentIndex(index);
    }
    //newly added
    setSettings(currentCamIp , SETTING_KEY_PANSPEED , index);
}
void MainWindow::onTiltSpeedChanged(int index)
{
    if(ui->panTiltLock->isChecked())
    {
        ui->panSpeed->setCurrentIndex(index);
    }
    //newly added
    setSettings(currentCamIp , SETTING_KEY_TILTSPEED , index);
}
void MainWindow::onAeModeSelected(int index)
{
    //update ui status...disable and enable
    switch(index)
    {
    case 0://auto
        ui->shutterDown->setEnabled(false);
        ui->shutterUp->setEnabled(false);
        //        ui->shutterReset->setEnabled(false);
        ui->shutterCombo->setEnabled(false);

        ui->irisDown->setEnabled(false);
        ui->irisUp->setEnabled(false);
        //        ui->irisReset->setEnabled(false);
        ui->irisCombo->setEnabled(false);

        ui->brightDown->setEnabled(false);
        ui->brightUp->setEnabled(false);
        //        ui->brightReset->setEnabled(false);
        ui->brightCombo->setEnabled(false);
        break;
    case 1://shutter
        ui->shutterDown->setEnabled(true);
        ui->shutterUp->setEnabled(true);
        //        ui->shutterReset->setEnabled(true);
        ui->shutterCombo->setEnabled(true);

        ui->irisDown->setEnabled(true);
        ui->irisUp->setEnabled(true);
        //        ui->irisReset->setEnabled(true);
        ui->irisCombo->setEnabled(true);

        ui->brightDown->setEnabled(false);
        ui->brightUp->setEnabled(false);
        //        ui->brightReset->setEnabled(false);
        ui->brightCombo->setEnabled(false);
        break;
    case 2://shutter
        ui->shutterDown->setEnabled(true);
        ui->shutterUp->setEnabled(true);
        //        ui->shutterReset->setEnabled(true);
        ui->shutterCombo->setEnabled(true);

        ui->irisDown->setEnabled(false);
        ui->irisUp->setEnabled(false);
        //        ui->irisReset->setEnabled(false);
        ui->irisCombo->setEnabled(false);

        ui->brightDown->setEnabled(false);
        ui->brightUp->setEnabled(false);
        //        ui->brightReset->setEnabled(false);
        ui->brightCombo->setEnabled(false);
        break;
    case 3://iris
        ui->shutterDown->setEnabled(false);
        ui->shutterUp->setEnabled(false);
        //        ui->shutterReset->setEnabled(false);
        ui->shutterCombo->setEnabled(false);

        ui->irisDown->setEnabled(true);
        ui->irisUp->setEnabled(true);
        //        ui->irisReset->setEnabled(true);
        ui->irisCombo->setEnabled(true);

        ui->brightDown->setEnabled(false);
        ui->brightUp->setEnabled(false);
        //        ui->brightReset->setEnabled(false);
        ui->brightCombo->setEnabled(false);
        break;
    case 4://brightness
        ui->shutterDown->setEnabled(false);
        ui->shutterUp->setEnabled(false);
        //        ui->shutterReset->setEnabled(false);
        ui->shutterCombo->setEnabled(false);

        ui->irisDown->setEnabled(false);
        ui->irisUp->setEnabled(false);
        //        ui->irisReset->setEnabled(false);
        ui->irisCombo->setEnabled(false);

        ui->brightDown->setEnabled(true);
        ui->brightUp->setEnabled(true);
        //        ui->brightReset->setEnabled(true);
        ui->brightCombo->setEnabled(true);
        break;
    default:
        break;
    }
    if(isControllable && startFlag != 1)
    {
        aeModeSelected(index);
        setSettings(currentCamIp , SETTING_KEY_AEMODE , index);
    }
}
void MainWindow::onWBModeSelected(int index)
{
    //81 01 04 35 pq FF
    if(isControllable && startFlag != 1)
    {
        wbModeSelected(index);
    }
    if(isControllable)
    {
        if(index == 3)
            ui->onePushTriggerBtn->setEnabled(true);
        else
            ui->onePushTriggerBtn->setEnabled(false);
    }
}
void MainWindow::onLuminanceSliderMoved(int pos)
{
    if(isControllable && startFlag != 1)
        setLuminance(pos);
}
void MainWindow::onContrastSliderMoved(int pos)
{
    if(isControllable && startFlag != 1)
        setContrast(pos);
}
void MainWindow::onHueSliderMoved(int pos)
{
    if(isControllable && startFlag != 1)
        setHue(pos);
}
void MainWindow::onRGainSliderMoved(int pos)
{
//    if(isControllable && startFlag != 1)
//        setRGain(pos);
}
void MainWindow::onBGainSliderMoved(int pos)
{
//    if(isControllable && startFlag != 1)
//        setBGain(pos);

}
void MainWindow::onOsdMenuClicked()
{
    if(isControllable)
        osdMenuToggle();
}
void MainWindow::onOsdEnterClicked()
{
    if(isControllable)
        osdMenuEnter();
}
void MainWindow::onOsdBackClicked()
{
    if(isControllable)
        osdMenuBack();
}

void MainWindow::onImageFlip()
{
    if(isControllable)
        imageFlip();
}
void MainWindow::onImageMirror()
{
    if(isControllable)
        imageMirror();
}

void MainWindow::onTimer()
{

    if (isControllable)
        loadCameraInfo();
}
void MainWindow::onResetBtnClicked()
{
    if(isControllable)
        resetCamera();
}
void MainWindow::onPresetSpeedSet(int index)
{
    if(isControllable)
        presetSpeedSet(index);
}
void MainWindow::onSpeedByZoomClicked()
{
    if(isControllable)
        speedByZoom();
}
void MainWindow::onMorePresetBtnClicked()
{
    //    if (isControllable)
    //    {
    //        int index = 0;
    //        index = ui->morePresets->currentIndex();
    //        if(index >= 3)
    //            index += 8;
    //        else
    //            index += 7;
    //        if (isPresetMode)
    //        {
    //            setPreset(index);

    //        }
    //        else
    //        {
    //            recallPreset(index);
    //        }
    //    }
}
void MainWindow::onSharpnessMoved(int pos)
{
    if(isControllable)
        setSharpness(pos);
}
void MainWindow::onSaturationMoved(int pos)
{
    if(isControllable)
        setSaturation(pos);
}
void MainWindow::onCompactViewClicked()
{

    showCompactView();
}
void MainWindow::onAdvancedViewClicked()
{
    showAdvancedView();
}

void MainWindow::onWriteFinish()
{
    return;
}
void MainWindow::onFileDownloaded(QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();


        QFile *file = downloadDestination[reply];
        if(file->open(QFile::WriteOnly))
        {
            file->write(reply->readAll());
            file->flush();
            file->close();
        }
        //" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "
        if(file->fileName().right(5) == "1.jpg")
            ui->mem1->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + currentCamIp + "1.jpg');\n}");
        if(file->fileName().right(5) == "2.jpg")
            ui->mem2->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + currentCamIp + "2.jpg');\n}");
        if(file->fileName().right(5) == "3.jpg")
            ui->mem3->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + currentCamIp + "3.jpg');\n}");
        if(file->fileName().right(5) == "4.jpg")
            ui->mem4->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + currentCamIp + "4.jpg');\n}");
        if(file->fileName().right(5) == "5.jpg")
            ui->mem5->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + currentCamIp + "5.jpg');\n}");
        if(file->fileName().right(5) == "6.jpg")
            ui->mem6->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + currentCamIp + "6.jpg');\n}");
        delete file;
    }

    reply->deleteLater();
}
void MainWindow::onStatusBarToggle()
{
    statusBarToggle();
}
void MainWindow::onLowLightBtnClicked()
{
    ui->aeModeSelect->setCurrentIndex(1);//manual exposure
    ui->shutterCombo->setCurrentIndex(1);//1/60
    ui->irisCombo->setCurrentIndex(12);//F1.8
}
void MainWindow::onSportsDayBtnClicked()
{
    ui->aeModeSelect->setCurrentIndex(1);//manual exposure
    ui->shutterCombo->setCurrentIndex(6);//1/250
    ui->irisCombo->setCurrentIndex(5);//F5.6
}
void MainWindow::onSportsNightBtnClicked()
{
    ui->aeModeSelect->setCurrentIndex(1);//manual exposure
    ui->shutterCombo->setCurrentIndex(4);//1/120
    ui->irisCombo->setCurrentIndex(12);//F1.8
}
void MainWindow::onOnePushTriggerBtnClicked()
{
    //8101041005FF
    if(isControllable)
    {
        uint8_t command[6];
        command[0] = 0x81;
        command[1] = 0x01;
        command[2] = 0x04;
        command[3] = 0x10;
        command[4] = 0x05;
        command[5] = 0xff;
        addToQue(command , 6 , Command);
    }
}
void MainWindow::onLuminanceChangeTriggered(bool increase){
    setSliderValue(ui->luminanceSlider , increase);
}
void MainWindow::onContrastChangeTriggered(bool increase){
    setSliderValue(ui->contrastSlider , increase);
}
void MainWindow::onHueChangeTriggered(bool increase){
    setSliderValue(ui->hueSlider , increase);
}
/*mainwindow instance sginal end*/

/*remote control functions begin*/
void MainWindow::autoFocusClick()
{
    ui->autoFocus->click();
}
//we need to move slider every 3 action for smooth moving
int sliderFreq = 0;
void MainWindow::setSliderValue(QSlider *slider, bool increase)
{
    sliderFreq = (sliderFreq + 1) % 3;
    if(sliderFreq != 0)
    {
        return;
    }
    int val = slider->value();
    if(increase)
//        val += slider->pageStep();
        val ++;
    else
//        val -= slider->pageStep();
        val --;
    if(val > slider->maximum())
        val = slider->maximum();
    else if(val < -slider->minimum())
        val = slider->minimum();
    slider->setValue(val);
}
//newly added
void MainWindow::setSettings(QString curIp, QString settingKey , QVariant value)
{
    settings->setValue(curIp + "_" + settingKey , value);
}
//newly added
void MainWindow::loadSettings(QString curIp){
    //following should be the first
    bool panTiltLockVal = settings->value(curIp + "_" + SETTING_KEY_PANTILTLOCK , true).toBool();
    ui->panTiltLock->setChecked(panTiltLockVal);

    int panSpeedVal = settings->value(curIp + "_" + SETTING_KEY_PANSPEED , 7).toInt();
    ui->panSpeed->setCurrentIndex(panSpeedVal);
    int tiltSpeedVal = settings->value(curIp + "_" + SETTING_KEY_TILTSPEED , 7).toInt();
    ui->tiltSpeed->setCurrentIndex(tiltSpeedVal);
    int zoomSpeedVal = settings->value(curIp + "_" + SETTING_KEY_ZOOMSPEED , 5).toInt();
    ui->zoomSpeed->setCurrentIndex(zoomSpeedVal);
    int focusSpeedVal = settings->value(curIp + "_" + SETTING_KEY_FOCUSSPEED , 5).toInt();
    ui->focusSpeed->setCurrentIndex(focusSpeedVal);

    //newly added
//    int aeMode = settings->value(curIp + "_" + SETTING_KEY_AEMODE , 0).toInt();
    //    ui->aeModeSelect->setCurrentIndex(aeMode);
    //Read aeMode from camera and automatically set

}
//newly added
void MainWindow::savePresetSettings(int presetNum, QString presetText)
{
    settings->setValue(QString::asprintf("mem%d", presetNum) + currentCamIp, presetText);

    if(!ui->storeExposure->isChecked())
        return;
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_aemode" , ui->aeModeSelect->currentIndex());
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_shutterspeed" , ui->shutterCombo->currentIndex());
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_iris" , ui->irisCombo->currentIndex());
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_brightness" , ui->brightCombo->currentIndex());
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_luminance" , ui->luminanceSlider->value());
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_contrast" , ui->contrastSlider->value());
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_hue" , ui->hueSlider->value());
    settings->setValue(currentCamIp + "_" + QString::number(presetNum) + "_wbmode" , ui->wbCombo->currentIndex());
}
void MainWindow::loadPresetSettings(int presetNum , int loadingStep)
{

    //if we call following commands at the same time, it does not work well.
    //we should call it one by one.
    //asynchronously
    if(!ui->storeExposure->isChecked())
        return;
    switch(loadingStep)
    {
    case 0:
        ui->aeModeSelect->setCurrentIndex(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_aemode" , 0).toInt());
        break;
    case 1:
        if(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_aemode" , 0).toInt() == 1 || settings->value(currentCamIp + "_" + QString::number(presetNum) + "_aemode" , 0).toInt() == 2)
            ui->shutterCombo->setCurrentIndex(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_shutterspeed" , 0).toInt());
        break;
    case 2:
        if(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_aemode" , 0).toInt() == 1 || settings->value(currentCamIp + "_" + QString::number(presetNum) + "_aemode" , 0).toInt() == 3)
            ui->irisCombo->setCurrentIndex(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_iris" , 0).toInt());
        break;
    case 3:
        if(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_aemode" , 0).toInt() == 4)
            ui->brightCombo->setCurrentIndex(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_brightness" , 0).toInt());
        break;
    case 4:
        ui->luminanceSlider->setValue(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_luminance" , 7).toInt());
        break;
    case 5:
        ui->contrastSlider->setValue(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_contrast" , 7).toInt());
        break;
    case 6:
        ui->hueSlider->setValue(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_hue" , 7).toInt());
        break;
    case 7:
        ui->wbCombo->setCurrentIndex(settings->value(currentCamIp + "_" + QString::number(presetNum) + "_wbmode" , 0).toInt());
        break;
    default:
        break;
    }

    if(loadingStep != 7)
    {
        loadingStep ++;
        auto loadNext = [this , presetNum , loadingStep]{
            this->loadPresetSettings(presetNum , loadingStep);
        };
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer , &QTimer::timeout , loadNext);
        timer->start(500);
    }

}
//newly added
void MainWindow::showMessage(QString str)
{
    QMessageBox msgBox;
    msgBox.setText(str);
    msgBox.exec();
}
void MainWindow::statusBarToggle()
{
    if(ui->actionStatusBar->isChecked())
    {
        ui->statusBar->show();
    }
    else
    {
        ui->statusBar->hide();
    }
}
void MainWindow::setSharpness(int pos)
{
    //there is no sharpness command
}
void MainWindow::setSaturation(int pos)
{
    //there is no sharpness command
}
void MainWindow::initConnection()
{

    //    ui->speedByZoom->setChecked(true);
    //    speedByZoom();

    //    ui->aeModeSelect->setCurrentIndex(0);
    //    aeModeSelected(0);

    //    ui->focusLock->setChecked(true);
    //    focusLock();
}
void MainWindow::focusLock(bool lock)
{
    //81 0a 04 68 02 FF//lock 02 unlock 03
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x0a;
    command[2] = 0x04;
    command[3] = 0x68;
    if(lock)
        command[4] = 02;
    else
        command[4] = 03;
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::setPanTiltLimit(int panLeftLimit, int panRightLimit, int tiltDownLimit, int tiltUpLimit)
{
    //    81 01 06 07 00 0W 0Y 0Y 0Y 0Y 0Z 0Z 0Z 0Z FF
    //    W: 1 UpRight, 0: DownLeft
    //    YYYY: Pan Limit Position | F670 (left) to 0990 (right)
    //    ZZZZ: Tilt Limit Position | FAFO (down) to 0510 (up)
        //following code is very correct do not change
        uint8_t command[15];
        command[0] = 0x81;
        command[1] = 0x01;
        command[2] = 0x06;
        command[3] = 0x07;
        command[4] = 0x00;
        command[5] = 0x01;
        command[6] = (panRightLimit >> 12) % 16;
        command[7] = ((panRightLimit << 4) >> 12) % 16;
        command[8] = ((panRightLimit << 8) >> 12) % 16;
        command[9] = ((panRightLimit << 12) >> 12) % 16;
        command[10] = (tiltUpLimit >> 12) % 16;
        command[11] = ((tiltUpLimit << 4) >> 12) % 16;
        command[12] = ((tiltUpLimit << 8) >> 12) % 16;
        command[13] = ((tiltUpLimit << 12) >> 12) % 16;
        command[14] = 0xff;
        addToQue(command , 15 , Command);

        uint8_t command1[15];
        command1[0] = 0x81;
        command1[1] = 0x01;
        command1[2] = 0x06;
        command1[3] = 0x07;
        command1[4] = 0x00;
        command1[5] = 0x00;
        command1[6] = (panLeftLimit >> 12) % 16;
        command1[7] = ((panLeftLimit << 4) >> 12) % 16;
        command1[8] = ((panLeftLimit << 8) >> 12) % 16;
        command1[9] = ((panLeftLimit << 12) >> 12) % 16;
        command1[10] = (tiltDownLimit >> 12) % 16;
        command1[11] = ((tiltDownLimit << 4) >> 12) % 16;
        command1[12] = ((tiltDownLimit << 8) >> 12) % 16;
        command1[13] = ((tiltDownLimit << 12) >> 12) % 16;
        command1[14] = 0xff;
        addToQue(command1, 15, Command);
}
void MainWindow::clearLimits(bool upright)
{
    uint8_t command[15];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x07;
    command[4] = 0x01;
    if(upright)
        command[5] = 0x01;
    else
        command[5] = 0x00;
    command[6] = 0x07;
    command[7] = 0x0f;
    command[8] = 0x0f;
    command[9] = 0x0f;
    command[10] = 0x07;
    command[11] = 0x0f;
    command[12] = 0x0f;
    command[13] = 0x0f;
    command[14] = 0xff;
    addToQue(command , 15 , Command);
}
void MainWindow::setCallPresetSpeed(int speed)
{
    //81 01 06 01 pp FF
    speed = speed % 0x18 + 1;
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = (char)speed;
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::speedByZoom()
{
    //    isZoomBySpeed = ui->speedByZoom->isChecked();
}
void MainWindow::presetSpeedSet(int index)
{
    //81 01 06 01 p FF
    index ++;
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = char(index);
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::setLuminance(int pos)
{
    //81 01 04 A1 00 00 0p 0q FF
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0xa1;
    command[4] = 0x00;
    command[5] = 0x00;
    //    command[6] = (char)(pos/10);
    //    command[7] = (char)(pos%10);
    command[6] = 0x00;
    command[7] = (char)pos;
    command[8] = (char)0xff;
    addToQue(command , 9 , Command);
}
void MainWindow::setContrast(int pos)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0xa2;
    command[4] = 0x00;
    command[5] = 0x00;
    //    command[6] = (char)(pos/10);
    //    command[7] = (char)(pos%10);
    command[6] = 0x00;
    command[7] = (char)pos;
    command[8] = 0xff;
    addToQue(command , 9 , Command);
}
void MainWindow::setHue(int pos)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x4f;
    command[4] = 0x00;
    command[5] = 0x00;
    command[6] = 0x00;
    command[7] = (char)pos;
    command[8] = 0xff;
    addToQue(command , 9 , Command);
}
void MainWindow::setRGain(int pos)
{
    //81 01 04 43 00 00 0p 0q FF
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x43;
    command[4] = 0x00;
    command[5] = 0x00;
    command[6] = (char)(pos / 16);
    command[7] = (char)(pos % 16);
    command[8] = 0xff;
//    addToQue(command , 9 , OSD);
//RGain removed
}
void MainWindow::setBGain(int pos)
{
    //81 01 04 44 00 00 0p 0q FF
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x44;
    command[4] = 0x00;
    command[5] = 0x00;
    command[6] = (char)(pos / 16);
    command[7] = (char)(pos % 16);
    command[8] = 0xff;
//    addToQue(command , 9 , OSD);
//BGain removed
}
void MainWindow::osdMenuToggle()
{
    //81 01 04 3F 02 5F FF
    uint8_t command[7];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x3f;
    command[4] = 0x02;
    command[5] = 0x5f;
    command[6] = 0xff;
    addToQue(command , 7 , Command);

}
void MainWindow::osdMenuEnter()
{
    //81 01 06 06 05 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x06;
    command[4] = 0x05;
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::osdMenuBack()
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x06;
    command[4] = 0x04;
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}

void MainWindow::imageFlip()
{

    //81 01 04 66 02 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x66;
    if(ui->imgFlipBtn->isChecked())
        command[4] = 0x02;
    else
        command[4] = 0x03;
    command[5] = 0xff;
    addToQue(command, 6 , Command);

}
void MainWindow::imageMirror()
{
    //81 01 04 61 02 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x61;
    if(ui->imgMirrorBtn->isChecked())
        command[4] = 0x02;
    else
        command[4] = 0x03;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::irisUpClicked()
{
    //8x 01 04 0B 02 FF
//    uint8_t command[6];
//    command[0] = 0x81;
//    command[1] = 0x01;
//    command[2] = 0x04;
//    command[3] = 0x0b;
//    command[4] = 0x02;
//    command[5] = 0xff;
//    addToQue(command, 6 , Command);
    if(ui->irisCombo->currentIndex() == 12)
        ui->irisCombo->setCurrentIndex(12);
    else
        ui->irisCombo->setCurrentIndex(ui->irisCombo->currentIndex() +  1);
    return;
}
void MainWindow::irisDownClicked()
{
    //8x 01 04 0B 03 FF
//    uint8_t command[6];
//    command[0] = 0x81;
//    command[1] = 0x01;
//    command[2] = 0x04;
//    command[3] = 0x0b;
//    command[4] = 0x03;
//    command[5] = 0xff;
//    addToQue(command, 6 , Command);
    if(ui->irisCombo->currentIndex() == 0)
        ui->irisCombo->setCurrentIndex(0);
    else
        ui->irisCombo->setCurrentIndex(ui->irisCombo->currentIndex() -  1);
}
void MainWindow::irisResetClicked()
{
    //8x 01 04 0B 00 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x0b;
    command[4] = 0x00;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::irisComboChanged(int index)
{
    //81 01 04 4B 00 00 0p 0q FF
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x4b;
    command[4] = 0x00;
    command[5] = 0x00;
    command[6] = (char)(index / 16);
    command[7] = (char)(index % 16);
    command[8] = 0xff;
    addToQue(command , 9 , Command);
}
void MainWindow::shutterUpClicked()
{
    //81 01 04 0A 02 FF
//    uint8_t command[6];
//    command[0] = 0x81;
//    command[1] = 0x01;
//    command[2] = 0x04;
//    command[3] = 0x0a;
//    command[4] = 0x02;
//    command[5] = 0xff;
//    addToQue(command, 6 , Command);
    if(ui->shutterCombo->currentIndex() == 16)
        ui->shutterCombo->setCurrentIndex(16);
    else
        ui->shutterCombo->setCurrentIndex(ui->shutterCombo->currentIndex() + 1);

}
void MainWindow::shutterDownClicked()
{
    //81 01 04 0A 03 FF
//    uint8_t command[6];
//    command[0] = 0x81;
//    command[1] = 0x01;
//    command[2] = 0x04;
//    command[3] = 0x0a;
//    command[4] = 0x03;
//    command[5] = 0xff;
//    addToQue(command, 6 , Command);
    if(ui->shutterCombo->currentIndex() == 0)
        ui->shutterCombo->setCurrentIndex(0);
    else
        ui->shutterCombo->setCurrentIndex(ui->shutterCombo->currentIndex() - 1);
}
void MainWindow::shutterResetClicked()
{
    //81 01 04 0A 00 FF

    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x0a;
    command[4] = 0x00;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::shutterComboChanged(int index)
{
    //81 01 04 4A 00 00 0p 0q FF
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x4a;
    command[4] = 0x00;
    command[5] = 0x00;
    command[6] = (char)((index + 1) / 16);
    command[7] = (char)((index + 1) % 16);
    command[8] = 0xff;
    addToQue(command , 9 , Command);
}
void MainWindow::brightUpClicked()
{
    //8x 01 04 0D 02 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x0d;
    command[4] = 0x02;
    command[5] = 0xff;
    addToQue(command , 6 , Command);

}
void MainWindow::brightDownClicked()
{
    //8x 01 04 0D 03 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x0d;
    command[4] = 0x03;
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::brightResetClicked()
{
    //8x 01 04 0D 00 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x0d;
    command[4] = 0x00;
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::brightComboChanged(int index)
{
    //81 01 04 0d 00 00 0p 0q FF//only executed not affect
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x0d;
    command[4] = 0x00;
    command[5] = 0x00;
    command[6] = (char)(index / 16);
    command[7] = (char)(index % 16);
    command[8] = 0xff;
    addToQue(command , 9 , Command);

}
void MainWindow::aeModeSelected(int index)
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x39;

    switch(index)
    {
    case 0:
        command[4] = 0x00;
        break;
    case 1:
        command[4] = 0x03;
        break;
    case 2:
        command[4] = 0x0a;
        break;
    case 3:
        command[4] = 0x0b;
        break;
    case 4:
        command[4] = 0x0d;
        break;
    default:
        command[4] = 0x00;
        break;
    }
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::wbModeSelected(int index)
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    //    if(index == 5)//one push trigger
    //        command[3] = 0x20;
    //    else
    command[3] = 0x35;
    //    if(index > 3)
    //        command[4] = 0x05;
    //    else
    //        command[4] = (char)(index);
    command[4] = (char)(index);
    if(index == 4)
        command[4] = 0x05;
    if(index == 5)
        command[4] = 0x10;
    command[5] = 0xff;
    addToQue(command , 6 , Command);
}
void MainWindow::connectHost(QString ip)
{
    closeSocket();
    socket->connectToHost(ip, PORT_NUM);
    if (!socket->waitForConnected(5000))
    {
        QMessageBox msgBox;
        msgBox.setText("Can not connect to the specific camera. " + socket->errorString());
        msgBox.exec();
    }
}
void MainWindow::setPreset(int index)
{
    if (index == 10)//this should saved on camera...
    {
	uint8_t command[7];
        command[0] = 0x81;
        command[1] = 0x01;
        command[2] = 0x04;
        command[3] = 0x3f;
        command[4] = 0x01;
        command[5] = (char)(index % 255);
        command[6] = 0xff;
        addToQue(command, 7 , Command);
    }
    else
    {
	uint8_t command[7];
        command[0] = 0x81;
        command[1] = 0x01;
        command[2] = 0x04;
        command[3] = 0x3f;
        command[4] = 0x01;
        command[5] = (char)(index % 255);
        command[6] = 0xff;
        addToQue(command, 7 , Command);

    }

    returnRecallMode();
}
void MainWindow::recallPreset(int index)
{
    uint8_t command[7];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x3f;
    command[4] = 0x02;
    command[5] = (char)(index % 255);
    command[6] = 0xff;
    addToQue(command, 7 , Command);
}
void MainWindow::setAbsolutPanTilt(char *pan, char *tilt)
{
    uint8_t command[15];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x02;
    /*default value for speed*/
    command[4] = 0x05;
    command[5] = 0x05;
    /*default value for speed*/
    command[6] = pan[0];
    command[7] = pan[1];
    command[8] = pan[2];
    command[9] = pan[3];
    command[10] = tilt[0];
    command[11] = tilt[1];
    command[12] = tilt[2];
    command[13] = tilt[3];
    command[14] = 0xff;
//    addToQue(command, 15 , Command);
}
void MainWindow::setDirectZoomFocus(char *zoom, char *focus)
{
    uint8_t command[13];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x47;
    command[4] = zoom[0];
    command[5] = zoom[1];
    command[6] = zoom[2];
    command[7] = zoom[3];
    command[8] = focus[0];
    command[9] = focus[1];
    command[10] = focus[2];
    command[11] = focus[3];
    command[12] = 0xff;
//    addToQue(command, 13 , Command);
}
void MainWindow::setDirectZoom(char *zoom)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x47;
    command[4] = zoom[0];
    command[5] = zoom[1];
    command[6] = zoom[2];
    command[7] = zoom[3];
    //    command[8] = focus[0];
    //    command[9] = focus[1];
    //    command[10] = focus[2];
    //    command[11] = focus[3];
    command[8] = 0xff;
//    addToQue(command, 9 , Command);
}
void MainWindow::setDirectFocus(char *focus)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x48;
    command[4] = focus[0];
    command[5] = focus[1];
    command[6] = focus[2];
    command[7] = focus[3];
    command[8] = 0xff;
//    addToQue(command, 9 , Command);
}
void MainWindow::focusInOnce(int focusSpeed)
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x08;
    command[4] = 0x30 + focusSpeed;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::focusOutOnce(int focusSpeed)
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x08;
    command[4] = 0x20 + focusSpeed;
    command[5] = 0xff;
    addToQue(command, 6 , Command);

}
void MainWindow::stopFocusing()
{
    //8x 01 04 08 00 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x08;
    command[4] = 0x00;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::autoFocusChange(bool isChecked)
{
    ui->focusIn->setEnabled(!isChecked);
    ui->focusOut->setEnabled(!isChecked);
    //8x 01 04 38 02 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x38;
    if (isChecked)
        command[4] = 0x02;
    else
        command[4] = 0x03;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::angleUpLeft(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x01;
    command[7] = 0x01;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::angleUpRight(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x02;
    command[7] = 0x01;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::angleDownLeft(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x01;
    command[7] = 0x02;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::angleDownRight(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x02;
    command[7] = 0x02;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::osdMenuChange(int index)
{
    //8x 01 06 06 03 FF
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x06;
    if (index == 0)
        command[4] = 0x03;
    if (index == 1)
        command[4] = 0x03;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::moveTopOnce(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x03;
    command[7] = 0x01;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::moveLeftOnce(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x01;
    command[7] = 0x03;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::moveBottomOnce(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x03;
    command[7] = 0x02;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::moveRightOnce(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x02;
    command[7] = 0x03;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::moveHome()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x04;
    command[4] = 0xff;
    addToQue(command, 5 , Command);
}
void MainWindow::zoomInOnce(int zoomSpeed)
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x07;
    command[4] = 0x20 + zoomSpeed;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::zoomOutOnce(int zoomSpeed)
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x07;
    command[4] = 0x30 + zoomSpeed;
    command[5] = 0xff;
    addToQue(command, 6 , Command);
}
void MainWindow::stopMoving(int panSpeed, int tiltSpeed)
{
    uint8_t command[9];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x06;
    command[3] = 0x01;
    command[4] = panSpeed % 255;
    command[5] = tiltSpeed % 255;
    command[6] = 0x03;
    command[7] = 0x03;
    command[8] = 0xff;
    addToQue(command, 9 , Command);
}
void MainWindow::stopZooming()
{
    uint8_t command[6];
    command[0] = 0x81;
    command[1] = 0x01;
    command[2] = 0x04;
    command[3] = 0x07;
    command[4] = 0x00;
    command[5] = 0xff;
    addToQue(command, 6 , Command);

}
/*remote control functions end*/

/*camera query functions begin*/

void MainWindow::loadCameraInfo()
{
//    isControllable = false;
    if (isControllable)
    {
        //to load camera info like following
        //you should call one after former one finishes
        //now we calls loadcamerainfo per 1 sec
        //then if one query is not finshed in 1 sec, the query repeats.
        //so we insert all queries into que per 10 secs
        //we should getstatusStep ++ here after each command adding
        //if we use bulk queries, executing commands will be delayed
        //        qDebug()<<getStatusStep;
        switch(getStatusStep)
        {
        case 0:
            getZoomInfo();
            getStatusStep ++;
            break;
        case 1:
            getFocusInfo();
            getStatusStep ++;
            break;
        case 2:
            getAFInfo();
            getStatusStep ++;
            break;
        case 3:
            getShutterInfo();
            getStatusStep ++;
            break;
        case 4:
            getIrisInfo();
            getStatusStep ++;
            break;
        case 5:
            getBrightInfo();
            getStatusStep ++;
            break;
        case 6:
            getPanTiltInfo();
            getStatusStep ++;
            break;
        case 7:
            getLuminanceInfo();
            getStatusStep ++;
            break;
        case 8:
            getContrastInfo();
            getStatusStep ++;
            break;
        case 9:
            getHueInfo();
            getStatusStep ++;
            break;
        case 10:
            getAeMode();
            getStatusStep ++;
            break;
        case 11:
            getWBMode();
            getStatusStep ++;
            break;
        case 12:
            getRGain();
            getStatusStep ++;
            break;
        case 13:
            getBGain();
            getStatusStep = 0;
            break;
        default:
            break;
        }
        //        getZoomInfo();
        //        getFocusInfo();
        //        getAFInfo();
        //        getShutterInfo();
        //        getIrisInfo();
        //        getBrightInfo();
        //        getPanTiltInfo();
        //        getLuminanceInfo();
        //        getContrastInfo();
        //        getHueInfo();

        //        getAeMode();
        //        getWBMode();
        //        getRGain();
        //        getBGain();
    }
//    isControllable = true;
}
void MainWindow::getZoomInfo()
{
    //    getStatusStep = 0;
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x47;
    command[4] = 0xff;
    addToQue(command, 5 , GetZoomInfo);
}
void MainWindow::getFocusInfo()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x48;
    command[4] = 0xff;
    addToQue(command, 5 , GetFocusInfo);
}
void MainWindow::getAFInfo()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x38;
    command[4] = 0xff;
    addToQue(command, 5 , GetAFInfo);
}
void MainWindow::getZoomFocusInfo()
{
    uint8_t command[6];
    //8x 09 7E 7E 00 FF
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x7e;
    command[3] = 0x7e;
    command[4] = 0x00;
    command[5] = 0xff;
    //    addToQue(command , 6 , GetZo);
}


void MainWindow::getShutterInfo()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x4a;
    command[4] = 0xff;
    addToQue(command, 5 , GetShutterInfo);
}
void MainWindow::getIrisInfo()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x4b;
    command[4] = 0xff;
    addToQue(command, 5 , GetIrisInfo);
}
void MainWindow::getShutterIrisInfo()
{
    uint8_t command[6];
    //8x 09 7E 7E 01 FF
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x7e;
    command[3] = 0x7e;
    command[4] = 0x01;
    command[5] = 0xff;
    //    addToQue(command , 6 , Ge);

}
void MainWindow::getBrightInfo()
{
    //8x 09 04 0D FF
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x4d;
    command[4] = 0xff;
    addToQue(command , 5 , GetBrightInfo);
}
void MainWindow::getPanTiltInfo()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x06;
    command[3] = 0x12;
    command[4] = 0xff;
    addToQue(command, 5 , GetPanTiltInfo);
}
void MainWindow::getLuminanceInfo()
{
    //81 09 04 A1 FF
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0xa1;
    command[4] = 0xff;
    addToQue(command , 5 , GetLuminanceInfo);
}
void MainWindow::getContrastInfo()
{
    //81 09 04 A2 FF
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0xa2;
    command[4] = 0xff;
    addToQue(command , 5 , GetContrastInfo);
}
void MainWindow::getHueInfo()
{
    //81 09 04 4F FF
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x4f;
    command[4] = 0xff;
    addToQue(command , 5 , GetHueInfo);
}
void MainWindow::getAeMode()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x39;
    command[4] = 0xff;
    addToQue(command , 5 , GetAEMode);
}
void MainWindow::getWBMode()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x35;
    command[4] = 0xff;
    addToQue(command , 5 , GetWBMode);
}
void MainWindow::getRGain()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x43;
    command[4] = 0xff;
    addToQue(command , 5 , GetRGain);
}
void MainWindow::getBGain()
{
    uint8_t command[5];
    command[0] = 0x81;
    command[1] = 0x09;
    command[2] = 0x04;
    command[3] = 0x44;
    command[4] = 0xff;
    addToQue(command , 5 , GetBGain);
}
void MainWindow::resetCamera()
{
    if(isControllable)
    {
        //8x 01 04 00 03 FF off
	uint8_t offCommand[6];
        offCommand[0] = 0x81;
        offCommand[1] = 0x01;
        offCommand[2] = 0x04;
        offCommand[3] = 0x00;
        offCommand[4] = 0x03;
        offCommand[5] = 0xff;
        addToQue(offCommand , 6 , Command);
        //8x 01 04 00 02 FF on
	uint8_t onCommand[6];
        onCommand[0] = 0x81;
        onCommand[1] = 0x01;
        onCommand[2] = 0x04;
        onCommand[3] = 0x00;
        onCommand[4] = 0x02;
        onCommand[5] = 0xff;
        addToQue(onCommand , 6 , Command);
    }
}
/*camera query functions end*/
void MainWindow::addToQue(uint8_t *command, int byteSize , COMMAND_TYPE type)
{
    COMMAND *com = new COMMAND;
    com->comandLength = byteSize;
    for(int i = 0 ; i < byteSize ; i ++)
    {
        com->commandByte[i] = (char) command[i];
    }
//    delete command;
    com->commandType = type;
    //    if(type == OSD)
    //    {
    //        executeCommand(com);
    //        //we should insert delay here for color gain

    //    }
    //    else
    //    {

    if(com->commandType == Command)
        executeCommand(com);
    else
    {
        commandQue.append(com);
        //    if(commandQue.size() == 0 || commandQue.size() == 1)
        //    {
        //        commandQue.append(com);
        //    }
        //    else
        //    {
        //        bool inserted = false;
        //        for(int i = 1; i < commandQue.size() ; i ++)
        //        {
        //            if(commandQue.at(i)->commandType != Command)
        //            {
        //                commandQue.insert(i , com);
        //                inserted = true;
        //                break;
        //            }
        //        }
        //        if(!inserted)
        //            commandQue.append(com);
        //    }

        if(commandQue.size() == 1)
        {
            executeCommand(commandQue.at(0));
        }
    }
}
void MainWindow::executeCommand(COMMAND *com)
{

    if(com->commandType != Command)
        currentCommandType = com->commandType;
    else
        log("----Command----" + QString(com->commandByte));
    sendPacket(com->commandByte , com->comandLength);
//    if(com->commandType != Command)
//    {
//        QTimer *singleShot = new QTimer(this);
//        singleShot->setSingleShot(true);
//        auto delay_100 = [this]{
//            this->moveToNextCommand();
//        };
//        connect(singleShot , &QTimer::timeout , delay_100);
//        singleShot->start(500);
//    }
}
bool MainWindow::sendPacket(char *bytes, int bytesize)
{
    QByteArray temp(bytes , bytesize);
    log(QString(temp.toHex())+","+QString::number(strlen(bytes))+","+QString::number(bytesize));
    socket->write(bytes, bytesize);
    return false;
}

void MainWindow::loadMultiCameraNames()
{
    ui->connectCam1->setText(settings->value(DEFAULT_CAMERA1_NAME_KEY, DEFAULT_CAMERA1_NAME).toString());
    ui->connectCam2->setText(settings->value(DEFAULT_CAMERA2_NAME_KEY, DEFAULT_CAMERA2_NAME).toString());
    ui->connectCam3->setText(settings->value(DEFAULT_CAMERA3_NAME_KEY, DEFAULT_CAMERA3_NAME).toString());
    ui->connectCam4->setText(settings->value(DEFAULT_CAMERA4_NAME_KEY, DEFAULT_CAMERA4_NAME).toString());
    ui->connectCam5->setText(settings->value(DEFAULT_CAMERA5_NAME_KEY, DEFAULT_CAMERA5_NAME).toString());
    ui->connectCam6->setText(settings->value(DEFAULT_CAMERA6_NAME_KEY, DEFAULT_CAMERA6_NAME).toString());
    ui->connectCam7->setText(settings->value(DEFAULT_CAMERA7_NAME_KEY, DEFAULT_CAMERA7_NAME).toString());
    ui->connectCam8->setText(settings->value(DEFAULT_CAMERA8_NAME_KEY, DEFAULT_CAMERA8_NAME).toString());
}

void MainWindow::showMultiCameraSetting()
{
    MultiCameras multiCameras;
    multiCameras.setWindowTitle("Multi Camera Setting");
    multiCameras.exec();
    loadMultiCameraNames();
}
void MainWindow::showJoyStickSetting()
{
    JoyStickDlg joydlg;
    joydlg.setWindowTitle("Additional Input Settings");
    joydlg.exec();
}
void MainWindow::showAboutUs()
{
    AboutUs aboutUs;
    aboutUs.setWindowTitle("About Us");
    aboutUs.exec();
}
void MainWindow::loadPresetTitleByCamera(QString ip)
{
    ui->mem1->setText(settings->value("mem1" + ip, "1").toString());
    ui->mem2->setText(settings->value("mem2" + ip, "2").toString());
    ui->mem3->setText(settings->value("mem3" + ip, "3").toString());
    ui->mem4->setText(settings->value("mem4" + ip, "4").toString());
    ui->mem5->setText(settings->value("mem5" + ip, "5").toString());
    ui->mem6->setText(settings->value("mem6" + ip, "6").toString());
    ui->mem7->setText(settings->value("mem7" + ip, "7").toString());
    ui->mem8->setText(settings->value("mem8" + ip, "8").toString());
    ui->mem9->setText(settings->value("mem9" + ip, "9").toString());

    //set button images
    ui->mem1->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + ip + "1.jpg');\n}");
    ui->mem2->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + ip + "2.jpg');\n}");
    ui->mem3->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + ip + "3.jpg');\n}");
    ui->mem4->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + ip + "4.jpg');\n}");
    ui->mem5->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + ip + "5.jpg');\n}");
    ui->mem6->setStyleSheet("QPushButton{\n	height : 40px;\n	image : url('" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString(DOWNLOAD_FILE_DEST_PREFIX) + ip + "6.jpg');\n}");
}
void MainWindow::showCompactView()
{
    //main ui components
    //    ui->goButton->hide();
    //    ui->morePresetLabel->hide();
    //    ui->morePresets->hide();
    if(getCurrentWinStatus() == ADVANCED_VIEW)
    {
//        ui->optionsGroup->hide();
//        ui->exposureGroup->hide();
//        ui->imageGroup->hide();
//        ui->colorGroup->hide();
//        ui->osdGroup->hide();

        ui->rightPanel->hide();
        //status bar components
//        shutterStatus->hide();
//        irisStatus->hide();
//        brightStatus->hide();
//        luminanceStatus->hide();
//        contrastStatus->hide();
//        hueStatus->hide();


        this->setWindowSize(this->width() - initialWidth / 2 , this->height());
        setCurrentWinStatus(COMPACT_VIEW);
    }

}
void MainWindow::setWindowSize(int w_width , int w_height)
{
//    this->setFixedSize(/*centerW - w_width / 2, centerH - w_height / 2 ,*/ w_width , w_height);
//    this->setMaximumSize(QSize(w_width , w_height));
//    qDebug()<<w_width;
    adjustSize();
//    this->setFixedSize(w_width , w_height);
//    this->setSizePolicy(QSizePolicy::Preferred , QSizePolicy::Preferred);
}
void MainWindow::showAdvancedView()
{
    //main ui components
    //    ui->goButton->show();
    //    ui->morePresetLabel->show();
    //    ui->morePresets->show();
    if(getCurrentWinStatus() == COMPACT_VIEW)
    {
//        ui->optionsGroup->show();
//        ui->exposureGroup->show();
//        ui->imageGroup->show();
//        ui->colorGroup->show();
//        ui->osdGroup->show();

        ui->rightPanel->show();
        //status bar components
//        shutterStatus->show();
//        irisStatus->show();
//        brightStatus->show();
//        luminanceStatus->show();
//        contrastStatus->show();
//        hueStatus->show();


        this->setWindowSize(this->width() + initialWidth / 2 , this->height());
        setCurrentWinStatus(ADVANCED_VIEW);
    }
}
int MainWindow::getCurrentWinStatus()
{
    return this->currentWinStatus;
}
void MainWindow::setCurrentWinStatus(int winStatus)
{
    this->currentWinStatus = winStatus;
}
char *readBuffer = new char[MAX_BUFFER_SIZE];
int currentPos = -1;//current byte position -1:did not find the preamble

void MainWindow::readBytes()
{
    QByteArray temp;
    temp = socket->readAll();
    log(QString(temp.toHex())+"---"+QString::number(temp.length()));
    if (temp.size() > 0)
    {
        for (int i = 0; i < temp.size(); i++)
        {

            if (currentPos > -1)//reading one command.
            {
                //for the correct solution it must not send FF as data byte
                if (temp.at(i) == (char)0xff && ((i == temp.size() - 1) || (temp.at(i + 1) == (char)0x90)))//you must fix this part(this is not correct when packets are coming by one byte
                {
                    currentPos++;
                    readBuffer[currentPos] = temp.at(i);
                    analyzeBuffer(readBuffer, (currentPos + 1));
                    //initialize new buffer
                    currentPos = -1;
                    readBuffer = new char[MAX_BUFFER_SIZE];
                }
                else
                {
                    currentPos++;
                    if (currentPos > ((MAX_BUFFER_SIZE) - 1))//unknown error
                    {
                        currentPos = -1;
                        readBuffer = new char[MAX_BUFFER_SIZE];
                    }
                    else
                    {
                        readBuffer[currentPos] = temp.at(i);
                    }
                }
            }
            else//finding the preamble
            {
                if (temp.at(i) == (char)0x90)//8 + 1 = 9
                {
                    currentPos = 0;
                    readBuffer[currentPos] = temp.at(i);
                }
            }
        }
    }
}
void MainWindow::closeSocket()
{
    if(isControllable)
    {
        //setPreset(10);
        socket->close();
    }
}
void MainWindow::analyzeBuffer(char *buffer, int bufSize)
{
    if(buffer[0] == (char)(0x90) && (buffer[1]>>4) == (char)(0x06))
    {
        log("error");
        moveToNextCommand();
    }
    else
    {
        log(QString::number(bufSize));
        switch(currentCommandType)
        {
     /*   case Command:
            //                if((buffer[1]>>4) == (char)(0x04))//ack:we should wait for complete msg;
            //                    return;
            if(buffer[1]>>4 == (char(0x04)))
            {
                log("Command");
                moveToNextCommand();
            }//when call presets, completion packet is optional...

            break;//this is used when command and query que is the same....*/
        case GetZoomInfo:
            if(bufSize != 3)
            {
                zoomLevel[0] = (char)buffer[2];
                zoomLevel[1] = (char)buffer[3];
                zoomLevel[2] = (char)buffer[4];
                zoomLevel[3] = (char)buffer[5];
                log("GetZoomInfo");
                moveToNextCommand();
            }
            break;
        case GetFocusInfo:
            if(bufSize != 3)
            {
                focusLevel[0] = (char)buffer[2];
                focusLevel[1] = (char)buffer[3];
                focusLevel[2] = (char)buffer[4];
                focusLevel[3] = (char)buffer[5];
                log("GetFocusInfo");
                moveToNextCommand();
            }
            break;
        case GetAFInfo:
            if(bufSize != 3)
            {
                if (((char)buffer[2]) == (char)(0x02))
                    focusMode = true;
                else
                    focusMode = false;
                log("GetAFInfo");
                moveToNextCommand();
            }
            break;
        case GetShutterInfo:
            if(bufSize != 3)
            {
                if(bufSize == 4)
                {
                    shutterLevel = (char)buffer[2];
                }
                else if(bufSize == 7)
                {
                    shutterLevel = (char)(buffer[4]<<4) + (char)buffer[5];
                }
                log("GetShutterInfo");
                moveToNextCommand();
            }
            break;
        case GetIrisInfo:
            if(bufSize != 3)
            {
                if(bufSize == 4)
                {
                    irisLevel = (char)buffer[2];
                }
                else if(bufSize == 7)
                {
                    irisLevel = (char)(buffer[4]<<4) + (char)buffer[5];
                }

                log("GetIrisInfo");
                moveToNextCommand();
            }
            break;
        case GetBrightInfo:
            if(bufSize != 3)
            {
                brightLevel = (char)buffer[2];

                moveToNextCommand();
                log("GetBrightInfo");
            }
            break;
        case GetPanTiltInfo:
            if(bufSize != 3)
            {
                panPos[0] = buffer[2];
                panPos[1] = buffer[3];
                panPos[2] = buffer[4];
                panPos[3] = buffer[5];

                tiltPos[0] = buffer[6];
                tiltPos[1] = buffer[7];
                tiltPos[2] = buffer[8];
                tiltPos[3] = buffer[9];
                log("GetPanTiltInfo");
                moveToNextCommand();
            }
            break;
        case GetLuminanceInfo:
            if(bufSize != 3)
            {
                if(bufSize == 4)
                {
                    luminanceLevel = (char)buffer[2];
                }
                else if(bufSize == 7)
                {
                    luminanceLevel = (char)buffer[5];
                }
                log("GetLuminanceInfo");
                moveToNextCommand();
            }
            break;
        case GetContrastInfo:
            if(bufSize != 3)
            {
                if(bufSize == 4)
                {
                    contrastLevel = (char)buffer[2];
                }
                else if(bufSize == 7)
                {
                    contrastLevel = (char)buffer[5];
                }
                log("GetContrastInfo");
                moveToNextCommand();
            }
            break;
        case GetHueInfo:
            if(bufSize != 3)
            {
                if(bufSize == 4)
                {
                    hueLevel = (char)buffer[2];
                }
                else if(bufSize == 7)
                {
                    hueLevel = (char)buffer[5];
                }
                log("GetHueInfo");
                moveToNextCommand();
            }
            break;

        case GetAEMode:
            if(bufSize != 3)
            {
                aeMode = (char)buffer[2];
                log("GetAeMode");
                moveToNextCommand();
            }
            break;

        case GetWBMode:
            if(bufSize != 3)
            {
                wbMode = (char)buffer[2];
                log("GetWBMode");
                moveToNextCommand();
            }
            break;

        case GetRGain:
            if(bufSize != 3)
            {
                rGain = (char)buffer[2];
                log("GetRGain");
                moveToNextCommand();
            }
            break;

        case GetBGain:
            if(bufSize != 3)
            {
                bGain = (char)buffer[2];
                log("GetBGain");
                moveToNextCommand();
                if(startFlag == 0)
                    startFlag = 1;
            }
            break;
        default:
            break;
        }
        displayStatus();
    }

}
void MainWindow::moveToNextCommand()
{

    if(commandQue.size() > 0)
    {
        COMMAND* firstCom = commandQue.at(0);
        commandQue.removeAt(0);
        delete firstCom;
    }
    //    qDebug()<<commandQue.size();
    if(commandQue.size() > 0)
    {
        executeCommand(commandQue.at(0));
    }
}
void MainWindow::displayStatus()
{
    //status...
//    panStatus->setText(QString::asprintf("Pan:%x%x%x%x", panPos[0], panPos[1], panPos[2], panPos[3]));
//    tiltStatus->setText(QString::asprintf("Tilt:%x%x%x%x", tiltPos[0], tiltPos[1], tiltPos[2], tiltPos[3]));
//    zoomStatus->setText(QString::asprintf("Zoom:%x%x%x%x", zoomLevel[0], zoomLevel[1], zoomLevel[2], zoomLevel[3]));
//    focusStatus->setText(QString::asprintf("Focus:%x%x%x%x", focusLevel[0], focusLevel[1], focusLevel[2], focusLevel[3]));
//    irisStatus->setText("Iris:" + irises[(unsigned char)irisLevel]);
    if(shutterLevel == 0)
        shutterLevel = 1;
//    shutterStatus->setText("Shutter:" + shutters[(unsigned char)shutterLevel - 1]);//we should set shutterlevel's initiative value as 1
//    brightStatus->setText("Bright:" + QString::number((unsigned char)brightLevel));
//    luminanceStatus->setText("Luminance:" + QString::number((unsigned char)luminanceLevel));
//    contrastStatus->setText("Contrast:" + QString::number((unsigned char)contrastLevel));
//    hueStatus->setText("Hue:" + QString::number((unsigned char)hueLevel));
//    rGainStatus->setText("R Gain:" + QString::number((unsigned char)rGain));
//    bGainStatus->setText("B Gain:" + QString::number((unsigned char)bGain));

    //set the initial positions of the ui
    if(startFlag == 1)
    {
        ui->hueSlider->setValue((unsigned char)hueLevel);
        ui->contrastSlider->setValue(/*(*/(unsigned char)contrastLevel/* % 16) * 10 + contrastLevel[0] % 16*/);
        ui->luminanceSlider->setValue(/*(*/(unsigned char)luminanceLevel /*% 16) * 10 + luminanceLevel[0] % 16*/);
//        ui->rGainSlider->setValue((unsigned char)rGain);
//        ui->bGainSlider->setValue((unsigned char)bGain);
        //00 03 0A 0B 0D
        switch(aeMode)
        {
        case 0x00:
            ui->aeModeSelect->setCurrentIndex(0);
            break;
        case 0x03:
            ui->aeModeSelect->setCurrentIndex(1);
            break;
        case 0x0a:
            ui->aeModeSelect->setCurrentIndex(2);
            break;
        case 0x0b:
            ui->aeModeSelect->setCurrentIndex(3);
            break;
        case 0x0d:
            ui->aeModeSelect->setCurrentIndex(4);
            break;
        default:
            break;
        }

        ui->shutterCombo->setCurrentIndex((unsigned char)shutterLevel - 1);
        ui->irisCombo->setCurrentIndex((unsigned char)irisLevel);
        ui->brightCombo->setCurrentIndex((unsigned char)brightLevel);
        switch(wbMode)
        {
        case 0x05:
            ui->wbCombo->setCurrentIndex(4);
        case 0x03:
        case 0x02:
        case 0x01:
        case 0x00:
            ui->wbCombo->setCurrentIndex((unsigned char)wbMode);
            break;
        default:
            break;
        }



        ui->autoFocus->setChecked(focusMode);

        ui->focusIn->setEnabled(!focusMode);
        ui->focusOut->setEnabled(!focusMode);

        startFlag = 2;
        //        ui->receiveLog->append("startFlag");
    }
}


bool MainWindow::log(QString str)
{
    QFile *file = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ptzoptics-controller/app.log");
//    qDebug()<<fileDir + "/app.log";
    if(file->open(QFile::Append))
    {
        str.append('\n');
        file->write(str.toUtf8());

        file->flush();
        file->close();
        return true;
    }
    delete file;
    return false;
}
void MainWindow::onSwitchCamera(int cameraNum)
{
    switch(cameraNum)
    {
    case 1:
        onCam1Clicked();
        break;
    case 2:
        onCam2Clicked();
        break;
    case 3:
        onCam3Clicked();
        break;
    case 4:
        onCam4Clicked();
        break;
    }
}
void MainWindow::onCallPreset(int presetNum)
{
    if(isControllable)
    {
        recallPreset(presetNum);
        loadPresetSettings(presetNum , 0);
    }
}
void MainWindow::returnRecallMode()
{
    isPresetMode = false;
    ui->recallMode->setChecked(true);
}
QSettings *MainWindow::getAppSettings()
{
    return settings;
}
void MainWindow::sendDevNameRequest()
{
    if(isControllable)
        devNameGetter->get(QNetworkRequest(QUrl("http://" + currentCamIp + "/cgi-bin/param.cgi?get_device_conf")));
}
void MainWindow::onGetName(QNetworkReply *reply)
{
    if(reply->error())
    {
        reply->deleteLater();
        return;
    }
    QString devName = processReply(reply->readAll());
    devNameLabel->setText(devName.replace("\"", ""));
}
QString MainWindow::processReply(QString str)
{
    QString temp = str;
    QStringList strList = temp.split('\n');
    if(strList.length() > 0)
    {
        temp = strList.at(0);
        if(temp.length() > 10)
            temp = temp.mid(10);
    }
    return temp;
}

