#-------------------------------------------------
#
# Project created by QtCreator 2018-04-24T14:25:03
#
#-------------------------------------------------
QT       += core gui network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PTZOptics
TEMPLATE = app

DESTDIR = $$PWD/../rundir/
MOC_DIR = $$PWD/../mocs
OBJECTS_DIR = $$PWD/../obj

#RC_ICONS += $$PWD/appicon.ico
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
RC_ICONS = appicon.ico
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    multicameras.cpp \
    aboutus.cpp \
    joystickdlg.cpp \
    QJoysticks.cpp \
    SDL_Joysticks.cpp \
    VirtualJoystick.cpp \
    joystickwrapper.cpp \
    aboutus.cpp \
    joystickdlg.cpp \
    joystickwrapper.cpp \
    main.cpp \
    mainwindow.cpp \
    multicameras.cpp \
    QJoysticks.cpp \
    SDL_Joysticks.cpp \
    VirtualJoystick.cpp \
    helpwin.cpp \
    hotkeyhelp.cpp \
    xboxhelp.cpp \
    pantiltlimitdlg.cpp

HEADERS += \
        mainwindow.h \
    mainwindow.h \
    aboutus.h \
    multicameras.h \
    constants.h \
    joystickdlg.h \
    JoysticksCommon.h \
    VirtualJoystick.h \
    QJoysticks.h \
    SDL_Joysticks.h \
    joystickwrapper.h \
    aboutus.h \
    constants.h \
    joystickdlg.h \
    JoysticksCommon.h \
    joystickwrapper.h \
    mainwindow.h \
    multicameras.h \
    QJoysticks.h \
    SDL_Joysticks.h \
    VirtualJoystick.h \
    helpwin.h \
    hotkeyhelp.h \
    xboxhelp.h \
    pantiltlimitdlg.h

FORMS += \
        mainwindow.ui \
    aboutus.ui \
    multicameras.ui \
    joystickdlg.ui \
    aboutus.ui \
    joystickdlg.ui \
    mainwindow.ui \
    multicameras.ui \
    helpwin.ui \
    hotkeyhelp.ui \
    xboxhelp.ui \
    pantiltlimitdlg.ui

RESOURCES += \
    ptzqrc.qrc\



mac: LIBS += -F$$PWD/lib/ -framework SDL2

INCLUDEPATH += $$PWD/lib/SDL2.framework/Versions/A/Headers
DEPENDPATH += $$PWD/lib/SDL2.framework/Versions/A/Headers


