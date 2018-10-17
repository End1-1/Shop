#ifndef DLGCASHCALC_H
#define DLGCASHCALC_H

#include <QDialog>

namespace Ui {
class DlgCashCalc;
}

class DlgCashCalc : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCashCalc(QWidget *parent = 0);
    ~DlgCashCalc();
    static bool getCash(double amount, double &cash);
private slots:
    void on_leCash_textChanged(const QString &arg1);

    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btn1000_clicked();

    void on_btn5000_clicked();

    void on_btn10000_clicked();

    void on_btn15000_clicked();

    void on_btn20000_clicked();

    void on_leCash_returnPressed();

private:
    Ui::DlgCashCalc *ui;
};

#endif // DLGCASHCALC_H
