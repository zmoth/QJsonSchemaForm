#ifndef QJSONSCHEMAWIDGETS_H
#define QJSONSCHEMAWIDGETS_H

#include <QJsonObject>
#include <QWidget>

QT_BEGIN_NAMESPACE

class QCheckBox;

class QJsonSchemaWidget : public QWidget
{
  public:
    explicit QJsonSchemaWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit QJsonSchemaWidget(const QJsonObject &schema,
                               QWidget *parent = nullptr,
                               Qt::WindowFlags f = Qt::WindowFlags());

    ~QJsonSchemaWidget() override = default;

    [[nodiscard]] inline QJsonObject jsonSchema() const { return schema; };
    virtual void setJsonSchema(const QJsonObject &) = 0;

    virtual void data(const QJsonObject &jsonData) = 0;

  Q_SIGNALS:
    void changed(const QJsonObject &schema);
    void update(const QJsonObject &data);

  public Q_SLOTS:
    void onChanged();

  public:
    QJsonObject schema;
};

class QJsonSchemaBoolean : public QJsonSchemaWidget
{
    Q_OBJECT

  public:
    explicit QJsonSchemaBoolean(const QJsonObject &schema,
                                QWidget *parent = nullptr,
                                Qt::WindowFlags f = Qt::WindowFlags());

    void setJsonSchema(const QJsonObject &s) override;

  private:
    QCheckBox *_checkBox{nullptr};
};

QT_END_NAMESPACE

#endif /*QJSONSCHEMAWIDGETS_H*/