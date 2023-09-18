#include "MainWindow.h"

#include <QJsonSchemaForm/QJsonSchemaForm.h>

#include <QBoxLayout>
#include <QDockWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QTextEdit>

#include "JsonHighlighter.h"

const std::string jsonText = R"(
{
  "type": "object",
  "title": "Json Example",
  "ui:order": [
    "str",
    "text",
    "enum",
    "file",
    "dir",
    "num",
    "range",
    "arr",
    "obj"
  ],
  "properties": {
    "invisible": {
      "ui:visible": false,
      "type": "string",
      "default": "hello world"
    },
    "str": {
      "type": "string",
      "default": "hello world",
      "description": "This is a tooltip"
    },
    "text": {
      "type": "string",
      "ui:widget": "text",
      "default": "hello world",
      "description": "This is a tooltip"
    },
    "enum": {
      "type": "string",
      "default": "hello world",
      "enum": ["first", "second", "third"]
    },
    "file": { "type": "string", "default": "/", "ui:widget": "file" },
    "dir": { "type": "string", "default": "/", "ui:widget": "dir" },
    "num": { "type": "number", "minimum": 0, "maximum": 50, "default": 10 },
    "range": {
      "type": "number",
      "minimum": 0,
      "maximum": 50,
      "default": 10,
      "ui:widget": "range"
    },
    "obj": {
      "type": "object",
      "oneOf": [
        {
          "type": "object",
          "title": "First object",
          "properties": {
            "str": { "type": "string", "title": "String" },
            "num": { "type": "number" }
          }
        },
        {
          "type": "object",
          "title": "Second Object",
          "properties": {
            "str": { "type": "string", "default": "hello world" },
            "enum": {
              "type": "string",
              "default": "hello world",
              "enum": ["first", "second", "third"]
            },
            "file": { "type": "string", "default": "/", "ui:widget": "file" },
            "dir": { "type": "string", "default": "/", "ui:widget": "dir" },
            "num": {
              "type": "number",
              "minimum": 0,
              "maximum": 50,
              "default": 10
            },
            "range": {
              "type": "number",
              "minimum": 0,
              "maximum": 50,
              "default": 10,
              "ui:widget": "range"
            }
          }
        }
      ],
      "properties": {
        "str": { "type": "string", "title": "String" },
        "num": { "type": "number" }
      }
    },
    "arr": {
      "type": "array",
      "items": [
        { "type": "string", "title": "String" },
        { "type": "number" },
        { "type": "number", "minimum": 0, "maximum": 100 },
        { "type": "number", "minimum": 0, "maximum": 100 },
        { "type": "number", "minimum": 0, "maximum": 100, "ui:widget": "range" }
      ],
      "additionalItems": [
        { "type": "string", "title": "String Object" },
        { "type": "number", "title": "Number Object" }
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

    // json schema 区域
    auto *jsonSchemaEditorDock = new QDockWidget("JsonSchema", this);
    addDockWidget(Qt::RightDockWidgetArea, jsonSchemaEditorDock);
    auto *jsonSchemaEditor = new QTextEdit(jsonSchemaEditorDock);
    jsonSchemaEditorDock->setWidget(jsonSchemaEditor);
    jsonSchemaEditor->setText(QJsonDocument(_schema).toJson(QJsonDocument::Indented));
    auto *highlighter = new JsonHighlighter(jsonSchemaEditor->document());

    // json 数据区域
    auto *jsonDataEditorDock = new QDockWidget("JsonData", this);
    addDockWidget(Qt::LeftDockWidgetArea, jsonDataEditorDock);
    auto *jsonDataEditor = new QTextEdit(jsonDataEditorDock);
    jsonDataEditorDock->setWidget(jsonDataEditor);

    // 表单绘制区域
    auto *form = new QWidget(this);
    setCentralWidget(form);
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
        }
    }

    show();
}
