#include "StandardPaths.h"
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
QString StandardPaths::applicationsLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
#endif
}
QString StandardPaths::cacheLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif
}
QString StandardPaths::dataLocation()
{
#if QT_VERSION >= 0x060000
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#elif QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
}
QString StandardPaths::desktopLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
}
QString StandardPaths::documentsLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
}
QString StandardPaths::moviesLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);
#endif
}
QString StandardPaths::picturesLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#endif
}
QString StandardPaths::fontsLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::FontsLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::FontsLocation);
#endif
}
QString StandardPaths::homeLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
}
QString StandardPaths::musicLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
#endif
}
QString StandardPaths::tempLocation()
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation);
#endif
}