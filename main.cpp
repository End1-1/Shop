#include "dlogin.h"
#include <QApplication>
#include "qsystem.h"
#include <QFontDatabase>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFontDatabase fontDb;
    int fontId = fontDb.addApplicationFont(a.applicationDirPath() + "/ahuni.ttf");
    QString fontFamily = fontDb.applicationFontFamilies(fontId).at(0);
    QFont font = fontDb.font(fontFamily, "Normal", 11);
    a.setFont(font);
    QTranslator t;
    t.load(":/Shop.qm");
    a.installTranslator(&t);
    QSystem sys(argv);
    sys.hdmPass = argv[1];
    Q_UNUSED(sys);
    DLogin w;
    w.showFullScreen();

    return a.exec();
}
