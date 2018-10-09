#ifndef DLGSELECTTAXDEP_H
#define DLGSELECTTAXDEP_H

#include <QDialog>
#include <QEvent>

namespace Ui {
class DlgSelectTaxDep;
}

class DlgSelectTaxDep : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSelectTaxDep(float maxValue, QString &partner, QWidget *parent = 0);
    ~DlgSelectTaxDep();
    int result;
    static int getKey(QWidget *parent, float &maxValue, QString &partnerTaxCode);

private slots:
    void on_leCard_returnPressed();

    void on_lePartnerTaxCode_returnPressed();

    void on_btnOK_clicked();

private:
    Ui::DlgSelectTaxDep *ui;
};

#endif // DLGSELECTTAXDEP_H
