#ifndef TELEMETRYCLASSAUTHENTICATOR_H
#define TELEMETRYCLASSAUTHENTICATOR_H
#include <QApplication>
#include <QObject>
#include <QString>
#include <QUrl>
class TelemetryClassAuthenticator : public QObject
{
    Q_OBJECT
public:
    static const QUrl getTrackingPushURL();
    static const QUrl getTrackingRegURL();
    static const QString getTrackingAuthID();
    static const QString getTrackingAuthPW();
    static bool havePushURL();
    static bool haveRegURL();
    static bool haveAccessData();
};
#endif