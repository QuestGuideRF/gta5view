#ifndef PICTUREEXPORT_H
#define PICTUREEXPORT_H
#include "SnapmaticPicture.h"
#include <QWidget>
#include <QString>
class PictureExport
{
public:
    PictureExport();
    static void exportAsPicture(QWidget *parent, SnapmaticPicture *picture);
    static void exportAsSnapmatic(QWidget *parent, SnapmaticPicture *picture);
    static QString getPictureFileName(SnapmaticPicture *picture);
};
#endif