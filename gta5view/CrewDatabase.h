#ifndef CREWDATABASE_H
#define CREWDATABASE_H
#include <QSettings>
#include <QObject>
#include <QMutex>
#include <QMap>
class CrewDatabase : public QObject
{
    Q_OBJECT
public:
    explicit CrewDatabase(QObject *parent = 0);
    QString getCrewName(QString crewID);
    QString getCrewName(int crewID);
    QStringList getCompatibleCrews();
    QStringList getCrews();
    void setAddingCrews(bool addingCrews);
    bool isCompatibleCrew(QString crewNID);
    bool isCompatibleCrew(int crewID);
    bool isAddingCrews();
    ~CrewDatabase();
private:
    mutable QMutex mutex;
    bool addProcess;
    QSettings *crewDB;
    QStringList getCrews_p();
    QStringList getCompatibleCrews_p();
public slots:
    void setCrewName(int crewID, QString crewName);
    void addCrew(int crewID);
};
#endif