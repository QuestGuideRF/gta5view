#ifndef SNAPMATICPICTURE_H
#define SNAPMATICPICTURE_H
#include "RagePhoto.h"
#include <QStringList>
#include <QDateTime>
#include <QObject>
#include <QString>
#include <QImage>
#include <QFile>
enum class SnapmaticFormat : int { Auto_Format = 0, PGTA_Format = 1, JPEG_Format = 2, G5E_Format = 3 };
struct SnapmaticProperties {
    struct SnapmaticLocation {
        QString area;
        double x;
        double y;
        double z;
        bool isCayoPerico;
    };
    int uid;
    int crewID;
    int streetID;
    QStringList playersList;
    uint createdTimestamp;
    QDateTime createdDateTime;
    bool isMeme;
    bool isMug;
    bool isSelfie;
    bool isFromDirector;
    bool isFromRSEditor;
    SnapmaticLocation location;
};
class SnapmaticPicture : public QObject
{
    Q_OBJECT
public:
    explicit SnapmaticPicture(const QString &fileName = "", QObject *parent = 0);
    ~SnapmaticPicture();
    void reset();
    bool preloadFile();
    bool readingPictureFromFile(const QString &fileName, bool cacheEnabled = false);
    bool readingPicture(bool cacheEnabled = false);
    bool isPicOk();
    void clearCache();
    QImage getImage();
    QByteArray getPictureStream();
    QString getLastStep(bool readable = true);
    QString getPictureStr();
    QString getPictureTitl();
    QString getPictureSortStr();
    QString getPictureFileName();
    QString getPictureFilePath();
    QString getExportPictureFileName();
    QString getOriginalPictureFileName();
    QString getOriginalPictureFilePath();
    int getContentMaxLength();
    bool setImage(const QImage &picture, bool eXtendMode = false);
    bool setPictureTitl(const QString &newTitle);
    bool setPictureStream(const QByteArray &streamArray);
    void updateStrings();
    void emitUpdate();
    void emitCustomSignal(const QString &signal);
    bool exportPicture(const QString &fileName, SnapmaticFormat format = SnapmaticFormat::Auto_Format);
    void setPicFileName(const QString &picFileName);
    void setPicFilePath(const QString &picFilePath);
    bool deletePicFile();
    bool isJsonOk();
    QString getJsonStr();
    SnapmaticProperties getSnapmaticProperties();
    bool setSnapmaticProperties(SnapmaticProperties properties);
    bool setJsonStr(const QString &jsonStr, bool updateProperties = false);
    bool isHidden();
    bool isVisible();
    bool setPictureHidden();
    bool setPictureVisible();
    QString getJsonString() { return getJsonStr(); }
    QString getPictureJson() { return getJsonStr(); }
    QString getPictureTitle() { return getPictureTitl(); }
    QString getPictureString() { return getPictureStr(); }
    bool setJsonString(const QString &jsonString, bool updateProperties = false) { return setJsonStr(jsonString, updateProperties); }
    bool setPictureJson(const QString &json, bool updateProperties = false) { return setJsonStr(json, updateProperties); }
    bool setPictureTitle(const QString &title) { return setPictureTitl(title); }
    void setPictureFileName(const QString &fileName) { return setPicFileName(fileName); }
    void setPictureFilePath(const QString &filePath) { return setPicFilePath(filePath); }
    bool deletePictureFile() { return deletePicFile(); }
    bool isPictureOk() { return isPicOk(); }
    bool isPictureHidden() { return isHidden(); }
    bool isPictureVisible() { return isVisible(); }
    bool setHidden() { return setPictureHidden(); }
    bool setVisible() { return setPictureVisible(); }
    static QSize getSnapmaticResolution();
    SnapmaticFormat getSnapmaticFormat();
    void setSnapmaticFormat(SnapmaticFormat format);
    bool isFormatSwitched();
    static bool verifyTitle(const QString &title);
    static QString parseTitleString(const QByteArray &commitBytes, int maxLength);
    static QString convertDrawStringForLog(const QString &inputStr);
    static QString convertLogStringForDraw(const QString &inputStr);
    RagePhoto* ragePhoto();
private:
    QImage cachePicture;
    QString picExportFileName;
    QString picFileName;
    QString picFilePath;
    QString pictureStr;
    QString lastStep;
    QString sortStr;
    bool picOk;
    bool cacheEnabled;
    bool isFormatSwitch;
    void parseJsonContent();
    bool jsonOk;
    SnapmaticProperties localProperties;
    static bool verifyTitleChar(const QChar &titleChar);
    RagePhoto p_ragePhoto;
signals:
    void customSignal(QString signal);
    void preloaded();
    void updated();
    void loaded();
public slots:
};
#endif