#include "SnapmaticPicture.h"
#include "StringParser.h"
#include "SavegameData.h"
#include <QStringBuilder>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#define savegameHeaderLength 260
#define verificationValue QByteArray::fromHex("00000001")
SavegameData::SavegameData(const QString &fileName, QObject *parent) : QObject(parent), savegameFileName(fileName)
{
    savegameStr = "";
    savegameOk = 0;
}
bool SavegameData::readingSavegame()
{
    QFile *saveFile = new QFile(savegameFileName);
    if (!saveFile->open(QFile::ReadOnly))
    {
        lastStep = "1;/1,ОткрытьФайл," % SnapmaticPicture::convertDrawStringForLog(savegameFileName);
        saveFile->deleteLater();
        delete saveFile;
        return false;
    }
    if (!saveFile->isReadable())
    {
        lastStep = "2;/3,Чтение файла," % SnapmaticPicture::convertDrawStringForLog(savegameFileName) % ",1,БЕЗ ЗАГОЛОВКА";
        saveFile->close();
        saveFile->deleteLater();
        delete saveFile;
        return false;
    }
    QByteArray savegameHeaderLine = saveFile->read(savegameHeaderLength);
    if (savegameHeaderLine.left(4) == verificationValue)
    {
        savegameStr = getSavegameDataString(savegameHeaderLine);
        if (savegameStr.length() >= 1)
        {
            savegameOk = true;
        }
    }
    saveFile->close();
    saveFile->deleteLater();
    delete saveFile;
    return savegameOk;
}
QString SavegameData::getSavegameDataString(const QByteArray &savegameHeader)
{
    QByteArray savegameBytes = savegameHeader.left(savegameHeaderLength);
    QList<QByteArray> savegameBytesList = savegameBytes.split(char(0x01));
    savegameBytes = savegameBytesList.at(1);
    savegameBytesList.clear();
    return SnapmaticPicture::parseTitleString(savegameBytes, savegameBytes.length());
}
bool SavegameData::readingSavegameFromFile(const QString &fileName)
{
    if (fileName != "")
    {
        savegameFileName = fileName;
        return readingSavegame();
    }
    else
    {
        return false;
    }
}
bool SavegameData::isSavegameOk()
{
    return savegameOk;
}
QString SavegameData::getSavegameFileName()
{
    return savegameFileName;
}
QString SavegameData::getSavegameStr()
{
    return savegameStr;
}
QString SavegameData::getLastStep()
{
    return lastStep;
}
void SavegameData::setSavegameFileName(QString savegameFileName_)
{
    savegameFileName = savegameFileName_;
}