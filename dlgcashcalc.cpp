#include "dlgcashcalc.h"
#include "ui_dlgcashcalc.h"

DlgCashCalc::DlgCashCalc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCashCalc)
{
    ui->setupUi(this);
    ui->leCash->setValidator(new QDoubleValidator(0, 1000000000, 2));
}

DlgCashCalc::~DlgCashCalc()
{
    delete ui;
}

bool DlgCashCalc::getCash(double amount, double &cash)
{
    DlgCashCalc *d = new DlgCashCalc(0);
    d->ui->leAmount->setText(QString::number(amount, 'f', 0));
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        cash = d->ui->leCash->text().toDouble();
    }
    delete d;
    return result;
}

void DlgCashCalc::on_leCash_textChanged(const QString &arg1)
{
    ui->leChange->setText(QString::number(ui->leAmount->text().toDouble() - arg1.toDouble(), 'f', 0));
}

void DlgCashCalc::on_btnCancel_clicked()
{
    reject();
}

void DlgCashCalc::on_btnOK_clicked()
{
    accept();
}

void DlgCashCalc::on_btn1000_clicked()
{
    ui->leCash->setText("1000");
}

void DlgCashCalc::on_btn5000_clicked()
{
    ui->leCash->setText("5000");
}

void DlgCashCalc::on_btn10000_clicked()
{
    ui->leCash->setText("10000");
}

void DlgCashCalc::on_btn15000_clicked()
{
    ui->leCash->setText("15000");
}

void DlgCashCalc::on_btn20000_clicked()
{
    ui->leCash->setText("20000");
}

void DlgCashCalc::on_leCash_returnPressed()
{
    accept();
}
