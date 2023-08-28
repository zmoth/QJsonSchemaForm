/// @file QJsonSchemaForm.h
/// @author moth (QianMoth@qq.com)
/// @brief
/// @version 0.0.1
/// @date 2023-08-16
///
/// @copyright Copyright (c) 2023
///
#ifndef QJSONSCHEMAFORM_H
#define QJSONSCHEMAFORM_H

#include <QWidget>

QT_BEGIN_NAMESPACE

/// @brief (vue-json-schema-form)[https://form.lljj.me/]
class QJsonSchemaForm : public QWidget
{
    Q_OBJECT

  public:
    using CreatorMap = QMap<QString, std::function<QWidget *()>>;

    // static QJsonSchemaForm::CreatorMap cmap();

    explicit QJsonSchemaForm(const QJsonObject &schema, QWidget *parent = nullptr);
    ~QJsonSchemaForm() override = default;

    /// @brief 根据Json生成widgets
    /// @param[in] schema
    /// @param[in,out] parent
    void fromJsonSchema(const QJsonObject &schema, QJsonSchemaForm *parent);
    /// @brief
    /// @param[in] parent
    /// @return QJsonObject
    QJsonObject toJsonSchema(QJsonSchemaForm *parent);

    // QWidget *createWidget(const QJsonObject &json, QWidget *parent = nullptr);

    /// @brief
    /// @param[in] json
    void fromJson(const QJsonObject &json);
    /// @brief
    /// @return QJsonObject
    [[nodiscard]] QJsonObject toJson() const;

  Q_SIGNALS:
    void changed();

  private Q_SLOTS:
    void _onOpen();

  private:
    Q_DISABLE_COPY(QJsonSchemaForm);
};

QT_END_NAMESPACE

#endif /*QJSONSCHEMAFORM_H*/