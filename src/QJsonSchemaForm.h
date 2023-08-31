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

    /// @brief 获取json schema
    /// @return const QJsonObject&
    [[nodiscard]] inline QJsonObject getSchema() const { return _schema; }
    /// @brief 设置json schema
    /// @param[in] s schema
    void setSchema(const QJsonObject &s) override;

    /// @brief 获取表单内容 json格式
    /// @return QJsonValue
    [[nodiscard]] QJsonValue getValue() const override;
    /// @brief 将json值解析到表单
    /// @param[in] data
    void setValue(const QJsonObject &json) override;

    /// @brief 废弃
    /// @param[in] JJ
    /// @return QJsonObject
    [[nodiscard]] QJsonObject dereference(QJsonObject jj) const;

    /**
     * @brief getDef
     * @param ref
     * @return
     *
     * Gets an object using the following reference
     *
     * ref = "#/key1/key2/key3"
     */
    [[nodiscard]] QJsonObject getDef(QString ref) const;

  private:
    QJsonObject _schema;

    // Form表单肯定是由一个Object构成的
    QJsonSchemaWidget *_widget{nullptr};
};

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE
