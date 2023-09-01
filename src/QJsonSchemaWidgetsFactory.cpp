#include "QJsonSchemaWidgetsFactory.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

static QJsonObject getRef(const QJsonObject &json, const QString &ref)
{
    QString key{ref};

    auto index = ref.indexOf('/');
    if (index != -1) {
        key = ref.left(index);
        qDebug() << key;

        auto it = json.find(key);
        if (it != json.end()) {
            return getRef(it->toObject(), ref.mid(index + 1));
        }
    } else {
        // 最后一个key
        qDebug() << key;
        if (json.contains(key)) {
            return json.find(key)->toObject();
        }
    }

    return {};
}

static QJsonObject getParentSchema(QWidget *widget)
{
    auto *w = dynamic_cast<QJsonSchemaWidget *>(widget->parentWidget());
    if (!w) {
        return {};
    }

    if (w->objectName() != "form") {
        return getParentSchema(w);
    }

    return w->getSchema();
}

QJsonSchemaWidgetsFactory::QJsonSchemaWidgetsFactory(QObject *parent) : QObject(parent) {}

QJsonSchemaWidget *QJsonSchemaWidgetsFactory::createWidget(const QJsonObject &schema, QWidget *parent)
{
    auto s = dereference(schema, parent);

    if (!s.contains("type")) {
        qCritical() << "Formatting error! The \"type\" keyword does not exist! " << s;
        return nullptr;
    }

    auto type = s["type"].toString();

    QJsonSchemaWidget *w{nullptr};

    if (type == "string") {
        w = new QJsonSchemaString(s, parent);
    } else if (type == "number" || type == "integer") {
        w = new QJsonSchemaNumber(s, parent);
    } else if (type == "object") {
        w = new QJsonSchemaObject(s, parent);
    } else if (type == "array") {
        w = new QJsonSchemaArray(s, parent);
    } else if (type == "boolean") {
        w = new QJsonSchemaBoolean(s, parent);
    } else {
        qWarning() << type << " is not exist!";
        return nullptr;
    }

    // 将子类的改变传递给父类
    connect(w, &QJsonSchemaWidget::changed, [parent]() {
        if (auto *p = dynamic_cast<QJsonSchemaWidget *>(parent)) {
            Q_EMIT p->changed();
        }
    });

    return w;
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

QJsonObject QJsonSchemaWidgetsFactory::getDef(const QJsonObject &schema, const QString &ref)
{
    if (ref.size() > 2) {
        auto pos = ref.indexOf("#/");
        if (pos == 0) {
            return getRef(schema, ref.mid(2));
        }
    }
    return {};
}

QJsonObject QJsonSchemaWidgetsFactory::dereference(const QJsonObject &schema, QWidget *widget)
{
    if (schema.contains("$ref")) {
        QJsonObject j;

        j = getDef(getParentSchema(widget), schema.find("$ref")->toString());

        for (const auto &i : schema.keys()) {
            j[i] = schema[i];
        }

        return j;
    }

    return schema;
}

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE