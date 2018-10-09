#ifndef DQUICKGOODS_H
#define DQUICKGOODS_H

#include <QDialog>
#include "dlogin.h"
#include "dbdriver.h"
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QItemDelegate>
#include <QLabel>
#include <QTableWidgetItem>

namespace Ui {
class DQuickGoods;
}

class TableCellLable : public QLabel {
public:
    QTableWidgetItem *m_oldItem;
    TableCellLable(QTableWidgetItem *item);
};

class D : public QItemDelegate {
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class DQuickGoods : public QDialog
{
    Q_OBJECT

public:
    explicit DQuickGoods(DLogin *login, QMap<int, Goods> &goods, QWidget *parent = 0);
    ~DQuickGoods();

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

private:
    DbDriver m_db;
    TableCellLable *m_tableCell;
    Ui::DQuickGoods *ui;
    DLogin *m_login;
    int m_currRow;
    QTimer m_timer;
    QMap<int, Goods> &m_goods;
    void readCode();
    void setSelection();
    void changeQty();
    void removeRow();

private slots:
    void timeout();
};

#endif // DQUICKGOODS_H
