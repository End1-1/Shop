#ifndef DCONFIRMATION_H
#define DCONFIRMATION_H

#include <QDialog>
#include <QEvent>

namespace Ui {
class DConfirmation;
}

class DConfirmation : public QDialog
{
    Q_OBJECT

public:
    explicit DConfirmation(QWidget *parent = 0);
    ~DConfirmation();
    static bool confirm(const QString &text = tr("To confirm, press Enter"), QWidget *parent = 0);

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

private:
    Ui::DConfirmation *ui;
};

#endif // DCONFIRMATION_H
