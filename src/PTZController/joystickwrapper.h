#ifndef JOYSTICKWRAPPER_H
#define JOYSTICKWRAPPER_H

#include <QObject>
#include <QJoysticks.h>
#include <QSettings>
#define BTN_START 6//9
#define BTN_BACK 4//8
#define BTN_LB 9//4
#define BTN_LT 15//6
#define BTN_RB 10//5
#define BTN_RT 16//7
#define BTN_X 2//3
#define BTN_Y 3//0
#define BTN_A 0//2
#define BTN_B 1//1
#define BTN_TL 7//10
#define BTN_TR 8//11
#define DPAD_UP 11
#define DPAD_DOWN 12
#define DPAD_LEFT 13
#define DPAD_RIGHT 14
typedef enum AXISMOVE{
    STOPMOVING =0,
    MOVEUP,
    MOVEDOWN,
    MOVELEFT,
    MOVERIGHT,
} AXISMOVE;
class JoystickWrapper : public QObject
{
    Q_OBJECT
public:
    explicit JoystickWrapper(QObject *parent = nullptr);
    ~JoystickWrapper();
    QStringList getJoystickNames();
private:
    AXISMOVE lastMove = STOPMOVING;
    int currentJoystickId;
    bool isJoystickEnabled;
    QSettings *settings;
    bool validEvent(int js);

    bool xPressed, yPressed, aPressed;//for control leminance and contrast and hue

public slots:
    void joystickCountChanged();

    void joystickPOVChanged(const int js , const int pov , const int angle);
    void joystickAxisChanged(const int js , const int axis , const double value);
    void joystickButtonChanged(const int js , const int button , const bool pressed);

signals:
    void osdMenuBtnTriggered();
    void enterBtnTriggered();
    void backBtnTriggered();
    void moveLeftBtnTriggered();
    void moveRightBtnTriggered();
    void moveTopBtnTriggered();
    void moveBottomBtnTriggered();
    void moveTopLeftBtnTriggered();
    void moveTopRightBtnTriggered();
    void moveBottomRightBtnTriggered();
    void moveBottomLeftBtnTriggered();
    void moveStop();
    void moveHome();
    void zoomInBtnTriggered();
    void zoomOutBtnTriggered();
    void zoomStop();
    void focusInBtnTriggered();
    void focusOutBtnTriggered();
    void focusStop();
    void focusStateChange();
    void luminanceChangeTriggered(bool increase);
    void contrastChangeTriggered(bool increase);
    void hueChangeTriggered(bool increase);
    void callPreset(int presetNum);
    void switchCamera(int cameraNum);

};

#endif // JOYSTICKWRAPPER_H
