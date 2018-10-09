#ifndef PRINTING_H
#define PRINTING_H

#include <QXmlDefaultHandler>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrinterInfo>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#define INCH_PER_MM 0.0393
//#define INCH_PER_MM 0.02
#define SIZE_A4_PORTRAIT QSize(210, 290)
#define SIZE_A4_LANDSCAPE QSize(290, 210)
#define SIZE_P1 QSize(80, 290)


class PrinterInfo {
public:
    QMap<QString, int> m_resolutions;
    QList<QPrinterInfo> m_printers;

public:
    PrinterInfo();
    int resolution(const QString &printerName);
    bool printerExists(const QString &printerName);
};


extern PrinterInfo *___printerInfo;

class SizeMetrics {
protected:
    friend class XmlPrintMaker;
    friend class XmlPrinter;
    float m_scaleFactor;
    float m_fontScaleFactor;
    QFont m_font;

public:
    SizeMetrics();
    SizeMetrics(int resolution);
    int m_resolution;
    void setResolution(int resolution);
    void points(float *point, int count);
    QFont &font();
    void reset();
};

class XmlPrinter : public QXmlDefaultHandler {
private:
    QPen m_pen;
    QPainter *m_painter;
    SizeMetrics *m_sm;
    int m_marginLeft, m_marginTop;

public:
    XmlPrinter(QPainter *painter, SizeMetrics *sm);
    void render(const QByteArray &data);
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
    bool error(const QXmlParseException &exception);
    void setMargins(int marginLeft, int marginTop);
    QFont &font();
};

class XmlGraphicItem : public QGraphicsItem {
private:
    QByteArray m_data;
    SizeMetrics *m_sm;

public:
    XmlGraphicItem(QGraphicsItem *parent);
    void setPaintData(const QByteArray &data, SizeMetrics *sm);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;
};

class XmlPrintMaker {
private:
    QString m_body;
    QStringList m_pages;
    QRect m_defTextRect;
    SizeMetrics *m_sm;
    int m_lastTextHeight;
    int m_lastTextWidth;
    int m_maxLength;
    int m_pageHeight;

public:
    XmlPrintMaker &operator =(const XmlPrintMaker &xpm);
    static const int m_PageHeightLandscape = 195;
    static const int m_PageWidthLandscape = 265;
    static const int m_pageHeightPortrate = 265;
    static const int m_pageWidthProtrate = 195;

    XmlPrintMaker();
    XmlPrintMaker(SizeMetrics *sm);
    void setSizeMetrics(SizeMetrics *sm);

    void setFontName(const QString &fontName);
    void setFontSize(const int &pointSize);
    void setFontBold(const bool &bold);
    void setFontItalic(const bool &italic);

    void setDefaultTextRect(const QRect &rect);
    void text(const QString &text, const int &left, const int &top, const int &max_width = 0);
    void textRightAlign(const QString &text, const int &right, const int &top);
    void textCenterAlign(const QString &text, const int &center, const int &top);
    int lastTextHeight() const;
    int lastTextWidth() const;
    int pageWidth();
    void setPageHeight(int pageHeight);

    void setPenWidth(const int &width);
    void setPenStyle(const int &style);

    void line(const int &left, const int &top, const int &right, const int &bottom);

    int image(const QString &fileName, const int &left, const int &top, bool del = false);
    int imageCenter(const QString &fileName, int top, int maxLeft, bool del = false);

    void finishPage();
    QByteArray page(int number);
    int pageCount();
    void checkForNewPage(int &top);
    const QFont &font();
};

class ThreadPrinter : public QThread {
private:
    QString m_prnName;
    SizeMetrics m_sm;
    XmlPrintMaker m_pm;
public:
    ThreadPrinter(const QString prnName, SizeMetrics sm, const XmlPrintMaker &x);
    ~ThreadPrinter();
    void run();
};

#endif // PRINTING_H
