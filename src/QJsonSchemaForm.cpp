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

QJsonSchemaForm::QJsonSchemaForm(QWidget *parent) : QWidget(parent)
{
    setObjectName("form");

    // 实例化时就配置QFormLayout
    auto *layout = new QFormLayout(this);
    layout->setRowWrapPolicy(QFormLayout::WrapAllRows);  // 设置表单中的标签都位于控件的上方
}

QJsonSchemaForm::QJsonSchemaForm(const QJsonObject &schema, QWidget *parent) : QJsonSchemaForm(parent)
{
    fromJsonSchema(schema);
}

// QJsonSchemaForm::CreatorMap QJsonSchemaForm::cmap()
// {
//     static CreatorMap m{{"a", [] { return new QSchemaLineEdit(); }}};
//     return m;
// }

[[nodiscard]] QFormLayout *QJsonSchemaForm::formLayout() const
{
    return qobject_cast<QFormLayout *>(layout());
}

void QJsonSchemaForm::setFormLayout(QFormLayout *layout)
{
    setLayout(layout);
}

void QJsonSchemaForm::fromJsonSchema(const QJsonObject &schema)
{
    // title
    if (schema.contains("title")) {
        setWindowTitle(schema["title"].toString());
    }

    // 控件存在于 "properties" 中
    if (!schema.contains("properties")) {
        return;  // 错误
    }

    // 解析schema生成表单控件

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
                auto *uploadButton = new QPushButton(tr("Upload"), this);
                uploadButton->setObjectName(key);
                connect(uploadButton, &QPushButton::clicked, this, &QJsonSchemaForm::_onOpen);
            } else {
                auto *lineEdit = new QLineEdit(this);
                lineEdit->setObjectName(key);
                formLayout()->addRow(prop["title"].toString(), lineEdit);

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
                auto *comboBox = new QComboBox(this);
                comboBox->setObjectName(key);
                formLayout()->addRow(prop["title"].toString(), comboBox);
            } else {
                auto *spinBox = new QSpinBox(this);
                spinBox->setObjectName(key);
                formLayout()->addRow(prop["title"].toString(), spinBox);
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