#ifndef DLOGIN_H
#define DLOGIN_H

#include <QDialog>
#include "dbdriver.h"
#include <QTimer>
#include <QKeyEvent>
#include "printing.h"

namespace Ui {
class DLogin;
}

typedef struct {
    int id;
    QString name;
    double price;
    QString adgcode;
} Goods;

class DLogin : public QDialog
{
    Q_OBJECT

public:
    QMap<QString, int> m_barCode;
    QMap<int, Goods> m_goods;
    explicit DLogin(QWidget *parent = 0);
    ~DLogin();
    int addDish(int id);
    void changeQty(int row, double qty);
    int removeRow(int r = -1);
    QTimer m_timer;

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
private:
    Ui::DLogin *ui;
    int fDep;
    DbDriver m_db;
    double cash;
    double change;
    void rowUp();
    void rowDown();
    void changeBarcodeColor(QColor color);
    void countTotal();
    void readBarcode();
    void writeOrder();
    void printOrder(const QString &taxOut);
    void clearOrder();
    void setFlag();

private slots:
    void timeout();
};

#endif // DLOGIN_H
