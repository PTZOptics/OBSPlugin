#include "helpwin.h"
#include "ui_helpwin.h"

HelpWin::HelpWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWin)
{
    ui->setupUi(this);
}

HelpWin::~HelpWin()
{
    delete ui;
}
