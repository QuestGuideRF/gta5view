#include "DatabaseThread.h"
#include "CrewDatabase.h"
#include "AppEnv.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QVariantMap>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QUrl>
#define crewMaxPages 83
#define maxLoadFails 3
DatabaseThread::DatabaseThread(CrewDatabase *crewDB, QObject *parent) : QThread(parent), crewDB(crewDB)
{
    continueLastCrew = true;
    threadRunning = true;
}
void DatabaseThread::run()
{
    QEventLoop threadLoop;
    QObject::connect(this, SIGNAL(threadTerminated()), &threadLoop, SLOT(quit()));
    while (threadRunning) {
        QTimer::singleShot(300000, &threadLoop, SLOT(quit()));
        threadLoop.exec();
    }
}
void DatabaseThread::scanCrewReference(const QStringList &crewList, const int &requestDelay)
{
    for (const QString &crewID : crewList) {
        if (threadRunning && crewID != QLatin1String("0")) {
            QNetworkAccessManager *netManager = new QNetworkAccessManager();
            QNetworkRequest netRequest(AppEnv::getCrewFetchingUrl(crewID));
#if QT_VERSION >= 0x050600
#if QT_VERSION < 0x060000
            netRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif
#endif
            netRequest.setRawHeader("User-Agent", AppEnv::getUserAgent());
            netRequest.setRawHeader("Accept", "text/html");
            netRequest.setRawHeader("Accept-Charset", "utf-8");
            netRequest.setRawHeader("Accept-Language", "ru-US,en;q=0,9");
            netRequest.setRawHeader("Connection", "keep-alive");
            QNetworkReply *netReply = netManager->get(netRequest);
            QEventLoop *downloadLoop = new QEventLoop();
            QObject::connect(netReply, SIGNAL(finished()), downloadLoop, SLOT(quit()));
            if (!continueLastCrew)
                QObject::connect(this, SIGNAL(threadTerminated()), downloadLoop, SLOT(quit()));
            QTimer::singleShot(30000, downloadLoop, SLOT(quit()));
            downloadLoop->exec();
            downloadLoop->disconnect();
            delete downloadLoop;
            if (netReply->isFinished()) {
                QString crewName;
                QByteArray crewHtml = netReply->readAll();
                QStringList crewHtmlSplit1 = QString::fromUtf8(crewHtml).split("<title>Rockstar Games Social Club – Команда: ");
                if (crewHtmlSplit1.length() >= 2) {
                    QStringList crewHtmlSplit2 = QString(crewHtmlSplit1.at(1)).split("</title>");
                    if (crewHtmlSplit2.length() >= 1) {
                        crewName = crewHtmlSplit2.at(0);
                    }
                }
                if (!crewName.isEmpty()) {
                    emit crewNameFound(crewID.toInt(), crewName);
                }
            }
            else {
                netReply->abort();
            }
            if (threadRunning) {
                QEventLoop *waitingLoop = new QEventLoop();
                QTimer::singleShot(requestDelay, waitingLoop, SLOT(quit()));
                if (!continueLastCrew)
                    QObject::connect(this, SIGNAL(threadTerminated()), waitingLoop, SLOT(quit()));
                waitingLoop->exec();
                waitingLoop->disconnect();
                delete waitingLoop;
            }
            delete netReply;
            delete netManager;
        }
    }
}
void DatabaseThread::scanCrewMembersList(const QStringList &crewList, const int &maxPages, const int &requestDelay)
{
    for (const QString &crewID : crewList) {
        if (threadRunning && crewID != QLatin1String("0")) {
            int currentFail = 0;
            int currentPage = 0;
            int foundPlayers = 0;
            int totalPlayers = 1000;
            while(foundPlayers < totalPlayers && currentPage < maxPages && (continueLastCrew ? true : threadRunning)) {
                QNetworkAccessManager *netManager = new QNetworkAccessManager();
                QNetworkRequest netRequest(AppEnv::getPlayerFetchingUrl(crewID, currentPage));
#if QT_VERSION >= 0x050600
#if QT_VERSION < 0x060000
                netRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif
#endif
                netRequest.setRawHeader("User-Agent", AppEnv::getUserAgent());
                netRequest.setRawHeader("Accept", "application/json");
                netRequest.setRawHeader("Accept-Charset", "utf-8");
                netRequest.setRawHeader("Accept-Language", "ru-US,en;q=0,9");
                netRequest.setRawHeader("Connection", "keep-alive");
                QNetworkReply *netReply = netManager->get(netRequest);
                QEventLoop *downloadLoop = new QEventLoop();
                QObject::connect(netReply, SIGNAL(finished()), downloadLoop, SLOT(quit()));
                if (!continueLastCrew)
                    QObject::connect(this, SIGNAL(threadTerminated()), downloadLoop, SLOT(quit()));
                QTimer::singleShot(30000, downloadLoop, SLOT(quit()));
                downloadLoop->exec();
                downloadLoop->disconnect();
                delete downloadLoop;
                if (netReply->isFinished()) {
                    QByteArray crewJson = netReply->readAll();
                    QJsonDocument crewDocument = QJsonDocument::fromJson(crewJson);
                    QJsonObject crewObject = crewDocument.object();
                    QVariantMap crewMap = crewObject.toVariantMap();
                    if (crewMap.contains("Total")) { totalPlayers = crewMap["Total"].toInt(); }
                    if (crewMap.contains("Members")) {
                        const QList<QVariant> memberList = crewMap["Members"].toList();
                        for (const QVariant &memberVariant : memberList) {
                            QMap<QString, QVariant> memberMap = memberVariant.toMap();
                            if (memberMap.contains("RockstarId") && memberMap.contains("Name")) {
                                int RockstarId = memberMap["RockstarId"].toInt();
                                QString memberName = memberMap["Name"].toString();
                                if (!memberName.isEmpty() && RockstarId != 0) {
                                    foundPlayers++;
                                    emit playerNameFound(RockstarId, memberName);
                                }
                            }
                        }
                    }
                    currentPage++;
                }
                else {
                    currentFail++;
                    if (currentFail == maxLoadFails) {
                        currentFail = 0;
                        currentPage++;
                    }
                }
                delete netReply;
                delete netManager;
                if (foundPlayers < totalPlayers && currentPage < maxPages && (continueLastCrew ? true : threadRunning)) {
                    QEventLoop *waitingLoop = new QEventLoop();
                    QTimer::singleShot(requestDelay, waitingLoop, SLOT(quit()));
                    if (!continueLastCrew) { QObject::connect(this, SIGNAL(threadTerminated()), waitingLoop, SLOT(quit())); }
                    waitingLoop->exec();
                    waitingLoop->disconnect();
                    delete waitingLoop;
                }
            }
        }
    }
}
void DatabaseThread::deleteCompatibleCrews(QStringList *crewList)
{
    for (const QString &crewNID : *crewList) {
        if (crewDB->isCompatibleCrew(crewNID)) {
            crewList->removeAll(crewNID);
        }
    }
}
QStringList DatabaseThread::deleteCompatibleCrews(const QStringList &crewList)
{
    QStringList crewListR = crewList;
    for (const QString &crewNID : crewListR) {
        if (crewDB->isCompatibleCrew(crewNID)) {
            crewListR.removeAll(crewNID);
        }
    }
    return crewListR;
}
void DatabaseThread::terminateThread()
{
    threadRunning = false;
    emit threadTerminated();
}