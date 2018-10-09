#include "qsystem.h"

QString QSystem::appPath;
QString QSystem::hdmPass;

QSystem::QSystem(char **argv)
{
    appPath = argv[0];
    appPath = appPath.mid(0, appPath.lastIndexOf("\\"));
}

