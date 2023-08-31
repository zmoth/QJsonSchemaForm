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

void QJsonSchemaWidgetsFactory::setValue(QJsonSchemaWidget *widget, const QJsonValue &value)
{
    if (auto *w = dynamic_cast<QJsonSchemaNumber *>(widget)) {
        if (value.isDouble()) {
            w->setValue(value.toDouble());
        }
    } else if (auto *w = dynamic_cast<QJsonSchemaString *>(widget)) {
        if (value.isString()) {
            w->setValue(value.toString());
        }
    } else if (auto *w = dynamic_cast<QJsonSchemaObject *>(widget)) {
        if (value.isObject()) {
            w->setValue(value.toObject());
        }
    } else if (auto *w = dynamic_cast<QJsonSchemaArray *>(widget)) {
        if (value.isArray()) {
            w->setValue(value.toArray());
        }
    } else if (auto *w = dynamic_cast<QJsonSchemaBoolean *>(widget)) {
        if (value.isBool()) {
            w->setValue(value.toBool());
        }
    }
}

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE