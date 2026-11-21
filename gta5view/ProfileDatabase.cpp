#include "ProfileDatabase.h"
#include "StandardPaths.h"
#include "config.h"
#include <QStringBuilder>
#include <QMutexLocker>
#include <QDebug>
#include <QFile>
#include <QDir>
ProfileDatabase::ProfileDatabase(QObject *parent) : QObject(parent)
{
    QDir dir;
    dir.mkpath(StandardPaths::dataLocation());
    dir.setPath(StandardPaths::dataLocation());
    QString dirPath = dir.absolutePath();
    QString defaultConfPath = dirPath % "/players.ini";
    QSettings confPathSettings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);
    confPathSettings.beginGroup("Database");
    QString confPathFile = confPathSettings.value("Players", defaultConfPath).toString();
    confPathSettings.endGroup();
    profileDB = new QSettings(confPathFile, QSettings::IniFormat);
    profileDB->beginGroup("Players");
}
ProfileDatabase::~ProfileDatabase()
{
    profileDB->endGroup();
    delete profileDB;
}
QStringList ProfileDatabase::getPlayers()
{
    QMutexLocker locker(&mutex);
#ifdef GTA5SYNC_DEBUG
    qDebug() << "getPlayers";
#endif
    return profileDB->childKeys();
}
QString ProfileDatabase::getPlayerName(QString playerID)
{
    QMutexLocker locker(&mutex);
#ifdef GTA5SYNC_DEBUG
    qDebug() << "getPlayerName" << playerID;
#endif
    return profileDB->value(playerID, playerID).toString();
}
QString ProfileDatabase::getPlayerName(int playerID)
{
    QMutexLocker locker(&mutex);
#ifdef GTA5SYNC_DEBUG
    qDebug() << "getPlayerName" << playerID;
#endif
    return profileDB->value(QString::number(playerID), playerID).toString();
}
void ProfileDatabase::setPlayerName(int playerID, QString playerName)
{
    QMutexLocker locker(&mutex);
#ifdef GTA5SYNC_DEBUG
    qDebug() << "setPlayerName" << playerID << playerName;
#endif
    profileDB->setValue(QString::number(playerID), playerName);
}