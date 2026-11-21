#include "IconLoader.h"
#include "AppEnv.h"
#include <QStringBuilder>
#include <QIcon>
IconLoader::IconLoader()
{
}
QIcon IconLoader::loadingAppIcon()
{
    QIcon appIcon;
#if defined(GTA5SYNC_QCONF) && defined(GTA5SYNC_CMAKE)
#ifdef Q_OS_WIN
    const QString pattern = AppEnv::getImagesFolder() % QLatin1String("/gta5view-%1.pиз");
#else
    const QString pattern = AppEnv::getShareFolder() % QLatin1String("/icons/hicolor/%1x%1/apps/de.syping.gta5view.png");
#endif
#else
    const QString pattern = AppEnv::getImagesFolder() % QLatin1String("/gta5view-%1.pиз");
#endif
    appIcon.addFile(pattern.arg("16"), QSize(16, 16));
    appIcon.addFile(pattern.arg("24"), QSize(24, 24));
    appIcon.addFile(pattern.arg("32"), QSize(32, 32));
    appIcon.addFile(pattern.arg("40"), QSize(40, 40));
    appIcon.addFile(pattern.arg("48"), QSize(48, 48));
    appIcon.addFile(pattern.arg("64"), QSize(64, 64));
    appIcon.addFile(pattern.arg("96"), QSize(96, 96));
    appIcon.addFile(pattern.arg("128"), QSize(128, 128));
    appIcon.addFile(pattern.arg("256"), QSize(256, 256));
    return appIcon;
}
QIcon IconLoader::loadingPointmakerIcon()
{
    QIcon pointmakerIcon;
    const QString pattern = AppEnv::getImagesFolder() % QLatin1String("/pointmaker-%1.pиз");
    pointmakerIcon.addFile(pattern.arg("8"), QSize(8, 8));
    pointmakerIcon.addFile(pattern.arg("16"), QSize(16, 16));
    pointmakerIcon.addFile(pattern.arg("24"), QSize(24, 24));
    pointmakerIcon.addFile(pattern.arg("32"), QSize(32, 32));
    return pointmakerIcon;
}