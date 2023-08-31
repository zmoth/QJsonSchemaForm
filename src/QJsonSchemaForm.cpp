#include "QJsonSchemaForm.h"

#include <iostream>
#include <QBoxLayout>

#include "QJsonSchemaWidgetsFactory.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

static QJsonObject getRef(const QJsonObject &json, std::string ref)
{
    {
        auto i = ref.begin();
        auto j = std::find(i, ref.end(), '/');

        const QString key = std::string(i, j).c_str();

        if (j == ref.end()) {
            if (json.contains(key)) {
                return json.find(key)->toObject();
            }
            return {};
        }

        auto it = json.find(key);
        if (it != json.end()) {
            return getRef(it->toObject(), std::string(j + 1, ref.end()));
        }
    }
    return {};
}

QJsonSchemaForm::QJsonSchemaForm(QWidget *parent) : QJsonSchemaWidget(parent)
{
    setObjectName("form");

    auto *layout = new QVBoxLayout(this);
}

QJsonSchemaForm::QJsonSchemaForm(const QJsonObject &schema, QWidget *parent) : QJsonSchemaForm(parent)
{
    setSchema(schema);
}

void QJsonSchemaForm::setSchema(const QJsonObject &s)
{
    _schema = s;

    QJsonSchemaWidgetsFactory f;
    _widget = f.createWidget(s, this);

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

QJsonObject QJsonSchemaForm::getDef(const QString &ref) const
{
    std::string r = ref.toStdString();
    if (r.size() > 2) {
        if (r.front() == '#' && r[1] == '/') {
            return getRef(_schema, std::string(r.begin() + 2, r.end()));
        }
    }
    return {};
}

QJsonObject QJsonSchemaForm::dereference(QJsonObject jj) const
{
    QJsonObject j;
    if (jj.contains("$ref")) {
        j = getDef(jj.find("$ref")->toString());

        for (const auto &i : jj.keys()) {
            j[i] = jj[i];
        }
    } else {
        j = jj;
    }
    return j;
}

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE