#ifndef JSHIGHLIGHTER_H
#define JSHIGHLIGHTER_H
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextFormat>
#include <QStringList>
#include <QVector>
#include <QHash>
#if QT_VERSION >= 0x050000
#include <QRegularExpression>
#else
#include <QRegExp>
#endif
class QTextDocument;
class JSHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    struct HighlightingRule
    {
#if QT_VERSION >= 0x050000
        QRegularExpression pattern;
#else
        QRegExp pattern;
#endif
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
    QTextCharFormat doubleFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat objectFormat;
    JSHighlighter(QTextDocument *parent = 0);
protected:
    void highlightBlock(const QString &text) override;
};
#endif