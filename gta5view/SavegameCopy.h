#ifndef SAVEGAMECOPY_H
#define SAVEGAMECOPY_H
#include <QWidget>
#include <QString>
class SavegameCopy
{
public:
    SavegameCopy();
    static void copySavegame(QWidget *parent, QString sgdPath);
};
#endif