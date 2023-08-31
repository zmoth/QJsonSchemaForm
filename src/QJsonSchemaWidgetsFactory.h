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

    static QJsonSchemaWidget *createWidget(const QJsonObject &schema, QJsonSchemaWidget *parent = nullptr);

    static void setValue(QJsonSchemaWidget *widget, const QJsonValue &value);

    [[nodiscard]] static QJsonObject getDef(const QJsonObject &schema, const QString &ref);

    [[nodiscard]] static QJsonObject dereference(const QJsonObject &schema, QJsonSchemaWidget *widget);
};

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE