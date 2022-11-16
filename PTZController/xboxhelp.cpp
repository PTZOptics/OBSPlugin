#include "xboxhelp.h"
#include "ui_xboxhelp.h"

XBoxHelp::XBoxHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XBoxHelp)
{
    ui->setupUi(this);
}

XBoxHelp::~XBoxHelp()
{
    delete ui;
}
