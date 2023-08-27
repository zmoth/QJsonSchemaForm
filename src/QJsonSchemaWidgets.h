#ifndef QJSONSCHEMAWIDGETS_H
#define QJSONSCHEMAWIDGETS_H

#include <QWidget>

QT_BEGIN_NAMESPACE

class QJsonSchemaWidget
{
  public:
    explicit QJsonSchemaWidget(const QJsonObject &schema);
    virtual ~QJsonSchemaWidget() = default;

    virtual void data(const QJsonObject &jsonData) = 0;

  public Q_SLOTS:
    void onChanged();

  protected:
    QString key;
};

#include <QLineEdit>

class QSchemaLineEdit
    : public QJsonSchemaWidget
    , public QLineEdit
{
  public:
    // explicit QSchemaLineEdit(QWidget *parent = nullptr);
    ~QSchemaLineEdit() override = default;

    void data(const QJsonObject &jsonData) override;
};

#include <QTextEdit>

class QSchemaTextEdit
    : public QTextEdit
    , public QJsonSchemaWidget
{
  public:
};

#include <QCheckBox>

class QSchemaCheckBox
    : public QCheckBox
    , public QJsonSchemaWidget
{
  public:
};

#include <QSpinBox>

class QSchemaSpinBox
    : public QSpinBox
    , public QJsonSchemaWidget
{
  public:
};

class QSchemaDoubleSpinBox
    : public QDoubleSpinBox
    , public QJsonSchemaWidget
{
  public:
};

#include <QSlider>

class QSchemaSlider
    : public QSlider
    , public QJsonSchemaWidget
{
  public:
};

#include <QPushButton>

class QSchemaPushButton
    : public QPushButton
    , public QJsonSchemaWidget
{
  public:
};

QT_END_NAMESPACE

#endif /*QJSONSCHEMAWIDGETS_H*/