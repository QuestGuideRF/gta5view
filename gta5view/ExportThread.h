#ifndef EXPORTTHREAD_H
#define EXPORTTHREAD_H
#include "SnapmaticWidget.h"
#include "SavegameWidget.h"
#include "ProfileWidget.h"
#include <QThread>
#include <QMap>
class ExportThread : public QThread
{
    Q_OBJECT
public:
    explicit ExportThread(QMap<ProfileWidget*,QString> profileMap, QString exportDirectory, bool pictureCopyEnabled, bool pictureExportEnabled, int exportCount, QObject *parent = 0);
    QStringList getFailedSavegames();
    QStringList getFailedCopyPictures();
    QStringList getFailedExportPictures();
protected:
    void run();
private:
    QMap <ProfileWidget*, QString> profileMap;
    QString exportDirectory;
    bool pictureCopyEnabled;
    bool pictureExportEnabled;
    int exportCount;
    QStringList failedSavegames;
    QStringList failedCopyPictures;
    QStringList failedExportPictures;
signals:
    void exportStringUpdate(QString currentFileName);
    void exportProgressUpdate(int currentProgressValue);
    void exportFinished();
};
#endif