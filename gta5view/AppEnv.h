#ifndef APPENV_H
#define APPENV_H
#include <QString>
#include <QUrl>
enum class GameVersion : int { NoVersion = 0, SocialClubVersion = 1, SteamVersion = 2, BothVersions = 3 };
enum class GameLanguage : int { Undefined = 0, English = 1, French = 2, Italian = 3, German = 4, Spanish = 5, Mexican = 6, Brasilian = 7, Russian = 8, Polish = 9, Japanese = 10, SChinese = 11, TChinese = 12, Korean = 13 };
class AppEnv
{
public:
    AppEnv();
    static QString getBuildDateTime();
    static QString getBuildCode();
    static QString getGameFolder(bool *ok = 0);
    static bool setGameFolder(QString gameFolder);
    static QString getExLangFolder();
    static QString getInLangFolder();
    static QString getImagesFolder();
    static QString getPluginsFolder();
    static QString getShareFolder();
    static QByteArray getUserAgent();
    static QUrl getCrewFetchingUrl(QString crewID);
    static QUrl getPlayerFetchingUrl(QString crewID, QString pageNumber);
    static QUrl getPlayerFetchingUrl(QString crewID, int pageNumber);
    static GameVersion getGameVersion();
    static GameLanguage getGameLanguage(GameVersion gameVersion);
    static GameLanguage gameLanguageFromString(QString gameLanguage);
    static QString gameLanguageToString(GameLanguage gameLanguage);
    static bool setGameLanguage(GameVersion gameVersion, GameLanguage gameLanguage);
    static qreal screenRatio();
    static qreal screenRatioPR();
};
#endif