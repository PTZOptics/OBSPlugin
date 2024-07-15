#include "hotkeyhelp.h"
#include "ui_hotkeyhelp.h"

HotkeyHelp::HotkeyHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HotkeyHelp)
{
    ui->setupUi(this);
}

HotkeyHelp::~HotkeyHelp()
{
    delete ui;
}
