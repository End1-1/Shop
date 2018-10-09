#include "dlgselecttaxdep.h"
#include "ui_dlgselecttaxdep.h"
#include <QKeyEvent>

DlgSelectTaxDep::DlgSelectTaxDep(float maxValue, QString &partner, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgSelectTaxDep)
{
    ui->setupUi(this);
    result = Qt::Key_Escape;
    ui->leCard->setValidator(new QDoubleValidator(0, maxValue, 2));
    installEventFilter(this);
}

DlgSelectTaxDep::~DlgSelectTaxDep()
{
    delete ui;
}

int DlgSelectTaxDep::getKey(QWidget *parent, float &maxValue, QString &partnerTaxCode)
{
    DlgSelectTaxDep *d = new DlgSelectTaxDep(maxValue, partnerTaxCode, parent);
    d->exec();
    maxValue = d->ui->leCard->text().toFloat();
    partnerTaxCode = d->ui->lePartnerTaxCode->text();
    int result = d->result;
    delete d;
    return result;
}

void DlgSelectTaxDep::on_leCard_returnPressed()
{
    ui->lePartnerTaxCode->setFocus();
}

void DlgSelectTaxDep::on_lePartnerTaxCode_returnPressed()
{
    ui->btnOK->setFocus();
}

void DlgSelectTaxDep::on_btnOK_clicked()
{
    accept();
}
