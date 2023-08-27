#include "QJsonSchemaForm.h"

#include <functional>
#include <QBoxLayout>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QJsonObject>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include "QJsonSchemaWidgets.h"

QT_BEGIN_NAMESPACE

QJsonSchemaForm::QJsonSchemaForm(const QJsonObject &schema, QWidget *parent) : QWidget(parent)
{
    fromJsonSchema(schema, this);
}

// QJsonSchemaForm::CreatorMap QJsonSchemaForm::cmap()
// {
//     static CreatorMap m{{"a", [] { return new QSchemaLineEdit(); }}};
//     return m;
// }

void QJsonSchemaForm::fromJsonSchema(const QJsonObject &schema, QJsonSchemaForm *parent)
{
    if (!schema.contains("properties")) {
        return;  // 错误
    }

    auto *layout = new QFormLayout(parent);
    layout->setRowWrapPolicy(QFormLayout::WrapAllRows);  // 设置表单中的标签都位于控件的上方

    // 解析schema生成表单控件

    // title
    if (schema.contains("title")) {
        parent->setWindowTitle(schema["title"].toString());
    }

    auto properties = schema["properties"].toObject();
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        QString key = it.key();
        QJsonObject prop = it.value().toObject();

        // auto *wid = createWidget(prop, parent);
        // if (prop.contains("title")) {
        //     layout->addRow(prop["title"].toString(), wid);
        // } else {
        //     layout->addRow(key, wid);
        // }

        if (prop["type"] == "string") {
            if (prop.contains("ui:widget") && prop["ui:widget"] == "UploadWidget") {
                auto *uploadButton = new QPushButton(tr("Upload"), parent);
                uploadButton->setObjectName(key);
                connect(uploadButton, &QPushButton::clicked, this, &QJsonSchemaForm::_onOpen);
            } else {
                auto *lineEdit = new QLineEdit(parent);
                lineEdit->setObjectName(key);
                layout->addRow(prop["title"].toString(), lineEdit);

                if (prop.contains("minLength")) {
                    // 最小长度
                }
                if (prop.contains("maxLength")) {
                    // 最大长度
                    lineEdit->setMaxLength(prop["maxLength"].toInt());
                }
                if (prop.contains("pattern")) {
                    // 正则表达式
                }
            }
        } else if (prop["type"] == "integer") {
            if (prop.contains("enum")) {
                auto *comboBox = new QComboBox(parent);
                comboBox->setObjectName(key);
                layout->addRow(prop["title"].toString(), comboBox);
            } else {
                auto *spinBox = new QSpinBox(parent);
                spinBox->setObjectName(key);
                layout->addRow(prop["title"].toString(), spinBox);
            }
        }

        // 添加其他类型控件
    }
}

QJsonObject QJsonSchemaForm::toJsonSchema([[maybe_unused]] QJsonSchemaForm *parent)
{
    return {};
}

// QWidget *QJsonSchemaForm::createWidget(const QJsonObject &json, QWidget *parent)
// {
//     auto *w = cmap()[json["type"].toString()]();

//     return w;
// }

QJsonObject QJsonSchemaForm::toJson() const
{
    QJsonObject json;

    for (auto *obj : children()) {
        if (obj->objectName().isEmpty()) {
            continue;
        }

        // qInfo() << QObject::metatype()->typeName() << " " << obj->objectName();
        const QString &typeName = QString::fromUtf8(obj->metaObject()->metaType().name());
        if (typeName == "QLineEdit") {
            auto *lineEdit = qobject_cast<QLineEdit *>(obj);
            json[obj->objectName()] = lineEdit->text();
        }
    }
    return json;
}

void QJsonSchemaForm::fromJson(const QJsonObject &json)
{
    for (auto *obj : children()) {
        if (obj->objectName().isEmpty()) {
            continue;
        }

        // qInfo() << QObject::metatype()->typeName() << " " << obj->objectName();
        const QString &typeName = QString::fromUtf8(obj->metaObject()->metaType().name());
        if (typeName == "QLineEdit") {
            auto *lineEdit = qobject_cast<QLineEdit *>(obj);

            lineEdit->setText(json[obj->objectName()].toString());
        }
    }
}

void QJsonSchemaForm::_onOpen()
{
    const QString fileName =
        QFileDialog::getOpenFileName(this, tr("Open Flow"), QDir::currentPath(), tr("Flow Files (*.flow)"));

    if (!QFileInfo::exists(fileName)) {
        qWarning() << "\"" << fileName << "\"不存在";
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "\"" << fileName << "\"权限不够";
        return;
    }
}

QT_END_NAMESPACE