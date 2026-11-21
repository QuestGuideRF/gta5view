#ifndef SAVEGAMEDATA_H
#define SAVEGAMEDATA_H
#include <QObject>
class SavegameData : public QObject
{
    Q_OBJECT
public:
    explicit SavegameData(const QString &fileName = "", QObject *parent = 0);
    bool readingSavegameFromFile(const QString &fileName);
    bool readingSavegame();
    bool isSavegameOk();
    QString getLastStep();
    QString getSavegameStr();
    QString getSavegameFileName();
    void setSavegameFileName(QString savegameFileName);
private:
    QString getSavegameDataString(const QByteArray &savegameHeader);
    QString savegameFileName;
    QString savegameStr;
    QString lastStep;
    bool savegameOk;
};
#endif