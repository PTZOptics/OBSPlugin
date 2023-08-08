#ifndef MULTICAMERAS_H
#define MULTICAMERAS_H

#include <QDialog>

namespace Ui {
class MultiCameras;
}

class MultiCameras : public QDialog
{
    Q_OBJECT

public:
    explicit MultiCameras(QWidget *parent = 0);
    ~MultiCameras();

private:
    Ui::MultiCameras *ui;
    QString settingsFile;

private:
    void saveSettings();
    void loadSettings();

private slots:
    void onOKClicked();
    void onCancelClicked();
};

#endif // MULTICAMERAS_H
