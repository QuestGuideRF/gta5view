#ifndef ICONLOADER_H
#define ICONLOADER_H
#include <QIcon>
class IconLoader
{
public:
    IconLoader();
    static QIcon loadingAppIcon();
    static QIcon loadingPointmakerIcon();
};
#endif