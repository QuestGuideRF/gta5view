#ifndef PROFILEDATABASE_H
#define PROFILEDATABASE_H
#include <QSettings>
#include <QObject>
#include <QMutex>
#include <QMap>
class ProfileDatabase : public QObject
{
    Q_OBJECT
public:
    explicit ProfileDatabase(QObject *parent = 0);
    QString getPlayerName(QString playerID);
    QString getPlayerName(int playerID);
    QStringList getPlayers();
    ~ProfileDatabase();
private:
    mutable QMutex mutex;
    QSettings *profileDB;
public slots:
    void setPlayerName(int playerID, QString playerName);
};
#endif