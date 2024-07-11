#ifndef HOTKEYHELP_H
#define HOTKEYHELP_H

#pragma once

#include <QtWidgets/QDialog>

namespace Ui {
class HotkeyHelp;
}

class HotkeyHelp : public QDialog
{
    Q_OBJECT

public:
    explicit HotkeyHelp(QWidget *parent = 0);
    ~HotkeyHelp();

private:
    Ui::HotkeyHelp *ui;
};

#endif // HOTKEYHELP_H
