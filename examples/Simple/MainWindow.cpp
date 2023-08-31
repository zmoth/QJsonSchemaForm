#include "MainWindow.h"

#include <QBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QTextEdit>

#include "JsonHighlighter.h"
#include "QJsonSchemaForm.h"

const std::string jsonText = R"(
    {
        "type" : "object",
        "title" : "Boolean Widgets",
        "properties" : {
            "standard_boolean" :
            {
                "type" : "boolean",
                "description" : "check this if you want it to be true",
                "title" : "Boolean Field"
            },
            "default_value" :
            {
                "type" : "boolean",
                "description" : "check this if you want it to be true",
                "title" : "Default",
                "default" : true
            },
            "switch" :
            {
                "type" : "boolean",
                "ui:widget" : "switch",
                "description" : "check this if you want it to be true",
                "title" : "Switch"
            },
            "switch_default" :
            {
                "type" : "boolean",
                "ui:widget" : "switch",
                "description" : "check this if you want it to be true",
                "title" : "Switch",
                "default" : true
            }
        },
        "ui:order" : [ "standard_boolean", "default_value", "switch", "switch_default"]
    }

)";

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonText.data());
    if (jsonDocument.isNull()) {
        qDebug() << "===> please check the string " << jsonText.data();
    }
    _schema = jsonDocument.object();

    // 为主窗口添加一个布局，以容纳json编辑器和Form窗口
    auto *w = new QWidget(this);
    w->setMinimumHeight(500);
    auto *layout = new QHBoxLayout(w);
    setCentralWidget(w);

    // json 数据区域
    auto *jsonDataEditor = new QTextEdit(this);
    layout->addWidget(jsonDataEditor);

    // json schema 区域
    auto *jsonSchemaEditor = new QTextEdit(this);
    jsonSchemaEditor->setText(QJsonDocument(_schema).toJson(QJsonDocument::Indented));
    auto *highlighter = new JsonHighlighter(jsonSchemaEditor->document());
    layout->addWidget(jsonSchemaEditor);

    // 表单绘制区域
    auto *form = new QWidget();
    auto *formLayout = new QVBoxLayout(form);
    {
        auto *body = new QJsonSchemaForm::QJsonSchemaForm(_schema, form);
        body->setObjectName("body");
        {
            const QJsonObject &obj = QJsonDocument::fromJson(jsonDataEditor->toPlainText().toLatin1()).object();

            body->setValue(obj);
        }
        formLayout->addWidget(body);

        auto *buttonLayout = new QHBoxLayout();
        formLayout->addLayout(buttonLayout);
        {
            // 刷新按钮
            auto *reloadButton = new QPushButton(tr("reload"), form);
            connect(reloadButton, &QPushButton::clicked, [this, body, jsonDataEditor]() {
                // 获取json数据显示到表单上
                const QJsonObject &obj = QJsonDocument::fromJson(jsonDataEditor->toPlainText().toLatin1()).object();

                body->setValue(obj);
            });
            buttonLayout->addWidget(reloadButton);

            // 保存按钮
            auto *saveButton = new QPushButton(tr("save"), form);
            connect(saveButton, &QPushButton::clicked, [this, body, jsonDataEditor]() {
                // 将表单上的数值转换为json传入插件
                const auto &json = body->getValue();
                jsonDataEditor->setText(QJsonDocument(json.toObject()).toJson());
                qInfo() << json;
            });
            buttonLayout->addWidget(saveButton);
        }
    }

    layout->addWidget(form);

    // connect(form, &QJsonSchemaForm::QJsonSchemaForm::changed, [form]() { qInfo() << form->getValue(); });

    show();
}
