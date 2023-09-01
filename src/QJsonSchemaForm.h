#pragma once

#include <qtconfigmacros.h>

#include <QJsonObject>

#include "QJsonSchemaWidgets.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

/// @brief schema 表单
class QJsonSchemaForm : public QJsonSchemaWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaForm(QWidget *parent = nullptr);
    explicit QJsonSchemaForm(const QJsonObject &schema, QWidget *parent = nullptr);
    ~QJsonSchemaForm() override = default;

    /// @brief 设置json schema
    /// @param[in] s schema
    void processSchema(const QJsonObject &s) override;

    /// @brief 获取表单内容 json格式
    /// @return QJsonValue
    [[nodiscard]] QJsonValue getValue() const override;
    /// @brief 将json值解析到表单
    /// @param[in] data
    void setValue(const QJsonObject &json) override;

  private:
    // Form表单肯定是由一个Object构成的
    QJsonSchemaWidget *_widget{nullptr};
};

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE
