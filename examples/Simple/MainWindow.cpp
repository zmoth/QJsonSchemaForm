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
        "properties": {
            "string_1692183345223x0": {
                "title": "str",
                "type": "string",
                "ui:options": {
                    "placeholder": "请输入"
                }
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

    auto *w = new QWidget(this);
    auto *layout = new QHBoxLayout(w);
    setCentralWidget(w);

    // 数据区域
    auto *jsonDataEditor = new QTextEdit(this);
    layout->addWidget(jsonDataEditor);

    // schema 区域
    auto *jsonSchemaEditor = new QTextEdit(this);
    jsonSchemaEditor->setText(QJsonDocument(_schema).toJson(QJsonDocument::Indented));
    auto *highlighter = new JsonHighlighter(jsonSchemaEditor->document());
    layout->addWidget(jsonSchemaEditor);

    // 表单绘制区域
    auto *form = new QJsonSchemaForm(_schema, w);
    form->setObjectName("form");
    layout->addWidget(form);

    // // 数据更新会刷新表单
    // connect(jsonDataEditor, &QTextEdit::textChanged, form, &QJsonSchemaForm::changed);
    // // schema更新会更新表单绘制
    // connect(jsonSchemaEditor, &QTextEdit::textChanged, form, &QJsonSchemaForm::changed);
    // // 表单点击保存会更新数据区域

    show();
}
