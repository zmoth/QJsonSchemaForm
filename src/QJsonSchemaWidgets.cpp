#include "QJsonSchemaWidgets.h"

QT_BEGIN_NAMESPACE

QJsonSchemaWidget::QJsonSchemaWidget(const QJsonObject &schema)
{
    // key = schema
}

void QJsonSchemaWidget::onChanged()
{
    qInfo() << "a";
}

// QSchemaLineEdit::QSchemaLineEdit(QWidget *parent) : QLineEdit(parent)
// {
//     connect(this, &QSchemaLineEdit::editingFinished, this, &QSchemaLineEdit::onChanged);
// }

void data(const QJsonObject &jsonData) {}

QT_END_NAMESPACE
