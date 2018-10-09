#ifndef DBDRIVER_H
#define DBDRIVER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <QVariant>
#include <QDebug>
#include <QSqlRecord>
#include <QDateTime>
#include <QDate>

#define DATE_FORMAT "dd.MM.yyyy"
#define DATETIME_FORMAT "dd.MM.yyyy HH:mm:ss"

#include <QObject>

class DbDriver : QObject
{
    Q_OBJECT

private:
    QSqlQuery *m_query;
    bool m_errorFlag;
    int m_dbnumber;
    static int m_number;

protected:
    QSqlDatabase m_db;
    void log(const QString &message);

public:
    DbDriver();
    ~DbDriver();
    void configureDb(const QString &dbPath, const QString &dbUser, const QString &dbPass);
    bool openDB();
    void closeDB();
    bool prepare(const QString &sql);
    bool bindValue(const QString &name, const QVariant &value);
    bool execSQL();
    bool execSQL(const QString &sql);
    bool next();
    QSqlRecord record();
    int genId(const QString &name);
    QVariant v(int index);
    QString v_str(int index);
    int v_int(int index);
    qreal v_dbl(int index);
    QDateTime v_dateTime(int index);
    QDate v_date(int index);
    QString lastQuery();

signals:
    void errorMsg(const QString &msg);
};

extern QStringList m_lastQuery;

#endif // DBDRIVER_H
