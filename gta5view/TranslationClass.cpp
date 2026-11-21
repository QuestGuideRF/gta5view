#include "TranslationClass.h"
#include "AppEnv.h"
#include "config.h"
#include <QStringBuilder>
#include <QApplication>
#include <QStringList>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include <QDebug>
#include <QFile>
#include <QDir>
#if QT_VERSION >= 0x050000
#define QtBaseTranslationFormat "qtbase_"
#else
#define QtBaseTranslationFormat "qt_"
#endif
TranslationClass TranslationClass::translationClassInstance;
void TranslationClass::initUserLanguage()
{
    QSettings settings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);
    settings.beginGroup("Interface");
    userLanguage = settings.value("Language", "System").toString();
    userAreaLanguage = settings.value("AreaLanguage", "Auto").toString();
    settings.endGroup();
}
void TranslationClass::loadTranslation(QApplication *app)
{
    if (isLangLoaded) {
        unloadTranslation(app);
    }
    else {
        currentLangIndex = 0;
    }
    const QString exLangPath = AppEnv::getExLangFolder();
    const QString inLangPath = AppEnv::getInLangFolder();
    if (userLanguage == "en" || userLanguage == "en_GB") {
        currentLanguage = "en_GB";
        if (loadQtTranslation_p(exLangPath, &exQtTranslator)) {
            app->installTranslator(&exQtTranslator);
        }
        else if (loadQtTranslation_p(inLangPath, &inQtTranslator)) {
            app->installTranslator(&inQtTranslator);
        }
#if QT_VERSION >= 0x060000
        QLocale::setDefault(QLocale(currentLanguage));
#else
        QLocale::setDefault(currentLanguage);
#endif
        isLangLoaded = true;
        return;
    }
#ifndef GTA5SYNC_QCONF
    QString externalLanguageStr;
    bool externalLanguageReady = false;
    bool externalEnglishMode = false;
    bool loadInternalLang = false;
    bool trLoadSuccess = false;
    if (isUserLanguageSystem_p()) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadExSystemLanguage";
#endif
        trLoadSuccess = loadSystemTranslation_p(exLangPath, &exAppTranslator);
    }
    else {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadExUserLanguage";
#endif
        trLoadSuccess = loadUserTranslation_p(exLangPath, &exAppTranslator);
        if (!trLoadSuccess) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "loadInUserLanguage";
#endif
            trLoadSuccess = loadUserTranslation_p(inLangPath, &inAppTranslator);
            if (!trLoadSuccess) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadUserLanguageFailed";
#endif
            }
            else {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadUserLanguageSuccess";
#endif
                loadInternalLang = true;
                isLangLoaded = true;
            }
        }
        else {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "loadUserLanguageSuccess";
#endif
            isLangLoaded = true;
        }
    }
    if (trLoadSuccess) {
        if (currentLangIndex != 0 || isEnglishMode) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "externalLanguageReady" << currentLanguage;
#endif
            externalEnglishMode = isEnglishMode;
            externalLanguageStr = currentLanguage;
            externalLanguageReady = true;
        }
        else {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "installTranslation";
#endif
            if (loadInternalLang) {
                app->installTranslator(&inAppTranslator);
            }
            else {
                app->installTranslator(&exAppTranslator);
            }
            if (loadQtTranslation_p(exLangPath, &exQtTranslator)) {
                app->installTranslator(&exQtTranslator);
            }
            else if (loadQtTranslation_p(inLangPath, &inQtTranslator)) {
                app->installTranslator(&inQtTranslator);
            }
#if QT_VERSION >= 0x060000
            QLocale::setDefault(QLocale(currentLanguage));
#else
            QLocale::setDefault(currentLanguage);
#endif
            isLangLoaded = true;
        }
    }
    if (externalLanguageReady) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadInSystemLanguage";
#endif
        int externalLangIndex = currentLangIndex;
        trLoadSuccess = loadSystemTranslation_p(inLangPath, &inAppTranslator);
#ifdef GTA5SYNC_DEBUG
        qDebug() << "externalLangIndex" << externalLangIndex << "internalLangIndex" << currentLangIndex;
        qDebug() << "externalEnglishMode" << externalEnglishMode << "internalEnglishMode" << isEnglishMode;
#endif
        if ((trLoadSuccess && externalLangIndex > currentLangIndex) || (trLoadSuccess && externalEnglishMode && !isEnglishMode)) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "installInternalTranslation";
#endif
            app->installTranslator(&inAppTranslator);
            if (loadQtTranslation_p(exLangPath, &exQtTranslator)) {
                app->installTranslator(&exQtTranslator);
            }
            else if (loadQtTranslation_p(inLangPath, &inQtTranslator)) {
                app->installTranslator(&inQtTranslator);
            }
#if QT_VERSION >= 0x060000
            QLocale::setDefault(QLocale(currentLanguage));
#else
            QLocale::setDefault(currentLanguage);
#endif
            isLangLoaded = true;
        }
        else {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "installExternalTranslation";
#endif
            isEnglishMode = externalEnglishMode;
            currentLanguage = externalLanguageStr;
            app->installTranslator(&exAppTranslator);
            if (loadQtTranslation_p(exLangPath, &exQtTranslator)) {
                app->installTranslator(&exQtTranslator);
            }
            else if (loadQtTranslation_p(inLangPath, &inQtTranslator)) {
                app->installTranslator(&inQtTranslator);
            }
#if QT_VERSION >= 0x060000
            QLocale::setDefault(QLocale(currentLanguage));
#else
            QLocale::setDefault(currentLanguage);
#endif
            isLangLoaded = true;
        }
    }
    else if (!isLangLoaded) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadInSystemLanguage";
#endif
        trLoadSuccess = loadSystemTranslation_p(inLangPath, &inAppTranslator);
        if (trLoadSuccess) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "installInternalTranslation";
#endif
            app->installTranslator(&inAppTranslator);
            if (loadQtTranslation_p(exLangPath, &exQtTranslator)) {
                app->installTranslator(&exQtTranslator);
            }
            else if (loadQtTranslation_p(inLangPath, &inQtTranslator)) {
                app->installTranslator(&inQtTranslator);
            }
#if QT_VERSION >= 0x060000
            QLocale::setDefault(QLocale(currentLanguage));
#else
            QLocale::setDefault(currentLanguage);
#endif
            isLangLoaded = true;
        }
        else if (!trLoadSuccess) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "fallbackToDefaultApplicationLanguage";
#endif
            currentLanguage = "en_GB";
            if (loadQtTranslation_p(exLangPath, &exQtTranslator)) {
                app->installTranslator(&exQtTranslator);
            }
            else if (loadQtTranslation_p(inLangPath, &inQtTranslator)) {
                app->installTranslator(&inQtTranslator);
            }
#if QT_VERSION >= 0x060000
            QLocale::setDefault(QLocale(currentLanguage));
#else
            QLocale::setDefault(currentLanguage);
#endif
            isLangLoaded = true;
        }
    }
#else
    bool trLoadSuccess;
    if (isUserLanguageSystem_p()) {
        trLoadSuccess = loadSystemTranslation_p(inLangPath, &inAppTranslator);
    }
    else {
        trLoadSuccess = loadUserTranslation_p(inLangPath, &inAppTranslator);
    }
    if (!trLoadSuccess && !isUserLanguageSystem_p()) {
        trLoadSuccess = loadSystemTranslation_p(inLangPath, &inAppTranslator);
    }
    if (trLoadSuccess) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "installTranslation" << currentLanguage;
#endif
        app->installTranslator(&inAppTranslator);
        if (loadQtTranslation_p(exLangPath, &exQtTranslator)) {
            app->installTranslator(&exQtTranslator);
        }
        else if (loadQtTranslation_p(inLangPath, &inQtTranslator)) {
            app->installTranslator(&inQtTranslator);
        }
#if QT_VERSION >= 0x060000
        QLocale::setDefault(QLocale(currentLanguage));
#else
        QLocale::setDefault(currentLanguage);
#endif
        isLangLoaded = true;
    }
#endif
}
QStringList TranslationClass::listTranslations(const QString &langPath)
{
    QDir langDir;
    langDir.setNameFilters(QStringList("gta5sync_*.qm"));
    langDir.setPath(langPath);
    QStringList availableLanguages;
    for (const QString &lang : langDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort)) {
        availableLanguages << QString(lang).remove("gta5sync_").remove(".qm");
    }
    return availableLanguages;
}
QStringList TranslationClass::listAreaTranslations()
{
    QDir langDir;
    langDir.setNameFilters(QStringList("глобальный.*.это"));
    langDir.setPath(":/global");
    QStringList availableLanguages;
    for (const QString &lang : langDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort)) {
        availableLanguages << QString(lang).remove("global.").remove(".ini");
    }
    return availableLanguages;
}
bool TranslationClass::loadSystemTranslation_p(const QString &langPath, QTranslator *appTranslator)
{
#ifdef GTA5SYNC_DEBUG
    qDebug() << "loadSystemTranslation_p";
#endif
    int currentLangCounter = 0;
    for (const QString &languageName : QLocale::system().uiLanguages()) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadLanguage" << languageName;
#endif
        const QStringList langList = QString(languageName).replace("-","_").split("_");
        if (langList.length() == 2) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "loadLanguageFile" << QString(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm");
#endif
            if (QFile::exists(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm")) {
                if (appTranslator->load(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                    qDebug() << "loadLanguageFileSuccess" << QString(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm");
#endif
                    isEnglishMode = false;
                    currentLanguage = languageName;
                    currentLangIndex = currentLangCounter;
                    return true;
                }
            }
#ifdef GTA5SYNC_DEBUG
            qDebug() << "loadLanguageFile" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
            if (QFile::exists(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
                if (appTranslator->load(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                    qDebug() << "loadLanguageFileSuccess" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
                    isEnglishMode = false;
                    currentLanguage = languageName;
                    currentLangIndex = currentLangCounter;
                    return true;
                }
                else if (langList.at(0) == "en") {
#ifdef GTA5SYNC_DEBUG
                    qDebug() << "ЯзыкEnglishMode index" << currentLangCounter;
#endif
                    isEnglishMode = true;
                    currentLanguage = languageName;
                    currentLangIndex = currentLangCounter;
                    return true;
                }
            }
            else if (langList.at(0) == "en") {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "ЯзыкEnglishMode index" << currentLangCounter;
#endif
                isEnglishMode = true;
                currentLanguage = languageName;
                currentLangIndex = currentLangCounter;
                return true;
            }
        }
        else if (langList.length() == 1) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "loadLanguageFile" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
            if (QFile::exists(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
                if (appTranslator->load(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                    qDebug() << "loadLanguageFileSuccess" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
                    isEnglishMode = false;
                    currentLanguage = languageName;
                    currentLangIndex = currentLangCounter;
                    return true;
                }
            }
        }
#ifdef GTA5SYNC_DEBUG
        qDebug() << "currentLangCounter удар";
#endif
        currentLangCounter++;
    }
    return false;
}
bool TranslationClass::loadUserTranslation_p(const QString &langPath, QTranslator *appTranslator)
{
#ifdef GTA5SYNC_DEBUG
    qDebug() << "loadUserTranslation_p";
#endif
    const QString languageName = userLanguage;
    const QStringList langList = QString(languageName).replace("-","_").split("_");
    if (langList.length() == 2) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadLanguageFile" << QString(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm");
#endif
        if (QFile::exists(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm")) {
            if (appTranslator->load(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadLanguageFileSuccess" << QString(langPath % "/gta5sync_" % langList.at(0) % "_" % langList.at(1) % ".qm");
#endif
                currentLanguage = languageName;
                return true;
            }
        }
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadLanguageFile" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
        if (QFile::exists(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
            if (appTranslator->load(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadLanguageFileSuccess" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
                currentLanguage = languageName;
                return true;
            }
        }
    }
    else if (langList.length() == 1) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadLanguageFile" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
        if (QFile::exists(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
            if (appTranslator->load(langPath % "/gta5sync_" % langList.at(0) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadLanguageFileSuccess" << QString(langPath % "/gta5sync_" % langList.at(0) % ".qm");
#endif
                currentLanguage = languageName;
                return true;
            }
        }
    }
    return false;
}
bool TranslationClass::loadQtTranslation_p(const QString &langPath, QTranslator *qtTranslator)
{
#ifdef GTA5SYNC_DEBUG
    qDebug() << "loadQtTranslation_p" << currentLanguage;
#endif
    const QString languageName = currentLanguage;
    const QStringList langList = QString(languageName).replace("-","_").split("_");
    if (langList.length() == 2) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadLanguageFile" << QString(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % "_" % langList.at(1) % ".qm");
#endif
        if (QFile::exists(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % "_" % langList.at(1) % ".qm")) {
            if (qtTranslator->load(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % "_" % langList.at(1) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadLanguageFileSuccess" << QString(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % "_" % langList.at(1) % ".qm");
#endif
                return true;
            }
        }
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadLanguageFile" << QString(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm");
#endif
        if (QFile::exists(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm")) {
            if (qtTranslator->load(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadLanguageFileSuccess" << QString(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm");
#endif
                return true;
            }
        }
    }
    else if (langList.length() == 1) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "loadLanguageFile" << QString(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm");
#endif
        if (QFile::exists(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm")) {
            if (qtTranslator->load(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm")) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "loadLanguageFileSuccess" << QString(langPath % QDir::separator() % QtBaseTranslationFormat % langList.at(0) % ".qm");
#endif
                return true;
            }
        }
    }
    return false;
}
bool TranslationClass::isUserLanguageSystem_p()
{
    return (userLanguage == "System" || userLanguage.trimmed().isEmpty());
}
QString TranslationClass::getCurrentAreaLanguage()
{
    const QStringList areaTranslations = listAreaTranslations();
    if (userAreaLanguage == "Auto" || userAreaLanguage.trimmed().isEmpty()) {
        const GameLanguage gameLanguage = AppEnv::getGameLanguage(AppEnv::getGameVersion());
        if (gameLanguage == GameLanguage::Undefined) {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "autoAreaLanguageModeInterface";
#endif
            QString langCode = QString(currentLanguage).replace("-", "_");
            if (areaTranslations.contains(langCode)) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "autoAreaLanguageSelected" << langCode;
#endif
                return langCode;
            }
            else if (langCode.contains("_")) {
                langCode = langCode.split("_").at(0);
                if (!areaTranslations.contains(langCode))
                    goto outputDefaultLanguage;
#ifdef GTA5SYNC_DEBUG
                qDebug() << "autoAreaLanguageSelected" << langCode;
#endif
                return langCode;
            }
        }
        else {
#ifdef GTA5SYNC_DEBUG
            qDebug() << "autoAreaLanguageModeGame";
#endif
            QString langCode = AppEnv::gameLanguageToString(gameLanguage).replace("-", "_");
            if (areaTranslations.contains(langCode)) {
#ifdef GTA5SYNC_DEBUG
                qDebug() << "autoAreaLanguageSelected" << langCode;
#endif
                return langCode;
            }
            else if (langCode.contains("_")) {
                langCode = langCode.split("_").at(0);
                if (!areaTranslations.contains(langCode))
                    goto outputDefaultLanguage;
#ifdef GTA5SYNC_DEBUG
                qDebug() << "autoAreaLanguageSelected" << langCode;
#endif
                return langCode;
            }
        }
    }
    else if (areaTranslations.contains(userAreaLanguage)) {
#ifdef GTA5SYNC_DEBUG
        qDebug() << "userAreaLanguageSelected" << userAreaLanguage;
#endif
        return userAreaLanguage;
    }
    else if (userAreaLanguage.contains("_")) {
        const QString langCode = QString(userAreaLanguage).replace("-", "_").split("_").at(0);
        if (!areaTranslations.contains(langCode))
            goto outputDefaultLanguage;
#ifdef GTA5SYNC_DEBUG
        qDebug() << "userAreaLanguageSelected" << langCode;
#endif
        return langCode;
    }
outputDefaultLanguage:
#ifdef GTA5SYNC_DEBUG
    qDebug() << "defaultAreaLanguageSelected";
#endif
    return "en";
}
QString TranslationClass::getCurrentLanguage()
{
    return currentLanguage;
}
bool TranslationClass::isLanguageLoaded()
{
    return isLangLoaded;
}
void TranslationClass::unloadTranslation(QApplication *app)
{
    if (isLangLoaded) {
#ifndef GTA5SYNC_QCONF
        app->removeTranslator(&exAppTranslator);
        app->removeTranslator(&exQtTranslator);
        app->removeTranslator(&inAppTranslator);
        app->removeTranslator(&inQtTranslator);
#else
        app->removeTranslator(&inAppTranslator);
        app->removeTranslator(&exQtTranslator);
#endif
        currentLangIndex = 0;
        currentLanguage = QString();
        QLocale::setDefault(QLocale::c());
        isEnglishMode = false;
        isLangLoaded = false;
    }
#ifdef _MSC_VER
    Q_UNUSED(app)
#endif
}
QString TranslationClass::getCountryCode(QLocale::Country country)
{
    const QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, country);
    if (!locales.isEmpty()) {
        const QStringList localeStrList = locales.at(0).name().split("_");
        if (localeStrList.length() >= 2) {
            return localeStrList.at(1).toLower();
        }
    }
    return QString();
}
QString TranslationClass::getCountryCode(QLocale locale)
{
    QStringList localeStrList = locale.name().split("_");
    if (localeStrList.length() >= 2) {
        return localeStrList.at(1).toLower();
    }
    return QString();
}