#ifndef PROFILELOADER_H
#define PROFILELOADER_H
#include "SnapmaticPicture.h"
#include "SavegameData.h"
#include "CrewDatabase.h"
#include <QThread>
#include <QDir>
class ProfileLoader : public QThread
{
    Q_OBJECT
public:
    explicit ProfileLoader(QString profileFolder, CrewDatabase *crewDB, QObject *parent = 0);
protected:
    void run();
private:
    QString profileFolder;
    CrewDatabase *crewDB;
    ProfileLoader *profileLoader;
private slots:
    void preloaded();
    void loaded();
signals:
    void pictureLoaded(SnapmaticPicture *picture);
    void pictureFixed(SnapmaticPicture *picture);
    void savegameLoaded(SavegameData *savegame, QString savegamePath);
    void loadingProgress(int value, int maximum);
    void directoryScanned(QVector<QString> savegameFiles, QVector<QString> snapmaticPics);
};
#endif