#ifndef XBOXHELP_H
#define XBOXHELP_H

#include <QDialog>

namespace Ui {
class XBoxHelp;
}

class XBoxHelp : public QDialog
{
    Q_OBJECT

public:
    explicit XBoxHelp(QWidget *parent = 0);
    ~XBoxHelp();

private:
    Ui::XBoxHelp *ui;
};

#endif // XBOXHELP_H
