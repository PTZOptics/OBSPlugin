#ifndef JOYSTICKDLG_H
#define JOYSTICKDLG_H

#include <QDialog>
#include <QSettings>
namespace Ui {
class JoyStickDlg;
}

class JoyStickDlg : public QDialog
{
    Q_OBJECT

public:
    explicit JoyStickDlg(QWidget *parent = 0);
    ~JoyStickDlg();

private:
    Ui::JoyStickDlg *ui;
    QSettings *settings;
    void loadJoystickSettings();
    void saveJoystickSettings();
private slots:
    void onOKBtnClicked();
    void onCancelBtnClicked();
};

#endif // JOYSTICKDLG_H
