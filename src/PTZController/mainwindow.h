#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QSettings>
#include <QKeyEvent>
#include "constants.h"
#include <QList>
#include "joystickwrapper.h"
#include <QSlider>
namespace Ui {
class MainWindow;
}
typedef enum COMMAND_TYPE{
    Command = 0,//ack, completion
    Preset = 1,//ack, completion is optional//treated as Command
    OSD = 2,//no return value//treated as Command
    GetZoomInfo = 3,
    GetFocusInfo = 4,
    GetAFInfo = 5,
    GetShutterInfo = 6,
    GetIrisInfo = 7,
    GetBrightInfo = 8,
    GetPanTiltInfo = 9,
    GetLuminanceInfo = 10,
    GetContrastInfo = 11,
    GetHueInfo = 12,
    GetAEMode = 13,
    GetWBMode = 14,
    GetRGain = 15,
    GetBGain = 16
}COMMAND_TYPE;
typedef struct COMMAND{
    char commandByte[16];
    int comandLength;
    COMMAND_TYPE commandType;
}COMMAND;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int getCurrentWinStatus();
    void setCurrentWinStatus(int winStatus);
    void showCompactView();
    void showAdvancedView();
    void setWindowSize(int w_width , int w_height);
    QString getCurrentCameraIp();
    bool getIsControllable();
    QSettings *getAppSettings();

//protected:
//    void keyPressEvent(QKeyEvent *ev);
//    void keyReleaseEvent(QKeyEvent *ev);

private:

    QTcpSocket *socket;
    QString settingsFile;
    QSettings *settings;
    JoystickWrapper *joystick;

    //for download
//    QNetworkRequest *request;
    QNetworkAccessManager *downLoadManager;
    QNetworkAccessManager *devNameGetter;

    //command que
    QList<COMMAND*> commandQue;
    QTimer *timer;

    bool isUserClosed = true;

    bool isControllable = false;

    bool isPresetMode = false;//newly changed

    int currentWinStatus = ADVANCED_VIEW;//0 means compact view and 1 means advanced view



    void connectHost(QString ip);
    void moveTopOnce(int panSpeed , int tiltSpeed);
    void moveLeftOnce(int panSpeed , int tiltSpeed);
    void moveBottomOnce(int panSpeed , int tiltSpeed);
    void moveRightOnce(int panSpeed , int tiltSpeed);
    void stopMoving(int panSpeed , int tiltSpeed);
    void stopZooming();
    void moveHome();
    void zoomInOnce(int zoomSpeed);
    void zoomOutOnce(int zoomSpeed);
    void osdMenuChange(int index);
    void angleUpLeft(int panSpeed , int tiltSpeed);
    void angleUpRight(int panSpeed , int tiltSpeed);
    void angleDownRight(int panSpeed , int tiltSpeed);
    void angleDownLeft(int panSpeed , int tiltSpeed);
    void autoFocusChange(bool isChecked);
    void focusInOnce(int focusSpeed);
    void focusOutOnce(int focusSpeed);
    void stopFocusing();
    void setPreset(int index);
    void recallPreset(int index);
    void showAboutUs();
    void showMultiCameraSetting();
    void showJoyStickSetting();

    void irisUpClicked();
    void irisDownClicked();
    void irisResetClicked();
    void irisComboChanged(int index);

    void shutterUpClicked();
    void shutterDownClicked();
    void shutterResetClicked();
    void shutterComboChanged(int index);

    void brightUpClicked();
    void brightDownClicked();
    void brightResetClicked();
    void brightComboChanged(int index);

    void aeModeSelected(int index);
    void wbModeSelected(int index);

    bool sendPacket(char *bytes , int bytesize);

    void loadCameraInfo();
    /*camera query functions begin*/
    void getZoomInfo();
    void getFocusInfo();
    void getAFInfo();
    void getZoomFocusInfo();

    void getShutterInfo();
    void getIrisInfo();
    void getBrightInfo();
    void getShutterIrisInfo();
    void getPanTiltInfo();
    void getLuminanceInfo();
    void getContrastInfo();
    void getHueInfo();
    void getAeMode();
    void getWBMode();
    void getRGain();
    void getBGain();
    /*camera query functions end*/

    /*camera status variables begin*/
    char panPos[4];
    char tiltPos[4];
    char zoomLevel[4];
    char focusLevel[4];
    bool focusMode;//true:auto , false:manual

    char irisLevel;
    char shutterLevel;
    char brightLevel;
    char aeMode;


    char luminanceLevel;
    char contrastLevel;
    char hueLevel;

    char wbMode;
    char rGain;
    char bGain;
    /*camera status variables end*/

    void setSliderValue(QSlider *slider , bool increase);

    void readBytes();
    void analyzeBuffer(char *buf , int bufSize);

    void displayStatus();

    void setAbsolutPanTilt(char *pan , char *tilt);
    void setDirectZoomFocus(char *zoom , char *focus);
    void setDirectZoom(char *zoom);
    void setDirectFocus(char *focus);

    void setLuminance(int pos);
    void setContrast(int pos);
    void setHue(int pos);

    void setRGain(int pos);
    void setBGain(int pos);

    void setSharpness(int pos);
    void setSaturation(int pos);

    void osdMenuToggle();
    void osdMenuEnter();
    void osdMenuBack();

    void imageFlip();
    void imageMirror();

    void loadMultiCameraNames();

    void setDefaultColor();

    void loadPresetTitleByCamera(QString ip);

    void closeSocket();

    void resetCamera();

    void presetSpeedSet(int index);

    bool isZoomBySpeed = false;//true : zoom by speed , false : static zoom speed;

    void speedByZoom();

    //newly added 08/19
public:
    void focusLock(bool lock);
    void setPanTiltLimit(int panLeftLimit, int panRightLimit, int tiltDownLimit, int tiltUpLimit);
    void clearLimits(bool upright);
    void setCallPresetSpeed(int speed);
private:
    void initConnection();

    void statusBarToggle();

    //newly added

    void setSettings(QString curIp, QString settingKey , QVariant value);
    void loadSettings(QString curIp);

    //newly added
    void savePresetSettings(int presetNum, QString presetText);
    void loadPresetSettings(int presetNum , int loadingStep);

    //newly added
public:
    void showMessage(QString str);
private:
    //newly added
    void addToQue(char* command , int byteSize , COMMAND_TYPE type);
    void moveToNextCommand();
    void executeCommand(COMMAND* com);
public slots:

    void autoFocusClick();
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

    void onFocusIn();

    void onFocusOut();

    void onStopMoving();

    void onStopZooming();

    void onStopFocusing();

    void onOSDMenuChanged(int index);

    void onAngleUpLeft();

    void onAngleUpRight();

    void onAngleDownRight();

    void onAngleDownLeft();

    void onAutoFocusChanged();

    void onPresetModeSet();

    void onRecallModeSet();

    void onMem1Clicked();
    void onMem2Clicked();
    void onMem3Clicked();
    void onMem4Clicked();
    void onMem5Clicked();
    void onMem6Clicked();

    void onMem7Clicked();
    void onMem8Clicked();
    void onMem9Clicked();
//    void onMem10Clicked();

    void onCam1Clicked();
    void onCam2Clicked();
    void onCam3Clicked();
    void onCam4Clicked();
    void onCam5Clicked();
    void onCam6Clicked();
    void onCam7Clicked();
    void onCam8Clicked();

    void onIrisUpClicked();
    void onIrisDownClicked();
    void onIrisResetClicked();
    void onIrisComboChanged(int index);

    void onShutterUpClicked();
    void onShutterDownClicked();
    void onShutterResetClicked();
    void onShutterComboChanged(int index);

    void onBrightUpClicked();
    void onBrightDownClicked();
    void onBrightResetClicked();
    void onBrightComboChanged(int index);

    void onAeModeSelected(int index);
    void onWBModeSelected(int index);

    void onPanTiltLockClicked();
    void onPanSpeedChanged(int index);
    void onTiltSpeedChanged(int index);
    //newly added
    void onZoomSpeedChanged(int index);
    void onFocusSpeedChanged(int index);

    void onLuminanceSliderMoved(int pos);
    void onContrastSliderMoved(int pos);
    void onHueSliderMoved(int pos);
    void onSharpnessMoved(int pos);
    void onSaturationMoved(int pos);

    void onOsdMenuClicked();
    void onOsdEnterClicked();
    void onOsdBackClicked();

    void onImageFlip();
    void onImageMirror();

    void onAboutUsClicked();
    //newly added 08/19
    void onPanTiltLimitSettingClicked();
    void onXboxHelpClicked();
    void onHotkeyHelpClicked();
    void returnRecallMode();

    void onMultiCameraClicked();

    void onReadyRead();

    void onTimer();

    void onResetBtnClicked();

    void onPresetSpeedSet(int index);

    void onSpeedByZoomClicked();

    void onFocusLockClicked();

    void onMorePresetBtnClicked();

    void onCompactViewClicked();
    void onAdvancedViewClicked();

    void onFileDownloaded(QNetworkReply *reply);

    void onStatusBarToggle();

    void onWriteFinish();

    void onRGainSliderMoved(int pos);

    void onBGainSliderMoved(int pos);

    void onLowLightBtnClicked();
    void onSportsDayBtnClicked();
    void onSportsNightBtnClicked();
    void onOnePushTriggerBtnClicked();

    void onJoyStickClicked();
    void onLuminanceChangeTriggered(bool increase);
    void onContrastChangeTriggered(bool increase);
    void onHueChangeTriggered(bool increase);
    void onCallPreset(int presetNum);
    void onSwitchCamera(int cameraNum);
    //newly added: 07/2019
    void onGetName(QNetworkReply *reply);
    QString processReply(QString str);

private:
    Ui::MainWindow *ui;
    COMMAND_TYPE currentCommandType = Command;
    bool log(QString str);
    //newly added: 07/2019
    void sendDevNameRequest();

};

#endif // MAINWINDOW_H
