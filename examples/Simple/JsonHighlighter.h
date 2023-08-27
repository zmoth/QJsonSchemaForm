#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class JsonHighlighter : public QSyntaxHighlighter
{
  public:
    explicit JsonHighlighter(QTextDocument *parent = nullptr);

  protected:
    void highlightBlock(const QString &text) override;

  private:
    QTextCharFormat _keyFormat;
    QTextCharFormat _stringFormat;
    QTextCharFormat _numberFormat;
    QTextCharFormat _booleanFormat;
    QTextCharFormat _nullFormat;
};
