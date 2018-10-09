#include "dquickgoods.h"
#include "ui_dquickgoods.h"
#include "dqty.h"

DQuickGoods::DQuickGoods(DLogin *login, QMap<int, Goods> &goods, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DQuickGoods),
    m_goods(goods)
{
    QFont f(font());
    f.setBold(true);
    f.setPointSize(f.pointSize() - 1);
    setFont(f);
    m_currRow = -1;
    m_login = login;
    ui->setupUi(this);
    showFullScreen();
    qApp->processEvents();
    ui->tblGoods->setColumnCount((width() - 10) / 250);
    for (int i = 0; i < ui->tblGoods->columnCount(); i++)
        ui->tblGoods->setColumnWidth(i, 250);
    if (!m_db.openDB())
        return;
    m_db.execSQL("select g.id, g.fname, q.fqueue from goods g, quick_goods q where g.id=q.fgoods order by q.fqueue");
    int c = 0, r = 0, queue = 1;
    while (m_db.next()) {
        if (!ui->tblGoods->rowCount())
            ui->tblGoods->setRowCount(1);
        QString text = QString("%1. %2").arg((ui->tblGoods->columnCount() * r) + c + 1).arg(m_db.v_str(1));
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setData(Qt::UserRole, m_db.v_int(0));
        item->setData(Qt::UserRole + 1, queue++);
        ui->tblGoods->setItem(r, c, item);
        c++;
        if (c >= ui->tblGoods->columnCount()) {
            c = 0;
            r++;
            if (r > ui->tblGoods->rowCount() - 1)
                ui->tblGoods->setRowCount(r + 1);
        }
    }
    m_db.closeDB();
    ui->leCode->installEventFilter(this);
    ui->leCode->setValidator(new QIntValidator());
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer.start(1000);
    ui->tblGoods->setItemDelegate(new D());
    m_tableCell = 0;
    ui->tblGoods->viewport()->installEventFilter(this);
}

DQuickGoods::~DQuickGoods()
{
    delete ui;
}

bool DQuickGoods::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->leCode) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *k = static_cast<QKeyEvent*>(e);
            switch (k->key()) {
            case Qt::Key_Escape:
                if (ui->leCode->text().length())
                    ui->leCode->clear();
                else
                    accept();
                return true;
            case Qt::Key_Enter:
            case Qt::Key_Return:
                readCode();
                return true;
            case Qt::Key_Plus:
                changeQty();
                return true;
            case Qt::Key_Minus:
                removeRow();
                return true;
            }
        }
        if (e->type() == QEvent::KeyRelease) {
            QKeyEvent *k = static_cast<QKeyEvent*>(e);
            switch (k->key()) {
            case Qt::Key_0:
            case Qt::Key_1:
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
                setSelection();
                return true;
            }
        }
        return false;
    }
    if (o == ui->tblGoods->viewport()) {
        if (e->type() == QEvent::MouseButtonPress) {
            QCursor c;
            QTableWidgetItem *item = ui->tblGoods->itemAt(c.pos());
            if (!item)
                return false;
            m_tableCell = new TableCellLable(item);
            m_tableCell->setMinimumSize(ui->tblGoods->columnWidth(0), ui->tblGoods->rowHeight(0));
            m_tableCell->setMaximumSize(ui->tblGoods->columnWidth(0), ui->tblGoods->rowHeight(0));
            m_tableCell->move(mapToGlobal(c.pos()));
            m_tableCell->show();
            return true;
        } else if(e->type() == QEvent::MouseButtonRelease) {
            if (m_tableCell) {
                QTableWidgetItem *oldItem = m_tableCell->m_oldItem;
                delete m_tableCell;
                m_tableCell = 0;
                QTableWidgetItem *item = ui->tblGoods->itemAt(QCursor().pos());
                if (item) {
                    QTableWidgetItem tempItem;
                    tempItem.setText(oldItem->text());
                    tempItem.setData(Qt::UserRole, oldItem->data(Qt::UserRole));
                    tempItem.setData(Qt::UserRole + 1, oldItem->data(Qt::UserRole + 1));
                    oldItem->setText(item->text());
                    oldItem->setData(Qt::UserRole, item->data(Qt::UserRole));
                    oldItem->setData(Qt::UserRole + 1, item->data(Qt::UserRole + 1));
                    item->setText(tempItem.text());
                    item->setData(Qt::UserRole, tempItem.data(Qt::UserRole));
                    item->setData(Qt::UserRole + 1, tempItem.data(Qt::UserRole + 1));
                    m_db.openDB();
                    m_db.prepare("update quick_goods set fqueue=:fqueue where fgoods=:fgoods");
                    int queue = 1;
                    bool done = false;
                    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
                        if (done)
                            break;
                        for (int j = 0; j < ui->tblGoods->columnCount(); j++) {
                            QTableWidgetItem *item = ui->tblGoods->item(i, j);
                            if (!item) {
                                done = true;
                                break;
                            }
                            m_db.bindValue(":fqueue", queue++);
                            m_db.bindValue(":fgoods", item->data(Qt::UserRole));
                            m_db.execSQL();
                        }
                    }
                    m_db.closeDB();
                }
                return true;
            }
        } else if (e->type() == QEvent::MouseMove) {
            if (m_tableCell) {
                m_tableCell->move(mapToGlobal(QCursor().pos()));
                return true;
            }
        }
    }
    return QDialog::eventFilter(o, e);
}

void DQuickGoods::readCode()
{
    ui->leCode->clear();
    QTableWidgetItem *item = ui->tblGoods->currentItem();
    if (!item)
        return;
    int goodsId = item->data(Qt::UserRole).toInt();
    m_currRow = m_login->addDish(goodsId);
    ui->leLastGoods->setText(m_goods[goodsId].name);
}

void DQuickGoods::setSelection()
{
    ui->tblGoods->clearSelection();
    int index = ui->leCode->text().toInt();
    if (!index)
        return;
    index--;
    int row = index / ui->tblGoods->columnCount();
    int col = index % ui->tblGoods->columnCount();
    QModelIndex i = ui->tblGoods->model()->index(row, col);
    if (!i.isValid()) {
        ui->leCode->clear();
        ui->tblGoods->setCurrentCell(-1, -1);
        return;
    }
    ui->tblGoods->setCurrentCell(row, col);
}

void DQuickGoods::changeQty()
{
    if (m_currRow < 0)
        return;
    double qty = DQty::getQty(this);
    if (qty > 0)
        m_login->changeQty(m_currRow, qty);
}

void DQuickGoods::removeRow()
{
    if (m_currRow < 0)
        return;
    m_currRow = m_login->removeRow(m_currRow);
}

void DQuickGoods::timeout()
{
    ui->leCode->setFocus();
}

void D::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    painter->save();
    QPen pen(Qt::black);
    pen.setStyle(Qt::NoPen);
    QBrush brush(Qt::white);
    if (option.state & QStyle::State_Selected) {
        brush.setColor(Qt::blue);
        pen.setColor(Qt::white);
    }
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(option.rect);
    QRect r = option.rect;
    r.adjust(4, 4, -4, -4);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawText(r, index.data(Qt::DisplayRole).toString());
    painter->restore();
}

TableCellLable::TableCellLable(QTableWidgetItem *item) :
    QLabel(0, Qt::FramelessWindowHint)
{
    setText(item->text());
    m_oldItem = item;
}
