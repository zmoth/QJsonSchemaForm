#include "QJsonSchemaWidgets.h"

#include <qcheckbox.h>
#include <qglobal.h>
#include <qwidget.h>

#include <QJsonObject>

QT_BEGIN_NAMESPACE

#pragma region /*父类*/

QJsonSchemaWidget::QJsonSchemaWidget(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f) {}

QJsonSchemaWidget::QJsonSchemaWidget(const QJsonObject &schema, QWidget *parent, Qt::WindowFlags f)
    : QJsonSchemaWidget(parent, f)
{
    setJsonSchema(schema);
}

void QJsonSchemaWidget::onChanged()
{
    qInfo() << "a";
}

// QSchemaLineEdit::QSchemaLineEdit(QWidget *parent) : QLineEdit(parent)
// {
//     connect(this, &QSchemaLineEdit::editingFinished, this, &QSchemaLineEdit::onChanged);
// }

void data(const QJsonObject &jsonData) {}

#pragma endregion /*父类*/

#pragma region /*boolean*/

QJsonSchemaBoolean::QJsonSchemaBoolean(const QJsonObject &schema, QWidget *parent, Qt::WindowFlags f)
    : QJsonSchemaWidget(schema, parent, f)
{
}

void QJsonSchemaBoolean::setJsonSchema(const QJsonObject &s)
{
    // TODO(moth): 判断格式
    schema = s;

    // {
    //     auto mI = schema.find("default");
    //     if (mI != schema.end()) {
    //         auto def = mI->toBool(false);
    //         m_widget->setChecked(def);
    //         m_switch->setChecked(def);
    //     }
    // }
    // QString wid = "checkbox";

    // {
    //     auto mI = schema.find("ui:widget");
    //     if (mI != J.end()) {
    //         wid = mI->toString("checkbox");
    //     }
    // }

    // if (wid == "switch") {
    //     m_switch->setVisible(true);
    //     m_widget->setVisible(false);
    // } else {
    //     m_switch->setVisible(false);
    //     m_widget->setVisible(true);
    // }
}

#pragma endregion /*boolean*/

QT_END_NAMESPACE
