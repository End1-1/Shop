#include "dconfirmation.h"
#include "ui_dconfirmation.h"

#include <QKeyEvent>

DConfirmation::DConfirmation(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DConfirmation)
{
    ui->setupUi(this);
    installEventFilter(this);
}

DConfirmation::~DConfirmation()
{
    delete ui;
}

bool DConfirmation::confirm(const QString &text, QWidget *parent)
{
    DConfirmation *d = new DConfirmation(parent);
    d->ui->label->setText(text);
    bool result = d->exec() == QDialog::Accepted;
    delete d;
    return result;
}

bool DConfirmation::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *k = static_cast<QKeyEvent*>(e);
        switch (k->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            accept();
            return true;
        default:
            reject();
            return true;
        }
    }
    return QDialog::eventFilter(o, e);
}
