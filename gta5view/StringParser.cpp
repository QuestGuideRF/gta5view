#include "StringParser.h"
#include "config.h"
#include <QTextDocument>
#include <QApplication>
#include <QLibraryInfo>
#include <QByteArray>
#include <QFileInfo>
#include <QString>
#include <QList>
#include <QDir>
QString StringParser::escapeString(const QString &toEscape)
{
#if QT_VERSION >= 0x050000
    return toEscape.toHtmlEscaped();
#else
    return Qt::escape(toEscape);
#endif
}
QString StringParser::convertBuildedString(const QString &buildedStr)
{
    QString outputStr = buildedStr;
    outputStr.replace("APPNAME:", QString::fromUtf8(GTA5SYNC_APPSTR));
    outputStr.replace("SHAREDDIR:", QString::fromUtf8(GTA5SYNC_SHARE));
    outputStr.replace("RUNDIR:", QFileInfo(QApplication::applicationFilePath()).canonicalPath());
#if QT_VERSION >= 0x060000
    outputStr.replace("QCONFLANG:", QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    outputStr.replace("QCONFPLUG:", QLibraryInfo::path(QLibraryInfo::PluginsPath));
#else
    outputStr.replace("QCONFLANG:", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    outputStr.replace("QCONFPLUG:", QLibraryInfo::location(QLibraryInfo::PluginsPath));
#endif
    outputStr.replace("SEPARATOR:", QDir::separator());
    return outputStr;
}