#ifndef STRINGPARSER_H
#define STRINGPARSER_H
#include <QByteArray>
#include <QString>
class StringParser
{
public:
    static QString escapeString(const QString &toEscape);
    static QString convertBuildedString(const QString &buildedStr);
};
#endif