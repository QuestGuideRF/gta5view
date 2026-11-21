#ifndef GLOBALSTRING_H
#define GLOBALSTRING_H
#include <QString>
#include <QMap>
class GlobalString
{
public:
    GlobalString();
    static QString getString(QString valueStr, bool *ok = nullptr);
    static QString getLanguageFile();
    static QString getLanguage();
    static QMap<QString, QString> getGlobalMap();
};
#endif