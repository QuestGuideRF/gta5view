#include "JSHighlighter.h"
JSHighlighter::JSHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    QBrush keywordBrush(QColor::fromRgb(66, 137, 244));
    keywordFormat.setForeground(keywordBrush);
    keywordFormat.setFontItalic(true);
    QStringList keywordPatterns;
    keywordPatterns << "\\btrue\\b" << "\\bfalse\\b";
    for (QString pattern : keywordPatterns)
    {
#if QT_VERSION >= 0x050000
        rule.pattern = QRegularExpression(pattern);
#else
        rule.pattern = QRegExp(pattern);
#endif
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    QBrush doubleBrush(QColor::fromRgb(66, 137, 244));
    doubleFormat.setForeground(doubleBrush);
#if QT_VERSION >= 0x050000
    rule.pattern = QRegularExpression("[+-]?\\d*\\.?\\d+");
#else
    rule.pattern = QRegExp("[+-]?\\d*\\.?\\d+");
#endif
    rule.format = doubleFormat;
    highlightingRules.append(rule);
    QBrush quotationBrush(QColor::fromRgb(66, 244, 104));
    quotationFormat.setForeground(quotationBrush);
#if QT_VERSION >= 0x050000
    rule.pattern = QRegularExpression("\"[^\"]*\"");
#else
    rule.pattern = QRegExp("\"[^\"]*\"");
#endif
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    QBrush objectBrush(QColor::fromRgb(255, 80, 80));
    objectFormat.setForeground(objectBrush);
#if QT_VERSION >= 0x050000
    rule.pattern = QRegularExpression("\"[^\"]*\"(?=:)");
#else
    rule.pattern = QRegExp("\"[^\"]*\"(?=:)");
#endif
    rule.format = objectFormat;
    highlightingRules.append(rule);
}
void JSHighlighter::highlightBlock(const QString &text)
{
#if QT_VERSION >= 0x050000
    for (const HighlightingRule &rule : qAsConst(highlightingRules))
#else
    for (const HighlightingRule &rule : highlightingRules)
#endif
    {
#if QT_VERSION >= 0x050000
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
#else
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
#endif
    }
    setCurrentBlockState(0);
}