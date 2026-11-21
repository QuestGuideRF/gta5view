#ifndef SIDEBARGENERATOR_H
#define SIDEBARGENERATOR_H
#include <QList>
#include <QUrl>
class SidebarGenerator
{
public:
    SidebarGenerator();
    static QList<QUrl> generateSidebarUrls(QList<QUrl> sidebarUrls);
};
#endif