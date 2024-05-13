#ifndef PANTILTLIMITDLG_H
#define PANTILTLIMITDLG_H

#include <QDialog>
#include <QWidget>

namespace Ui {
class PanTiltLimitDlg;
}

class PanTiltLimitDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PanTiltLimitDlg(QWidget *parent = 0);
    ~PanTiltLimitDlg();

private:
    QWidget *parent;
    Ui::PanTiltLimitDlg *ui;
    int checkLimitRage(int val);
    void setCameraPanTiltLimit(QString cameraId, int panLeft, int panRight, int tiltDown, int tiltUp);
    int getCameraLimit(QString cameraId, int side);
private slots:
    void applyLimits();
    void cancel();
    void clearUpRight();
    void clearDownLeft();
};

#endif // PANTILTLIMITDLG_H
