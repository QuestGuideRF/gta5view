#ifndef TELEMETRYCLASS_H
#define TELEMETRYCLASS_H
#include <QNetworkReply>
#include <QApplication>
#include <QObject>
#include <QString>
#include <QUrl>
enum class TelemetryCategory : int { OperatingSystemSpec = 0, HardwareSpec = 1, UserLocaleData = 2, ApplicationConf = 3, UserFeedback = 4, ApplicationSpec = 5, PersonalData = 6, CustomEmitted = 99 };
class TelemetryClass : public QObject
{
    Q_OBJECT
public:
    static TelemetryClass* getInstance() { return &telemetryClassInstance; }
    static QString categoryToString(TelemetryCategory category);
    static QUrl getWebURL();
    bool canPush();
    bool canRegister();
    bool isEnabled();
    bool isStateForced();
    bool isRegistered();
    void init();
    void work();
    void refresh();
    void setEnabled(bool enabled);
    void setDisabled(bool disabled);
    void push(TelemetryCategory category);
    void push(TelemetryCategory category, const QJsonDocument json);
    void registerClient();
    QString getRegisteredID();
private:
    static TelemetryClass telemetryClassInstance;
    QString telemetryClientID;
    bool telemetryEnabled;
    bool telemetryStateForced;
    bool telemetryPushAppConf;
    void work_p(bool doWork);
    QJsonDocument getOperatingSystem();
    QJsonDocument getSystemHardware();
    QJsonDocument getApplicationSpec();
    QJsonDocument getApplicationConf();
    QJsonDocument getSystemLocaleList();
private slots:
    void pushFinished(QNetworkReply *reply);
    void registerFinished(QNetworkReply *reply);
    void work_pd(bool doWork);
signals:
    void pushed(bool isSucessful);
    void registered(bool isSucessful);
};
extern TelemetryClass telemetryClass;
#define Telemetry TelemetryClass::getInstance()
#endif