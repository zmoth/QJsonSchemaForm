#pragma once

#include <qjsonobject.h>

#include <QObject>

#include "QJsonSchemaWidgets.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

class QJsonSchemaWidgetsFactory : public QObject
{
    Q_OBJECT

  public:
    explicit QJsonSchemaWidgetsFactory(QObject *parent = nullptr);
    ~QJsonSchemaWidgetsFactory() override = default;

    QJsonSchemaWidget *createWidget(const QJsonObject &schema, QJsonSchemaWidget *parent = nullptr);
};

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE