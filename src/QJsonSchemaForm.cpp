#include "QJsonSchemaForm.h"

#include <iostream>
#include <QBoxLayout>
#include <QScrollArea>

#include "QJsonSchemaWidgetsFactory.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

QJsonSchemaForm::QJsonSchemaForm(QWidget *parent) : QJsonSchemaWidget(parent)
{
    setObjectName("form");

    _scrollArea = new QScrollArea(this);
    _scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollArea->setWidgetResizable(true);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(_scrollArea);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
}

QJsonSchemaForm::QJsonSchemaForm(const QJsonObject &schema, QWidget *parent) : QJsonSchemaForm(parent)
{
    setSchema(schema);
}

void QJsonSchemaForm::processSchema(const QJsonObject &s)
{
    _widget = QJsonSchemaWidgetsFactory::createWidget(s, this);
    if (_widget) {
        _scrollArea->setWidget(_widget);
    }
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