#ifndef HELPWIN_H
#define HELPWIN_H

#include <QDialog>

namespace Ui {
class HelpWin;
}

class HelpWin : public QDialog
{
    Q_OBJECT

public:
    explicit HelpWin(QWidget *parent = 0);
    ~HelpWin();

private:
    Ui::HelpWin *ui;
};

#endif // HELPWIN_H
