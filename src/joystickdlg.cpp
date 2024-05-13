#include "joystickdlg.h"
#include "ui_joystickdlg.h"
#include "constants.h"
#include <QStandardPaths>
#include "QJoysticks.h"
JoyStickDlg::JoyStickDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoyStickDlg)
{
    ui->setupUi(this);

    settings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + DEFAULT_SETTINGS_PATH , QSettings::IniFormat);
    connect(ui->okBtn , SIGNAL(clicked(bool)) , this , SLOT(onOKBtnClicked()));
    connect(ui->cancelBtn , SIGNAL(clicked(bool)) , this , SLOT(onCancelBtnClicked()));

    QJoysticks::getInstance()->updateInterfaces();
    QJoysticks::getInstance()->setVirtualJoystickEnabled(false);
    QStringList joystickNames = QJoysticks::getInstance()->deviceNames();
    for(int i = 0 ; i < joystickNames.size() ; i ++)
    {
        ui->joystickCombo->addItem(joystickNames.at(i));
    }
    loadJoystickSettings();
}

JoyStickDlg::~JoyStickDlg()
{
    delete settings;
    delete ui;
}

void JoyStickDlg::loadJoystickSettings()
{
    int js = settings->value(SETTING_SELECTED_JOYSTICK , 0).toInt();
    bool joystickEnabled = settings->value(SETTING_JOYSTICK_AVAILABLE , false).toBool();
    bool hotkeyEnabled = settings->value(SETTING_HOTKEY_AVAILABLE , false).toBool();
    ui->useJoyStickCheck->setChecked(joystickEnabled);
    ui->useHotKeyCheck->setChecked(hotkeyEnabled);
    ui->joystickCombo->setCurrentIndex(js);
}
void JoyStickDlg::saveJoystickSettings()
{
    settings->setValue(SETTING_SELECTED_JOYSTICK , ui->joystickCombo->currentIndex());
    settings->setValue(SETTING_JOYSTICK_AVAILABLE , ui->useJoyStickCheck->isChecked());
    settings->setValue(SETTING_HOTKEY_AVAILABLE , ui->useHotKeyCheck->isChecked());
}

void JoyStickDlg::onOKBtnClicked()
{
    saveJoystickSettings();
    close();
}
void JoyStickDlg::onCancelBtnClicked()
{
    close();
}
