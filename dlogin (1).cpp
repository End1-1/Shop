#include "dlogin.h"
#include "ui_dlogin.h"
#include <QPalette>
#include <QDate>
#include <QProcess>
#include "dconfirmation.h"
#include "dquickgoods.h"
#include "dqty.h"
#include "qsystem.h"
#include "dlgselecttaxdep.h"
#include "common.h"
#include "dlgcashcalc.h"

DLogin::DLogin(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DLogin)
{
    ui->setupUi(this);
    QFont f(qApp->font());
    f.setPointSize(f.pointSize() + 4);
    qApp->setFont(f);
    f.setPointSize(16);
    ui->tblTotal->setFont(f);
    /* Config tablewidget */
    QList<int> widths;
    widths << 0 << 350 << 100 << 100 << 150 << 0;
    for (int i = 0; i < widths.count(); i ++) {
        ui->tblGoods->setColumnWidth(i, widths.at(i));
        ui->tblTotal->setColumnWidth(i, widths.at(i));
        ui->tblTotal->setItem(0, i, new QTableWidgetItem());
    }
    /* Enable timer */
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer.start(1000);
    /* Get goods data */
    m_db.openDB();
    m_db.execSQL("select fbarcode, fgoods from barcode");
    while (m_db.next())
        m_barCode[m_db.v_str(0)] = m_db.v_int(1);
    m_db.execSQL("select g.id, g.fname, g.fprice, gg.adgcode from goods g, groups gg where gg.id=g.fgroup");
    while (m_db.next()) {
        Goods g;
        g.id = m_db.v_int(0);
        g.name = m_db.v_str(1);
        g.price = m_db.v_dbl(2);
        g.adgcode = m_db.v_str(3);
        m_goods.insert(g.id, g);
    }
    m_db.closeDB();
    /* etc... */
    //ui->leBarcode->setValidator();
    ui->leBarcode->installEventFilter(this);
    /* set flags images */
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(50, 50));
    item->setIcon(QIcon(QPixmap(":/dostavka.png")));
    item->setFlags(item->flags() ^ Qt::ItemIsEnabled);
    ui->lstFlag->addItem(item);
}

DLogin::~DLogin()
{
    delete ui;
}

bool DLogin::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->leBarcode) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *k = static_cast<QKeyEvent*>(e);
            switch (k->key()) {
            case Qt::Key_Up:
                rowUp();
                return true;
            case Qt::Key_Down:
                rowDown();
                return true;
            case Qt::Key_Plus: {
                int row = ui->tblGoods->currentRow();
                if (row > -1) {
                    double qty = DQty::getQty(this);
                    if (qty > 0)
                        changeQty(row, qty);
                }
                return true;
            }
            case Qt::Key_Minus:
                removeRow();
                return true;
            case Qt::Key_Return:
            case Qt::Key_Enter:
                readBarcode();
                return true;
            case Qt::Key_Escape:
                return true;
            case Qt::Key_F4: {
                DQuickGoods *d = new DQuickGoods(this, m_goods, this);
                d->exec();
                delete d;
                break;
            }
            case Qt::Key_F12:
                if (!ui->tblGoods->rowCount())
                    return true;
                if (DlgCashCalc::getCash(ui->tblTotal->item(0, 4)->data(Qt::EditRole).toDouble()))
                    writeOrder();
                return true;
            case Qt::Key_F5:
                setFlag();
                return true;
            }
        }
        return false;
    }
    return DLogin::eventFilter(o, e);
}

void DLogin::rowUp()
{
    if (ui->tblGoods->currentRow() > 0)
        ui->tblGoods->setCurrentCell(ui->tblGoods->currentRow() - 1, 0);
}

void DLogin::rowDown()
{
    if (ui->tblGoods->currentRow() < ui->tblGoods->rowCount() - 1)
        ui->tblGoods->setCurrentCell(ui->tblGoods->currentRow() + 1, 0);
}


void DLogin::changeBarcodeColor(QColor color)
{
    QPalette p = ui->leBarcode->palette();
    p.setColor(ui->leBarcode->backgroundRole(), color);
    ui->leBarcode->setPalette(p);
}

void DLogin::countTotal()
{
    double q = 0.0, a = 0.0;
    for (int i = 0, c = ui->tblGoods->rowCount(); i < c; i++){
        q += ui->tblGoods->item(i, 2)->data(Qt::EditRole).toDouble();
        a += ui->tblGoods->item(i, 4)->data(Qt::EditRole).toDouble();
    }
    ui->tblTotal->item(0, 2)->setData(Qt::EditRole, q);
    ui->tblTotal->item(0, 4)->setData(Qt::EditRole, a);
}

void DLogin::timeout()
{
    ui->leBarcode->setFocus();
}

void DLogin::readBarcode()
{
    QString barcode = ui->leBarcode->text();
    ui->leBarcode->clear();
    if (!m_barCode.contains(barcode)) {
        changeBarcodeColor(Qt::red);
        return;
    }
    int goodsId = m_barCode[barcode];
    addDish(goodsId);
}

int DLogin::addDish(int id)
{
    if (!m_goods.contains(id)) {
        changeBarcodeColor(Qt::red);
        return -1;
    }
    Goods g = m_goods[id];
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    for (int i = 0; i < ui->tblGoods->columnCount(); i++)
        ui->tblGoods->setItem(row, i, new QTableWidgetItem());
    ui->tblGoods->item(row, 0)->setData(Qt::EditRole, g.id);
    ui->tblGoods->item(row, 1)->setData(Qt::EditRole, g.name);
    ui->tblGoods->item(row, 2)->setData(Qt::EditRole, 1.0);
    ui->tblGoods->item(row, 3)->setData(Qt::EditRole, g.price);
    ui->tblGoods->item(row, 4)->setData(Qt::EditRole, g.price);
    ui->tblGoods->item(row, 5)->setData(Qt::EditRole, g.adgcode);
    ui->tblGoods->setCurrentCell(row, 0);
    changeBarcodeColor(Qt::white);
    QStringList h;
    h << QString::number(ui->tblGoods->rowCount());
    ui->tblTotal->setVerticalHeaderLabels(h);
    countTotal();
    return row;
}

void DLogin::changeQty(int row, double qty)
{
    ui->tblGoods->item(row, 2)->setData(Qt::EditRole, qty);
    ui->tblGoods->item(row, 4)->setData(Qt::EditRole, ui->tblGoods->item(row, 2)->data(Qt::EditRole).toDouble() * ui->tblGoods->item(row, 3)->data(Qt::EditRole).toDouble());
    countTotal();
}

int DLogin::removeRow(int r)
{
    int row = ui->tblGoods->currentRow();
    if (r > -1)
        row = r;
    if (row < -1)
        return -1;
    ui->tblGoods->removeRow(row);
    countTotal();
    if (row > 0)
        row--;
    else if (ui->tblGoods->rowCount())
        row = 0;
    if (!ui->tblGoods->rowCount())
        row = -1;
    ui->tblGoods->setCurrentCell(row, 0);
    return row;
}

void DLogin::writeOrder()
{
    if (!m_db.openDB())
        return;
    int orderId = m_db.genId("GEN_STH_ID");
    if (!orderId)
        return;
    ui->tblTotal->item(0, 0)->setData(Qt::EditRole, orderId);
    if (!m_db.prepare("insert into sth (id, ftype, fdate, fpartner, famount) values (:id, :ftype, :fdate, :fpartner, :famount)"))
        return;
    m_db.bindValue(":id", orderId);
    m_db.bindValue(":ftype", 1);
    m_db.bindValue(":fdate", QDate::currentDate());
    m_db.bindValue(":fpartner", 1);
    m_db.bindValue(":famount", ui->tblTotal->item(0, 4)->data(Qt::EditRole));
    if (!m_db.execSQL())
        return;
    if (!m_db.prepare("insert into stb (fdoc, fgoods, fqty, fprice) values (:fdoc, :fgoods, :fqty, :fprice);"))
        return;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        m_db.bindValue(":fdoc", orderId);
        m_db.bindValue(":fgoods", ui->tblGoods->item(i, 0)->data(Qt::EditRole));
        m_db.bindValue(":fqty", ui->tblGoods->item(i, 2)->data(Qt::EditRole));
        m_db.bindValue(":fprice", ui->tblGoods->item(i, 3)->data(Qt::EditRole));
        if (!m_db.execSQL())
            return;
    }
    QListWidgetItem *item = ui->lstFlag->item(0);
    if (item->data(Qt::UserRole).toBool()) {
        if (!m_db.prepare("insert into stf values (:forder, :fflag)"))
            return;
        m_db.bindValue(":forder", orderId);
        m_db.bindValue(":fflag", 1);
        if (!m_db.execSQL())
            return;
    }
    m_db.closeDB();
    printOrder();
    printTax();
    clearOrder();
}

void DLogin::printOrder()
{
    SizeMetrics sm(___printerInfo->resolution("local"));
    XmlPrintMaker pm(&sm);
    pm.setFontName(qApp->font().family());
    pm.setFontSize(16);
    int top = 5;
    int num = 1;
    pm.textCenterAlign(tr("Order N") + QString::number(ui->tblTotal->item(0, 0)->data(Qt::EditRole).toInt()), 60, top);
    top += pm.lastTextHeight() + 1;
    pm.setFontSize(10);
    pm.line(0, top, 80, top);
    top++;
    pm.text("N", 5, top);
    pm.text(tr("Name"), 10, top);
    pm.text(tr("Qty"), 30, top);
    pm.text(tr("Price"), 45, top);
    pm.text(tr("Amount"), 60, top);
    top += pm.lastTextHeight() + 1;
    pm.line(0, top, 80, top);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        pm.text(QString("%1.").arg(num), 5, top);
        pm.text(ui->tblGoods->item(i, 1)->data(Qt::EditRole).toString(), 10, top);
        top += pm.lastTextHeight() + 1;
        pm.text(QString::number(ui->tblGoods->item(i, 2)->data(Qt::EditRole).toDouble(), 'f', 2), 30, top);
        pm.text(QString::number(ui->tblGoods->item(i, 3)->data(Qt::EditRole).toDouble(), 'f', 0), 45, top);
        pm.text(QString::number(ui->tblGoods->item(i, 4)->data(Qt::EditRole).toDouble(), 'f', 0), 60, top);
        top += pm.lastTextHeight() + 1;
        pm.line(0, top, 80, top);
        top++;
        pm.checkForNewPage(top);
    }
    pm.line(0, top, 80, top);
    pm.setFontSize(12);
    pm.text(tr("Total"), 5, top);
    pm.text(QString::number(ui->tblTotal->item(0, 4)->data(Qt::EditRole).toDouble(), 'f', 0), 60, top);
    top += pm.lastTextHeight() + 4;
    pm.setFontSize(10);
    pm.text(tr("Thank you for visit!"), 5, top);
    top += pm.lastTextHeight() + 2;
    pm.setFontSize(8);
    pm.text(QDateTime::currentDateTime().toString(DATETIME_FORMAT), 5, top);
    top += pm.lastTextHeight() + 2;
    pm.text(".", 5, top);
    pm.finishPage();
    ThreadPrinter *tp = new ThreadPrinter("local", sm, pm);
    tp->start();
}

void DLogin::clearOrder()
{
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->tblTotal->item(0, 2)->setData(Qt::EditRole, 0);
    ui->tblTotal->item(0, 4)->setData(Qt::EditRole, 0);
    QStringList h;
    h << "0";
    ui->tblTotal->setVerticalHeaderLabels(h);
    QListWidgetItem *item = ui->lstFlag->item(0);
    if (item->data(Qt::UserRole).toBool())
        item->setFlags(item->flags() ^ Qt::ItemIsEnabled);
    item->setData(Qt::UserRole, false);
}

void DLogin::printTax()
{
    if (!ui->tblGoods->rowCount())
        return;
    float amountCard = ui->tblTotal->item(0, 4)->data(Qt::EditRole).toDouble();
    float amount = amountCard;
    DlgSelectTaxDep *dlg = new DlgSelectTaxDep(amount, this);
    dlg->exec();
    int dep = dlg->result;
    delete dlg;
    if (dep == Qt::Key_Escape)
        return;
    switch (dep) {
    case Qt::Key_F1:
        dep = 1;
        break;
    case Qt::Key_F2:
        dep = 2;
        break;
    }

    QString fileName = QString("tax_%1.json")
            .arg(ui->tblTotal->item(0, 0)->data(Qt::EditRole).toString());

    amount = amountCard;
    amountCard = 0;
    QFile f(QSystem::appPath + "\\" + fileName);
    if (f.open(QIODevice::WriteOnly)) {
        float service = 0;
        float discount = 0;
        f.write((QString("{\"seq\":1,"
                         "\"paidAmount\":%1, \"paidAmountCard\":%2, \"partialAmount\":0, \"prePaymentAmount\":0, "
                         "\"mode\":2, \"useExtPOS\":true,\"items\":[")
                 .arg(QString::number(amount, 'f', 2))
                 .arg(QString::number(amountCard, 'f', 2)).toUtf8()));
        bool first = true;
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if (!first)
                f.write(",");
            else
                first = false;
            float price = ui->tblGoods->item(i, 3)->data(Qt::EditRole).toDouble();
            price += (price * service);
            price -= (price * discount);
            f.write(QString("{\"dep\":%1,\"qty\":%2,\"price\":%3,\"productCode\":\"%4\",\"productName\":\"%5\", "
                            "\"adgCode\":\"%6\", \"unit\":\"%7\"}")
                    .arg(dep) //Payment type
                    .arg(QString::number(ui->tblGoods->item(i, 2)->data(Qt::EditRole).toDouble(), 'f', 3))
                    .arg(QString::number(ui->tblGoods->item(i, 3)->data(Qt::EditRole).toDouble(), 'f', 2))
                    .arg(ui->tblGoods->item(i, 0)->data(Qt::EditRole).toString())
                    .arg(ui->tblGoods->item(i, 1)->data(Qt::EditRole).toString())
                    .arg(ui->tblGoods->item(i, 5)->data(Qt::EditRole).toString())
                    .arg(QString::fromUtf8("հատ"))
                    .toUtf8());
        }
        f.write("]}");
        f.close();
        QStringList args;
        args << getx("taxip").toString()
             << getx("taxport").toString()
             << QSystem::hdmPass
             << fileName;
        QProcess *p = new QProcess();
        p->start(QSystem::appPath + "\\printtax.exe", args);
    }
}

void DLogin::setFlag()
{
    QListWidgetItem *item = ui->lstFlag->item(0);
    if (item->data(Qt::UserRole).toBool()) {
        item->setFlags(item->flags() ^ Qt::ItemIsEnabled);
        item->setData(Qt::UserRole, false);
     } else {
        item->setFlags(item->flags() | Qt::ItemIsEnabled);
        item->setData(Qt::UserRole, true);
    }
}
