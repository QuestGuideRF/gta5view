#include "SidebarGenerator.h"
#include "StandardPaths.h"
#include "AppEnv.h"
#include <QList>
#include <QUrl>
#include <QDir>
SidebarGenerator::SidebarGenerator()
{
}
QList<QUrl> SidebarGenerator::generateSidebarUrls(QList<QUrl> sidebarUrls)
{
    QDir dir;
    dir.setPath(StandardPaths::picturesLocation());
    if (dir.exists())
    {
        sidebarUrls += QUrl::fromLocalFile(dir.absolutePath());
    }
    dir.setPath(StandardPaths::documentsLocation());
    if (dir.exists())
    {
        sidebarUrls += QUrl::fromLocalFile(dir.absolutePath());
    }
    bool gameFolderExists;
    QString gameFolder = AppEnv::getGameFolder(&gameFolderExists);
    if (gameFolderExists)
    {
        sidebarUrls += QUrl::fromLocalFile(gameFolder);
    }
    dir.setPath(StandardPaths::desktopLocation());
    if (dir.exists())
    {
        sidebarUrls += QUrl::fromLocalFile(dir.absolutePath());
    }
    return sidebarUrls;
}