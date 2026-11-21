#ifndef STANDARDPATHS_H
#define STANDARDPATHS_H
#include <QString>
class StandardPaths
{
public:
    static QString applicationsLocation();
    static QString cacheLocation();
    static QString dataLocation();
    static QString desktopLocation();
    static QString documentsLocation();
    static QString fontsLocation();
    static QString homeLocation();
    static QString moviesLocation();
    static QString picturesLocation();
    static QString musicLocation();
    static QString tempLocation();
};
#endif