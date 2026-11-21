#ifndef DATABASETHREAD_H
#define DATABASETHREAD_H
#include "CrewDatabase.h"
#include <QObject>
#include <QThread>
class DatabaseThread : public QThread
{
    Q_OBJECT
public:
    explicit DatabaseThread(CrewDatabase *crewDB, QObject *parent = 0);
public slots:
    void terminateThread();
private:
    CrewDatabase *crewDB;
    void scanCrewMembersList(const QStringList &crewList, const int &maxPages, const int &requestDelay);
    void scanCrewReference(const QStringList &crewList, const int &requestDelay);
    void deleteCompatibleCrews(QStringList *crewList);
    QStringList deleteCompatibleCrews(const QStringList &crewList);
    bool continueLastCrew;
    bool threadRunning;
    int plyrPerReq;
protected:
    void run();
signals:
    void crewNameFound(int crewID, QString crewName);
    void crewNameUpdated();
    void playerNameFound(int playerID, QString playerName);
    void playerNameUpdated();
    void threadTerminated();
};
#endif