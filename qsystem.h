#ifndef QSYSTEM_H
#define QSYSTEM_H

#include <QString>

class QSystem
{
public:
    QSystem(char **argv);
    static QString appPath;
    static QString hdmPass;
};

#endif // QSYSTEM_H
