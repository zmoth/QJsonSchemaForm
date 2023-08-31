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
  "type": "object",
  "title": "Arrays",
  "ui:width": 500,
  "ui:height": 800,
  "properties": {
    "fixed_order": {
      "type": "array",
      "description": "Fixed Size array",
      "title": "Fixed Size",
      "ui:spacing": 0,
      "ui:fixedOrder": true,
      "items": [
        { "type": "number", "default": 33 },
        { "type": "number", "default": 44 },
        { "type": "number", "default": 55 }
      ]
    },

    "fixed_size": {
      "type": "array",
      "description": "Fixed Size array",
      "title": "Fixed Size",
      "ui:spacing": 0,
      "items": [
        { "type": "number", "default": 33 },
        { "type": "number", "default": 44 },
        { "type": "number", "default": 55 }
      ]
    },

    "growable_array": {
      "type": "array",
      "description": "this is a test",
      "title": "Growable",
      "ui:spacing": 0,
      "additionalItems": [
        {
          "type": "number",
          "title": "Number"
        }
      ]
    },

    "multiple_types": {
      "type": "array",
      "description": "Choose what type of",
      "title": "Multi Growable",
      "additionalItems": [
        {
          "type": "number",
          "title": "Number"
        },
        {
          "type": "integer",
          "title": "Integer"
        },
        {
          "type": "integer",
          "title": "Integer Slider",
          "ui:widget": "range"
        },
        {
          "type": "string",
          "title": "String"
        },
        {
          "type": "string",
          "title": "Directory",
          "ui:widget": "dir"
        },
        {
          "type": "string",
          "title": "File",
          "ui:widget": "file"
        },
        {
          "type": "string",
          "title": "Combo Box",
          "enum": ["Choice 1", "Choice 2"]
        },
        {
          "type": "boolean",
          "title": "Boolean"
        },
        {
          "type": "string",
          "ui:widget": "color",
          "title": "Color"
        },
        {
          "type": "string",
          "ui:widget": "date",
          "title": "Date"
        },
        {
          "title": "Object",
          "type": "object",
          "properties": {
            "first_name": { "type": "string" },
            "last_name": { "type": "string" }
          },
          "ui:order": ["first_name", "last_name"]
        }
      ]
    }
  }
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
        connect(body, &QJsonSchemaForm::QJsonSchemaForm::changed, [this, body, jsonDataEditor]() {
            // 将表单上的数值转换为json传入插件
            const auto &json = body->getValue();
            jsonDataEditor->setText(QJsonDocument(json.toObject()).toJson());
            qInfo() << json;
        });
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
