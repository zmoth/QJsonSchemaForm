// https://stackoverflow.com/a/62247360

#pragma once

#include <QtWidgets/QAbstractButton>

class QPropertyAnimation;

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

class ToggleSwitch : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(int _offset READ offset WRITE setOffset);

  public:
    explicit ToggleSwitch(int trackRadius, int thumbRadius, QWidget *parent = nullptr);
    ~ToggleSwitch() override;

    [[nodiscard]] QSize sizeHint() const override;

  public Q_SLOTS:
    void setChecked(bool checked);

  protected:
    void paintEvent(QPaintEvent * /*event*/) override;
    void resizeEvent(QResizeEvent * /*event*/) override;
    void mouseReleaseEvent(QMouseEvent * /*event*/) override;

    [[nodiscard]] int offset() const;
    void setOffset(int value);

  private:
    qreal _offset;
    qreal _baseOffset;
    qreal _margin;
    qreal _trackRadius;
    qreal _thumbRadius;
    qreal _opacity;
    QPropertyAnimation *_animation;

    QHash<bool, qreal> _endOffset;
    QHash<bool, QBrush> _trackColor;
    QHash<bool, QBrush> _thumbColor;
    QHash<bool, QColor> _textColor;
    QHash<bool, QString> _thumbText;
};

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE
