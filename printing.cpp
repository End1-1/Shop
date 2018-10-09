#include "printing.h"
#include <QPicture>
#include <math.h>

PrinterInfo *___printerInfo;
QMutex m;

XmlPrinter::XmlPrinter(QPainter *painter, SizeMetrics *sm)
{
    m_painter = painter;
    m_sm = sm;
    m_marginLeft = 3;
    m_marginTop = 3;
}

void XmlPrinter::render(const QByteArray &data)
{
    QXmlInputSource xmlSource;
    xmlSource.setData(data);

    QXmlSimpleReader xmlReader;
    xmlReader.setContentHandler(this);
    xmlReader.setErrorHandler(this);
    xmlReader.parse(xmlSource);
}
#include <QDebug>

bool XmlPrinter::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)

    if (qName == "line") {
        float points[4];
        points[0] = atts.value("left").toInt() + m_marginLeft;
        points[1] = atts.value("top").toInt() + m_marginTop;
        points[2] = atts.value("right").toInt() + m_marginLeft;
        points[3] = atts.value("bottom").toInt() + m_marginTop;
        m_sm->points(points, 4);
        m_painter->drawLine(points[0], points[1], points[2], points[3]);
        return true;
    }

    if (qName == "penwidth") {
        m_pen.setWidth(atts.value("value").toInt());
        m_painter->setPen(m_pen);
        return true;
    }

    if (qName == "penstyle") {
        m_pen.setStyle(static_cast<Qt::PenStyle>(atts.value("value").toInt()));
        m_painter->setPen(m_pen);
        return true;
    }

    if (qName == "image") {
        float points[4];
        points[0] = atts.value("left").toFloat() + m_marginLeft;
        points[1] = atts.value("top").toFloat() + m_marginTop;
        points[2] = atts.value("right").toFloat() + m_marginLeft;
        points[3] = atts.value("bottom").toFloat() + m_marginTop;
        m_sm->points(points, 4);

        QPixmap pix(atts.value("file"));
        m_painter->drawPixmap(QPointF(points[0], points[1]), pix);


        if (!atts.value("del").compare("yes"))
            qDebug() << QFile::remove(atts.value("file"));
        return true;
    }

    if (qName == "text") {
        QFontMetrics *fm = new QFontMetrics(m_sm->font());
        int linesCount = atts.value("lines").toInt();

        float points[6];
        points[0] = atts.value("left").toInt() + m_marginLeft;
        points[1] = atts.value("top").toInt() + m_marginTop;
        points[2] = atts.value("maxwidth").toInt();;
        if (points[2] == 0)
            points[2] = fm->width(atts.value("value")) ;
        points[3] = fm->width(atts.value("value"));
        points[4] = fm->height();// / m_sm->m_scaleFactor;
        points[5] = fm->ascent() + fm->descent();
        delete fm;
        m_sm->points(points, 6);

        //QRect rect(points[0], points[1] - (points[5] * linesCount), points[2], points[4] * linesCount);
        QRect rect(points[0], points[1], points[2], points[4] * linesCount);
        QTextOption to;
        to.setWrapMode(QTextOption::WordWrap);
        m_painter->drawText(rect, atts.value("value"), to);
        return true;
    }

    if (qName == "fontsize") {
        m_sm->font().setPointSize(atts.value("value").toInt());
        m_painter->setFont(m_sm->font());
        return true;
    }

    if (qName == "fontname") {
        m_sm->font().setFamily(atts.value("value"));
        m_painter->setFont(m_sm->font());
        return true;
    }

    if (qName == "fontbold") {
        m_sm->font().setBold(static_cast<bool>(atts.value("value").toInt()));
        m_painter->setFont(m_sm->font());
        return true;
    }

    if (qName == "fontitalic") {
        m_sm->font().setItalic(static_cast<bool>(atts.value("value").toInt()));
        m_painter->setFont(m_sm->font());
        return true;
    }

    return true;
}

bool XmlPrinter::error(const QXmlParseException &exception)
{
    Q_UNUSED(exception)
    return true;
}

void XmlPrinter::setMargins(int marginLeft, int marginTop)
{
    m_marginLeft = marginLeft;
    m_marginTop = marginTop;
}

QFont &XmlPrinter::font()
{
    return m_sm->font();
}

XmlPrintMaker& XmlPrintMaker::operator =(const XmlPrintMaker &xpm)
{
    if (this == &xpm)
        return *this;
    m_pages = xpm.m_pages;
    m_sm = xpm.m_sm;
    return *this;
}

XmlPrintMaker::XmlPrintMaker()
{
    m_pageHeight = m_pageHeightPortrate;
    m_maxLength = 0;
}

XmlPrintMaker::XmlPrintMaker(SizeMetrics *sm)
{
    m_maxLength = 0;
    m_pageHeight = m_pageHeightPortrate;
    m_sm = sm;
}

void XmlPrintMaker::setSizeMetrics(SizeMetrics *sm)
{
    m_sm = sm;
}

void XmlPrintMaker::text(const QString &text, const int &left, const int &top, const int &max_width)
{
    QFontMetrics fm(m_sm->font());
    int linesCount = 0;
    if (!max_width) {
        m_lastTextWidth = fm.width(text) / m_sm->m_fontScaleFactor;
        m_lastTextHeight = (fm.height() + fm.leading()) / m_sm->m_fontScaleFactor;
        linesCount = 1;
    } else {
        QStringList words = text.split(" ");
        m_lastTextWidth = 0;
        for (int i = 0, count = words.count(); i < count; i++) {
            m_lastTextWidth += fm.width(words[i]);
            if (i < count - 1) {
                if (m_lastTextWidth + fm.width(" ") < (max_width * m_sm->m_fontScaleFactor))
                    m_lastTextWidth += fm.width(" ");
            }
            if (m_lastTextWidth > (max_width * m_sm->m_fontScaleFactor)) {
                m_lastTextWidth = fm.width(words[i]);
                linesCount++;
            }
        }
        linesCount++;
        m_lastTextWidth = max_width * m_sm->m_fontScaleFactor;
        if (!linesCount)
            linesCount = 1;
        m_lastTextHeight = linesCount * ((fm.height()) / m_sm->m_fontScaleFactor);
    }
    m_body += QString("<text value=\"%1\" left=\"%2\" top=\"%3\" maxwidth=\"%4\" lines=\"%5\"></text>")
            .arg(text).arg(left).arg(top).arg(max_width).arg(linesCount);
    m_maxLength = m_lastTextWidth + left > m_maxLength ? m_lastTextWidth + left : m_maxLength;
}

void XmlPrintMaker::textRightAlign(const QString &text, const int &right, const int &top)
{
    int width = QFontMetrics(m_sm->font()).width(text) / m_sm->m_fontScaleFactor;
    XmlPrintMaker::text(text, right - width, top);
}

void XmlPrintMaker::textCenterAlign(const QString &text, const int &center, const int &top)
{
    int width = QFontMetrics(m_sm->font()).width(text) / m_sm->m_fontScaleFactor;
    XmlPrintMaker::text(text, (center / 2) - (width / 2), top);
}

int XmlPrintMaker::lastTextHeight() const
{
    return m_lastTextHeight;
}

int XmlPrintMaker::lastTextWidth() const
{
    return m_lastTextWidth;
}

int XmlPrintMaker::pageWidth()
{
    return m_maxLength;
}

void XmlPrintMaker::setPageHeight(int pageHeight)
{
    m_pageHeight = pageHeight;
}

void XmlPrintMaker::setPenWidth(const int &width)
{
    m_body += QString("<penwidth value=\"%1\"></penwidth>").arg(width);
}

void XmlPrintMaker::setPenStyle(const int &style)
{
    m_body += QString("<penstyle value=\"%1\"></penstyle>").arg(style);
}

void XmlPrintMaker::line(const int &left, const int &top, const int &right, const int &bottom)
{
    m_body += QString("<line left=\"%1\" top=\"%2\" right=\"%3\" bottom=\"%4\"></line>")
            .arg(left).arg(top).arg(right).arg(bottom);
    m_maxLength = right > m_maxLength ? right : m_maxLength;
}

int XmlPrintMaker::image(const QString &fileName, const int &left, const int &top, bool del)
{
    QPixmap p(fileName);
    m_body += QString("<image file=\"%1\" left=\"%2\" top=\"%3\" right=\"%4\" bottom=\"%5\" del=\"%6\"></image>")
            .arg(fileName)
            .arg(left).arg(top)
            .arg((p.width() / p.devicePixelRatio()) / m_sm->m_scaleFactor).arg(p.height() / m_sm->m_scaleFactor)
            .arg(del ? "yes" : "no");
    m_lastTextHeight = p.height() / m_sm->m_scaleFactor;
    return m_lastTextHeight;
}

int XmlPrintMaker::imageCenter(const QString &fileName, int top, int maxLeft, bool del)
{
    QPixmap p(fileName);
    int imgWidth = p.width() / m_sm->m_scaleFactor;
    int left = (maxLeft / 2) - (imgWidth / 2);
    m_lastTextHeight = p.height() / m_sm->m_scaleFactor;
    return image(fileName, left, top, del);
}

void XmlPrintMaker::finishPage()
{
    m_body = "<?xml version=\"1.0\" encoding=\"utf-8\"?><data>" + m_body + "</data>";
    m_pages.append(m_body);
    m_body.clear();
}

void XmlPrintMaker::setFontName(const QString &fontName)
{
    m_body += "<fontname value=\"" + fontName + "\"></fontname>";
    m_sm->font().setFamily(fontName);
}

void XmlPrintMaker::setFontSize(const int &pointSize)
{
    m_body += "<fontsize value=\"" + QString::number(pointSize) + "\"></fontsize>";
    m_sm->font().setPointSize(pointSize);
}

void XmlPrintMaker::setFontBold(const bool &bold)
{
    m_body += QString("<fontbold value=\"%1\"></fontbold>").arg(bold ? "1" : "0");
    m_sm->font().setBold(bold);
}

void XmlPrintMaker::setFontItalic(const bool &italic)
{
    m_body += QString("<fontitalic value=\"%1\"></fontitalic>").arg(italic ? "1" : "0");
    m_sm->font().setItalic(italic);
}

void XmlPrintMaker::setDefaultTextRect(const QRect &rect)
{
    m_defTextRect = rect;
}

QByteArray XmlPrintMaker::page(int number)
{
    return m_pages.at(number).toUtf8();
}

int XmlPrintMaker::pageCount()
{
    return m_pages.count();
}

void XmlPrintMaker::checkForNewPage(int &top)
{
    if (top > m_pageHeight) {
        top = 0;
        finishPage();
        setFontName(m_sm->font().family());
        setFontSize(m_sm->font().pointSize());
        setFontBold(m_sm->font().bold());
        setFontItalic(m_sm->font().italic());
    }
}

const QFont &XmlPrintMaker::font()
{
    return m_sm->font();
}

SizeMetrics::SizeMetrics()
{
    setResolution(96);
}

SizeMetrics::SizeMetrics(int resolution)
{
    setResolution(resolution);
}

void SizeMetrics::setResolution(int resolution)
{
    m_resolution = resolution;
    m_scaleFactor = (m_resolution * INCH_PER_MM);
    m_fontScaleFactor = (m_resolution * INCH_PER_MM) * 0.5;
}

void SizeMetrics::points(float *point, int count)
{
    for (int i = 0; i < count; i++)
        point[i] *= m_scaleFactor;
}

QFont &SizeMetrics::font()
{
    return m_font;
}

void SizeMetrics::reset()
{
    setResolution(96);
    m_font.setBold(false);
    m_font.setItalic(false);
}


XmlGraphicItem::XmlGraphicItem(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
}

void XmlGraphicItem::setPaintData(const QByteArray &data, SizeMetrics *sm)
{
    m_data = data;
    m_sm = sm;
}

void XmlGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    XmlPrinter p(painter, m_sm);
    p.render(m_data);
}

QRectF XmlGraphicItem::boundingRect() const
{
    return QRectF(0, 0, 210, 290);
}


ThreadPrinter::ThreadPrinter(const QString prnName, SizeMetrics sm, const XmlPrintMaker &x) :
    m_prnName(prnName),
    m_sm(sm),
    m_pm(x)
{
    connect(this, SIGNAL(finished()), SLOT(deleteLater()));
}

ThreadPrinter::~ThreadPrinter()
{
}

void ThreadPrinter::run()
{
    QMutexLocker ml(&m);
    QPrinter prn;
    prn.setPrinterName(m_prnName);
    prn.setResolution(m_sm.m_resolution);
    prn.setColorMode(QPrinter::GrayScale);
    qreal left, top, right, bottom;
    prn.getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);
    QPainter *p = new QPainter(&prn);
    XmlPrinter xp(p, &m_sm);
    xp.setMargins(left, top);
    for (int i = 0, count =  m_pm.pageCount(); i < count; i++) {
        if (i > 0)
            prn.newPage();
        xp.render(m_pm.page(i));
    }
    delete p;
    quit();
}


PrinterInfo::PrinterInfo()
{
    m_printers = QPrinterInfo::availablePrinters();
}

int PrinterInfo::resolution(const QString &printerName)
{
    Q_UNUSED(printerName)
    return 180;
    /*
    for (int i = 0; i < m_printers.count(); i++) {
        if (m_printers.at(i).printerName() == printerName) {
            if (m_printers.at(i).supportedResolutions().count())
                return m_printers.at(i).supportedResolutions().at(m_printers.at(i).supportedResolutions().count() - 1);
            else
                return 96;
            //return m_printers.at(i).supportedResolutions().at(0);
        }
    }
    return 96;
    */
}

bool PrinterInfo::printerExists(const QString &printerName)
{
    for (int i = 0; i < m_printers.count(); i++) {
        if (m_printers.at(i).printerName() == printerName)
            return true;
    }
    return false;
}
