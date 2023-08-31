#include "QJsonSchemaWidgetsFactory.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

QJsonSchemaWidgetsFactory::QJsonSchemaWidgetsFactory(QObject *parent) : QObject(parent) {}

QJsonSchemaWidget *QJsonSchemaWidgetsFactory::createWidget(const QJsonObject &schema, QJsonSchemaWidget *parent)
{
    if (!schema.contains("type")) {
        qCritical() << "Formatting error! The \"type\" keyword does not exist!";
        return nullptr;
    }

    auto type = schema["type"].toString();

    if (type == "string") {
        return new QJsonSchemaString(schema, parent);
    }

    if (type == "number" || type == "integer") {
        return new QJsonSchemaNumber(schema, parent);
    }

    if (type == "object") {
        return new QJsonSchemaObject(schema, parent);
    }

    if (type == "array") {
        return new QJsonSchemaArray(schema, parent);
    }

    if (type == "boolean") {
        return new QJsonSchemaBoolean(schema, parent);
    }

    qWarning() << type << " is not exist!";
    return nullptr;
}

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE