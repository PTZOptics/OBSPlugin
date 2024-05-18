#include "joystickwrapper.h"
#include <QDebug>
#include <QStandardPaths>
#include "constants.h"
#include <SDL.h>

#define STICK_THRESHOULD 0.2
JoystickWrapper::JoystickWrapper(QObject *parent) : QObject(parent)
{
    settings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + DEFAULT_SETTINGS_PATH , QSettings::IniFormat);
//    QJoysticks::getInstance()->setVirtualJoystickRange(0.8);
    QJoysticks::getInstance()->setVirtualJoystickEnabled(false);
    connect(QJoysticks::getInstance() , SIGNAL(countChanged()) , this , SLOT(joystickCountChanged()));
    connect(QJoysticks::getInstance() , SIGNAL(povChanged(int,int,int)) , this , SLOT(joystickPOVChanged(int,int,int)));
    connect(QJoysticks::getInstance() , SIGNAL(axisChanged(int,int,qreal)) , this , SLOT(joystickAxisChanged(int,int,qreal)));
    connect(QJoysticks::getInstance() , SIGNAL(buttonChanged(int,int,bool)) , this , SLOT(joystickButtonChanged(int,int,bool)));
    xPressed = false;
    yPressed = false;
    aPressed = false;

}
JoystickWrapper::~JoystickWrapper()
{
    delete settings;
}
QStringList JoystickWrapper::getJoystickNames()
{
    return QJoysticks::getInstance()->deviceNames();
}
void JoystickWrapper::joystickCountChanged()
{
    xPressed = false;
    yPressed = false;
    aPressed = false;

//    qDebug()<<QJoysticks::getInstance()->count();
}
void JoystickWrapper::joystickPOVChanged(const int js , const int pov , const int angle)
{
    qDebug()<<angle;
    if(validEvent(js))
    {
        switch(angle)
        {
        case 0:
            emit moveTopBtnTriggered();
            break;
        case 45:
            emit moveTopRightBtnTriggered();
            break;
        case 90:
            emit moveRightBtnTriggered();
            break;
        case 135:
            emit moveBottomRightBtnTriggered();
            break;
        case 180:
            emit moveBottomBtnTriggered();
            break;
        case 225:
            emit moveBottomLeftBtnTriggered();
            break;
        case 270:
            emit moveLeftBtnTriggered();
            break;
        case 315:
            emit moveTopLeftBtnTriggered();
            break;
        case -1:
            emit moveStop();
            break;
        default:
            break;
        }
    }
}
void JoystickWrapper::joystickAxisChanged(const int js , const int axis , const double value)
{
    qDebug()<<axis;
    qDebug()<<value;
    int axesNum = QJoysticks::getInstance()->getNumAxes(js);
//    qDebug()<<"Number of axes"<<axesNum;
//    qDebug()<<(QJoysticks::getInstance()->getInputDevice(js)->axes);


    if(axesNum < 4)
        return;
    //Remember the box axis map is different between xbox nad generic usb joystick
    //after first moving there are some noise must remove
    //use lastMove

//    if(validEvent(js))
//    {

//        if(axis == SDL_CONTROLLER_AXIS_RIGHTX)//TR
//        {
//            if(value < STICK_THRESHOULD)
//            {
//                if(lastMove != MOVERIGHT)
//                    emit moveRightBtnTriggered();
//                lastMove = MOVERIGHT;
//            }
//            else if(value > -STICK_THRESHOULD)
//            {
//                if(lastMove != MOVELEFT)
//                    emit moveLeftBtnTriggered();
//                lastMove = MOVELEFT;
//            }
//            else
//            {
//                if(lastMove != STOPMOVING)
//                    emit moveStop();
//                lastMove = STOPMOVING;
//            }
//        }

//        if(axis == SDL_CONTROLLER_AXIS_RIGHTY)//TR - generic us
//        {
//            if(value < STICK_THRESHOULD)
//            {
//                if(lastMove != MOVEDOWN)
//                    emit moveBottomBtnTriggered();
//                lastMove = MOVEDOWN;
//            }
//            else if(value > -STICK_THRESHOULD)
//            {
//                if(lastMove != MOVEUP)
//                    emit moveTopBtnTriggered();
//                lastMove = MOVEUP;
//            }
//            else
//            {
//                if(lastMove != STOPMOVING)
//                    emit moveStop();
//                lastMove = STOPMOVING;
//            }
//        }

//        if(axis == SDL_CONTROLLER_AXIS_LEFTY)//TL
//        {
////            if(value > STICK_THRESHOULD)
////                emit switchCamera(3);
////            else if(value < - STICK_THRESHOULD)
////                emit switchCamera(1);
//            if(value < STICK_THRESHOULD)
//            {
//                if(lastMove != MOVEDOWN)
//                    emit moveBottomBtnTriggered();
//                lastMove = MOVEDOWN;
//            }
//            else if(value > -STICK_THRESHOULD)
//            {
//                if(lastMove != MOVEUP)
//                    emit moveTopBtnTriggered();
//                lastMove = MOVEUP;
//            }
//            else
//            {
//                if(lastMove != STOPMOVING)
//                    emit moveStop();
//                lastMove = STOPMOVING;
//            }

//        }

//        if(axis == SDL_CONTROLLER_AXIS_LEFTX)//TL
//        {
////            if(value > STICK_THRESHOULD)
////                emit switchCamera(2);
////            else if(value < - STICK_THRESHOULD)
////                emit switchCamera(4);
//            if(value < STICK_THRESHOULD)
//            {
//                if(lastMove != MOVERIGHT)
//                    emit moveRightBtnTriggered();
//                lastMove = MOVERIGHT;
//            }
//            else if(value > -STICK_THRESHOULD)
//            {
//                if(lastMove != MOVELEFT)
//                    emit moveLeftBtnTriggered();
//                lastMove = MOVELEFT;
//            }
//            else
//            {
//                if(lastMove != STOPMOVING)
//                    emit moveStop();
//                lastMove = STOPMOVING;
//            }
//        }

//    }
}
void JoystickWrapper::joystickButtonChanged(const int js , const int button , const bool pressed)
{
    qDebug()<<button;
    if(validEvent(js))
    {
        switch(button)
        {
        case BTN_START:
            if(!pressed)
                emit enterBtnTriggered();
            break;
        case BTN_BACK:
            if(!pressed)
                emit backBtnTriggered();
            break;
        case BTN_X:
            if(!pressed)
                emit callPreset(2);
            break;
        case BTN_Y:
            if(!pressed)
                emit callPreset(3);
            break;
        case BTN_A:
            if(!pressed)
                emit callPreset(1);
            break;
        case BTN_B:
            if(!pressed)
                emit focusStateChange();
            break;
        case BTN_TL:
            if(!pressed)
                emit osdMenuBtnTriggered();
            break;
        case BTN_TR:
            if(!pressed)
                emit moveHome();
        case BTN_LB:
            if(pressed)
                emit zoomInBtnTriggered();
            else
                emit zoomStop();
            break;
        case BTN_RB:
            if(pressed)
                emit zoomOutBtnTriggered();
            else
                emit zoomStop();
            break;
        case BTN_LT:
            if(pressed)
                emit focusInBtnTriggered();
            else
                emit focusStop();
            break;
        case BTN_RT:
            if(pressed)
                emit focusOutBtnTriggered();
            else
                emit focusStop();
            break;
        case DPAD_UP:
            if(pressed)
                emit moveTopBtnTriggered();
            else
                emit moveStop();
            break;
        case DPAD_DOWN:
            if(pressed)
                emit moveBottomBtnTriggered();
            else
                emit moveStop();
            break;
        case DPAD_LEFT:
            if(pressed)
                emit moveLeftBtnTriggered();
            else
                emit moveStop();
            break;
        case DPAD_RIGHT:
            if(pressed)
                emit moveRightBtnTriggered();
            else
                emit moveStop();
            break;
        default:
            break;
        }
    }
}
bool JoystickWrapper::validEvent(int js)
{
    if(!settings->value(SETTING_JOYSTICK_AVAILABLE , false).toBool())
        return false;
    if(js != settings->value(SETTING_SELECTED_JOYSTICK))
        return false;
    return true;
}
