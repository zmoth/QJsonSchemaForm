#include "QJsonSchemaForm.h"

#include <iostream>
#include <QBoxLayout>

#include "QJsonSchemaWidgetsFactory.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

QJsonSchemaForm::QJsonSchemaForm(QWidget *parent) : QJsonSchemaWidget(parent)
{
    setObjectName("form");

    auto *layout = new QVBoxLayout(this);
}

QJsonSchemaForm::QJsonSchemaForm(const QJsonObject &schema, QWidget *parent) : QJsonSchemaForm(parent)
{
    setSchema(schema);
}

void QJsonSchemaForm::processSchema(const QJsonObject &s)
{
    _widget = QJsonSchemaWidgetsFactory::createWidget(s, this);
    layout()->addWidget(_widget);
}

QJsonValue QJsonSchemaForm::getValue() const
{
    return _widget->getValue();
}

void QJsonSchemaForm::setValue(const QJsonObject &json)
{
    return _widget->setValue(json);
}

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE