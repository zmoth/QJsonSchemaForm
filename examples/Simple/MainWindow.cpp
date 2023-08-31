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
  "ui:width": 500,
  "ui:height": 800,
  "title": "Object types",

  "$defs": {
    "numbers": {
      "type": "object",
      "properties": {
        "standard_float": {
          "type": "number",
          "description": "Standard floating point input"
        },
        "standard_integer": {
          "type": "integer",
          "description": "Standard integer input"
        },
        "integer_range": {
          "type": "integer",
          "ui:widget": "range",
          "minimum": 10,
          "maximum": 100,
          "description": "Integer values clamped between 10 and 100"
        },
        "float_range": {
          "type": "number",
          "ui:widget": "range",
          "minimum": 0,
          "maximum": 1,
          "description": "Float values clamped between 0 and 1"
        }
      }
    },
    "strings": {
      "type": "object",
      "properties": {
        "standard_string": {
          "type": "string",
          "default": "default value",
          "description": "Standard string input"
        },
        "string_choice": {
          "type": "string",
          "enum": [
            "human",
            "elf",
            "dwarf",
            "halfling",
            "gnome",
            "tiefling",
            "dragonborn",
            "tortle"
          ],
          "default": "dragonborn",
          "description": "String choice with default value"
        },
        "string_color": {
          "type": "string",
          "ui:widget": "colour",
          "default": "#FFAA00AA",
          "description": "String color value"
        },
        "file_path": {
          "type": "string",
          "ui:widget": "file",
          "description": "Choose a file"
        },
        "dir_path": {
          "type": "string",
          "ui:widget": "dir",
          "description": "Choose a directory"
        }
      }
    }
  },

  "properties": {
    "invisible": {
      "type": "string",
      "default": "This field is not visible in the UI",
      "ui:visible": false
    },
    "numbers": {
      "$ref": "#/$defs/numbers",
      "title": "Numbers"
    },
    "strings": {
      "$ref": "#/$defs/strings",
      "title": "Strings"
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
