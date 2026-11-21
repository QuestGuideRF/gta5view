#include "TelemetryClassAuthenticator.h"
#include <QUrlQuery>
#include <QUrl>
#ifndef GTA5SYNC_TELEMETRY_PUSHURL
#define GTA5SYNC_TELEMETRY_PUSHURL ""
#endif
#ifndef GTA5SYNC_TELEMETRY_REGURL
#define GTA5SYNC_TELEMETRY_REGURL ""
#endif
#ifndef GTA5SYNC_TELEMETRY_AUTHID
#define GTA5SYNC_TELEMETRY_AUTHID ""
#endif
#ifndef GTA5SYNC_TELEMETRY_AUTHPW
#define GTA5SYNC_TELEMETRY_AUTHPW ""
#endif
const QUrl TelemetryClassAuthenticator::getTrackingPushURL()
{
    if (haveAccessData())
    {
        QUrl pushUrl(GTA5SYNC_TELEMETRY_PUSHURL);
        QUrlQuery pushQuery(pushUrl);
        if (!getTrackingAuthID().isEmpty()) { pushQuery.addQueryItem("tid", getTrackingAuthID()); }
        if (!getTrackingAuthPW().isEmpty()) { pushQuery.addQueryItem("tpw", getTrackingAuthPW()); }
        pushUrl.setQuery(pushQuery.query(QUrl::FullyEncoded));
        return pushUrl;
    }
    else
    {
        QUrl pushUrl(GTA5SYNC_TELEMETRY_PUSHURL);
        return pushUrl;
    }
}
const QUrl TelemetryClassAuthenticator::getTrackingRegURL()
{
    if (haveAccessData())
    {
        QUrl regUrl(GTA5SYNC_TELEMETRY_REGURL);
        QUrlQuery regQuery(regUrl);
        if (!getTrackingAuthID().isEmpty()) { regQuery.addQueryItem("tid", getTrackingAuthID()); }
        if (!getTrackingAuthPW().isEmpty()) { regQuery.addQueryItem("tpw", getTrackingAuthPW()); }
        regUrl.setQuery(regQuery.query(QUrl::FullyEncoded));
        return regUrl;
    }
    else
    {
        QUrl regUrl(GTA5SYNC_TELEMETRY_REGURL);
        return regUrl;
    }
}
const QString TelemetryClassAuthenticator::getTrackingAuthID()
{
    return QString(GTA5SYNC_TELEMETRY_AUTHID);
}
const QString TelemetryClassAuthenticator::getTrackingAuthPW()
{
    return QString(GTA5SYNC_TELEMETRY_AUTHPW);
}
bool TelemetryClassAuthenticator::havePushURL()
{
    return !getTrackingPushURL().isEmpty();
}
bool TelemetryClassAuthenticator::haveRegURL()
{
    return !getTrackingRegURL().isEmpty();
}
bool TelemetryClassAuthenticator::haveAccessData()
{
    if (getTrackingAuthID().isEmpty() && getTrackingAuthPW().isEmpty()) { return false; }
    return true;
}