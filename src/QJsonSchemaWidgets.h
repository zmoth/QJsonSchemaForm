#pragma once

#include <map>
#include <QJsonArray>
#include <QJsonObject>
#include <QWidget>

QT_BEGIN_NAMESPACE

class QComboBox;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QAbstractButton;
class QDateEdit;
class QFormLayout;
class QToolButton;
class QListWidget;
class QHBoxLayout;

namespace QJsonSchemaForm {

/// @brief Schema控件基类
class QJsonSchemaWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaWidget(QWidget *parent = nullptr);
    ~QJsonSchemaWidget() override = default;

    /// @brief 获取json schema
    /// @return const QJsonObject&
    [[nodiscard]] inline QJsonObject getSchema() const { return _schema; }
    /// @brief Schema生成Form
    /// @param[in] schema
    void setSchema(const QJsonObject &schema);

    virtual void processSchema(const QJsonObject &schema) = 0;

    /// @brief 由Form中控件的数据组成满足Schema规则的Json数据
    /// @return QJsonValue
    [[nodiscard]] virtual QJsonValue getValue() const = 0;
    virtual void setValue(const QJsonObject & /*json*/){};

  Q_SIGNALS:
    void changed();

  private:
    QJsonObject _schema{};
};

/// @brief object类型
class QJsonSchemaObject : public QJsonSchemaWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaObject(QJsonSchemaWidget *parent = nullptr);
    explicit QJsonSchemaObject(const QJsonObject &schema, QJsonSchemaWidget *parent = nullptr);
    ~QJsonSchemaObject() override = default;

    void processSchema(const QJsonObject &s) override;

    [[nodiscard]] QJsonValue getValue() const override;
    void setValue(const QJsonObject &json) override;

    [[nodiscard]] QFormLayout *formLayout() const;
    void setFormLayout(QFormLayout *layout);

    // object 必定存在 properties，内容是其他控件
    std::map<QString, std::pair<QString, QJsonSchemaWidget *>> widgetsMap;
};

/// @brief array类型
class QJsonSchemaArray : public QJsonSchemaWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaArray(QJsonSchemaWidget *parent = nullptr);
    explicit QJsonSchemaArray(const QJsonObject &schema, QJsonSchemaWidget *parent = nullptr);
    ~QJsonSchemaArray() override = default;

    void processSchema(const QJsonObject &schema) override;

    [[nodiscard]] QJsonValue getValue() const override;
    void setValue(QJsonArray data);

    struct IteT
    {
        QJsonSchemaWidget *widget;
        QHBoxLayout *layout;
        QToolButton *up;
        QToolButton *down;
        QToolButton *del;
    };

    std::vector<IteT> items;

    void rebuild();

    void pushBack(const QJsonObject &o);
    QJsonArray oneOfArray;
    QComboBox *oneOf = nullptr;
    QToolButton *add = nullptr;
    QFormLayout *propertiesLayout = nullptr;
    QListWidget *listWidget = nullptr;
    bool fixedSize = false;
    bool fixedOrder = false;
};

/// @brief 字符串
class QJsonSchemaString : public QJsonSchemaWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaString(QJsonSchemaWidget *parent = nullptr);
    explicit QJsonSchemaString(const QJsonObject &schema, QJsonSchemaWidget *parent = nullptr);
    ~QJsonSchemaString() override = default;

    void processSchema(const QJsonObject &schema) override;

    [[nodiscard]] QJsonValue getValue() const override;
    void setValue(const QString &) const;

    QComboBox *combo = nullptr;
    QLineEdit *widget = nullptr;
    QPushButton *fileButton = nullptr;
    QPushButton *dirButton = nullptr;
    QPushButton *colorButton = nullptr;
    QDateEdit *dateEdit = nullptr;
};

/// @brief 布尔
class QJsonSchemaBoolean : public QJsonSchemaWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaBoolean(QJsonSchemaWidget *parent = nullptr);
    explicit QJsonSchemaBoolean(const QJsonObject &schema, QJsonSchemaWidget *parent = nullptr);
    ~QJsonSchemaBoolean() override = default;

    void processSchema(const QJsonObject &schema) override;

    [[nodiscard]] QJsonValue getValue() const override;
    void setValue(bool b);

  private:
    QAbstractButton *_switch{nullptr};
};

/// @brief 数字类型
class QJsonSchemaNumber : public QJsonSchemaWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaNumber(QJsonSchemaWidget *parent = nullptr);
    explicit QJsonSchemaNumber(const QJsonObject &schema, QJsonSchemaWidget *parent = nullptr);
    ~QJsonSchemaNumber() override = default;

    void processSchema(const QJsonObject &schema) override;

    [[nodiscard]] QJsonValue getValue() const override;
    void setValue(double);

  private:
    QWidget *_widget{nullptr};
};

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE
