#include "JsonHighlighter.h"

JsonHighlighter::JsonHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    // 自定义json各键值显示高亮颜色
    _keyFormat.setForeground(Qt::darkBlue);
    QFont f;
    f.setBold(true);
    _keyFormat.setFont(f);
    _stringFormat.setForeground(Qt::darkGreen);
    _numberFormat.setForeground(Qt::darkMagenta);
    _booleanFormat.setForeground(Qt::darkYellow);
    _nullFormat.setForeground(Qt::gray);
}

void JsonHighlighter::highlightBlock(const QString &text)
{
    // 匹配值
    QRegularExpression regex1(R"((".*?")|(-?\d+\.?\d*)|(true|false)|(null))");
    int index1 = 0;
    while (index1 >= 0) {
        QRegularExpressionMatch match = regex1.match(text, index1);
        if (!match.hasMatch()) {
            break;
        }
        int start = match.capturedStart(1);
        int length = match.capturedLength(1);

        if (match.capturedStart(1) != -1) {  // String
            setFormat(match.capturedStart(1), match.capturedLength(1), _stringFormat);
        } else if (match.capturedStart(2) != -1) {  // Number
            setFormat(match.capturedStart(2), match.capturedLength(2), _numberFormat);
        } else if (match.capturedStart(3) != -1) {  // Boolean
            setFormat(match.capturedStart(3), match.capturedLength(3), _booleanFormat);
        } else if (match.capturedStart(4) != -1) {  // Null
            setFormat(match.capturedStart(4), match.capturedLength(4), _nullFormat);
        }
        index1 = match.capturedEnd();
    }
    // 匹配键
    QRegularExpression regex(R"(("\w+"):\s*)");
    int index = 0;
    while (index >= 0) {
        QRegularExpressionMatch match = regex.match(text, index);
        if (!match.hasMatch()) {
            break;
        }
        int start = match.capturedStart(1);
        int length = match.capturedLength(1);
        setFormat(start, length, _keyFormat);
        index = match.capturedEnd();
    }
}
