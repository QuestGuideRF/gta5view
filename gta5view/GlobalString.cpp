#include "TranslationClass.h"
#include "GlobalString.h"
#include "config.h"
#include <QStringBuilder>
#include <QStringList>
#include <QFileInfo>
#include <QSettings>
#include <QLocale>
#include <QDebug>
GlobalString::GlobalString()
{
}
QMap<QString, QString> GlobalString::getGlobalMap()
{
    QMap<QString, QString> globalMap;
    QSettings globalFile(getLanguageFile(), QSettings::IniFormat);
#if QT_VERSION < 0x060000
    globalFile.setIniCodec("UTF-8");
#endif
    globalFile.beginGroup("Global");
    for (const QString &globalStr : globalFile.childKeys()) {
        globalMap[globalStr] = globalFile.value(globalStr, globalStr).toString();
    }
    globalFile.endGroup();
    return globalMap;
}
QString GlobalString::getString(QString valueStr, bool *ok)
{
    QString globalString = valueStr;
    QSettings globalFile(getLanguageFile(), QSettings::IniFormat);
#if QT_VERSION < 0x060000
    globalFile.setIniCodec("UTF-8");
#endif
    globalFile.beginGroup("Global");
    QStringList globalStrList = globalFile.childKeys();
    if (globalStrList.contains(valueStr)) {
        if (ok != nullptr)
            *ok = true;
        globalString = globalFile.value(valueStr, valueStr).toString();
    }
    globalFile.endGroup();
    return globalString;
}
QString GlobalString::getLanguageFile()
{
    QString language = getLanguage();
    QString languageFile = ":/global/global." % language % ".ini";
#if QT_VERSION >= 0x050200
    if (!QFileInfo::exists(languageFile))
        languageFile = ":/global/global.en.ini";
#else
    if (!QFileInfo(languageFile).exists())
        languageFile = ":/global/global.en.ini";
#endif
    return languageFile;
}
QString GlobalString::getLanguage()
{
    return Translator->getCurrentAreaLanguage();
}