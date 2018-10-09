#-------------------------------------------------
#
# Project created by QtCreator 2015-10-31T22:41:15
#
#-------------------------------------------------

QT       += core gui sql xml printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shop
TEMPLATE = app


SOURCES += main.cpp\
        dlogin.cpp \
    dbdriver.cpp \
    dconfirmation.cpp \
    dquickgoods.cpp \
    dqty.cpp \
    printing.cpp \
    qsystem.cpp \
    dlgselecttaxdep.cpp \
    dlgcashcalc.cpp \
    ../NewTax/printtaxn.cpp

HEADERS  += dlogin.h \
    dbdriver.h \
    dconfirmation.h \
    dquickgoods.h \
    dqty.h \
    printing.h \
    qsystem.h \
    dlgselecttaxdep.h \
    dlgcashcalc.h \
    ../NewTax/printtaxn.h

FORMS    += dlogin.ui \
    dconfirmation.ui \
    dquickgoods.ui \
    dqty.ui \
    dlgselecttaxdep.ui \
    dlgcashcalc.ui

CONFIG += static

RESOURCES += \
    res.qrc

RC_FILE = rc.rc

ICON = app.ico

LIBS += -lwsock32
LIBS += -LC:/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -lVersion

INCLUDEPATH += C:/Qt/projects/ShopControl
INCLUDEPATH += C:/Qt/projects/NewTax
INCLUDEPATH += C:/OpenSSL-Win32.0/include
INCLUDEPATH += C:/OpenSSL-Win32.0/include/openssl
