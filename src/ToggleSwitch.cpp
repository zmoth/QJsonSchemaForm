#include "ToggleSwitch.h"

#include <qtconfigmacros.h>

#include <QPainterPath>
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>


QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

/// <summary>
/// Toggle button has 2 different design. In the first design, if the ball
/// (thumb) radius is larger than the slide (track) radius, a flat ball slides
/// and colors from the slide according to the on / off situation. In the second
/// design, if the ball radius is smaller than the slide radius, the ball moves
/// according to the on / off status inside the slide and includes the check and
/// uncheck marks.
/// </summary>
ToggleSwitch::ToggleSwitch(int trackRadius, int thumbRadius, QWidget *parent) : QAbstractButton(parent)
{
    setCheckable(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed));
    _trackRadius = trackRadius;
    _thumbRadius = thumbRadius;
    _animation = new QPropertyAnimation(this);
    _animation->setTargetObject(this);

    _margin = 0 > (_thumbRadius - _trackRadius) ? 0 : (_thumbRadius - _trackRadius);
    _baseOffset = _thumbRadius > _trackRadius ? _thumbRadius : _trackRadius;
    _endOffset.insert(true, 4 * _trackRadius + 2 * _margin - _baseOffset);  // width - offset
    _endOffset.insert(false, _baseOffset);
    _offset = _baseOffset;
    const QPalette palette = this->palette();

    if (_thumbRadius > _trackRadius) {
        _trackColor.insert(true, palette.highlight());
        _trackColor.insert(false, palette.dark());
        _thumbColor.insert(true, palette.highlight());
        _thumbColor.insert(false, palette.light());
        _textColor.insert(true, palette.highlightedText().color());
        _textColor.insert(false, palette.dark().color());
        _thumbText.insert(true, "");
        _thumbText.insert(false, "");
        _opacity = 0.5;
    } else {
        _trackColor.insert(true, palette.highlight());
        _trackColor.insert(false, palette.dark());
        _thumbColor.insert(true, palette.highlightedText());
        _thumbColor.insert(false, palette.light());
        _textColor.insert(true, palette.highlight().color());
        _textColor.insert(false, palette.dark().color());
        _thumbText.insert(true, QChar(0x2714));   // check character
        _thumbText.insert(false, QChar(0x2715));  // uncheck character
        _opacity = 1.0;
    }
}

ToggleSwitch::~ToggleSwitch()
{
    delete _animation;
}

QSize ToggleSwitch::sizeHint() const
{
    const int w = _trackRadius * 4 + 2 * _margin;
    const int h = _trackRadius * 2 + 2 * _margin;

    return {w, h};
}

void ToggleSwitch::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    auto paintFlags = QPainter::RenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    p.setRenderHints(paintFlags, true);
    p.setPen(Qt::NoPen);
    const bool check = isChecked();
    qreal trackOpacity = _opacity;
    const qreal textOpacity = 1.0;
    const qreal thumbOpacity = 1.0;
    QBrush trackBrush;
    QBrush thumbBrush;
    QColor textColor;

    if (this->isEnabled()) {
        trackBrush = _trackColor[check];
        thumbBrush = _thumbColor[check];
        textColor = _textColor[check];
    } else {
        trackOpacity *= 0.8;
        trackBrush = this->palette().shadow();
        thumbBrush = this->palette().mid();
        textColor = this->palette().shadow().color();
    }

    p.setBrush(trackBrush);
    p.setOpacity(trackOpacity);
    p.drawRoundedRect(_margin, _margin, width() - 2 * _margin, height() - 2 * _margin, _trackRadius, _trackRadius);

    p.setBrush(thumbBrush);
    p.setOpacity(thumbOpacity);
    p.drawEllipse(_offset - _thumbRadius, _baseOffset - _thumbRadius, 2 * _thumbRadius, 2 * _thumbRadius);

    p.setPen(textColor);
    p.setOpacity(textOpacity);
    QFont font = p.font();
    font.setPixelSize(1.5 * _thumbRadius);
    p.setFont(font);

    // Since the antialiasasing provided by the drawText function is incompetent,
    // DrawPath function preferred. But since the drawPath function is not capable
    // of aligment, Pixel offsets calculated to provide aligment.
    QPainterPath textPath;
    qreal pixelOffset = (qreal)_thumbRadius * (1 - 1 / 1.414);
    textPath.addText(_offset - _thumbRadius + pixelOffset, _baseOffset + _thumbRadius - pixelOffset, font,
                     _thumbText.value(check));
    p.drawPath(textPath);

    /*p.drawText(QRectF(_offset - _thumbRadius,
        _baseOffset - _thumbRadius,
        2 * _thumbRadius,
        2 * _thumbRadius),
        Qt::AlignCenter,
        _thumbText.value(check));*/
}

void ToggleSwitch::resizeEvent(QResizeEvent *e)
{
    QAbstractButton::resizeEvent(e);
    _offset = _endOffset.value(isChecked());
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent *e)
{
    QAbstractButton::mouseReleaseEvent(e);
    if (e->button() == Qt::LeftButton) {
        _animation->setDuration(120);
        _animation->setPropertyName("_offset");
        _animation->setStartValue(_offset);
        _animation->setEndValue(_endOffset[isChecked()]);
        _animation->start();
    }
}

// void ToggleSwitch::enterEvent(QEvent * event)
// {
//     setCursor(Qt::PointingHandCursor);
//     QAbstractButton::enterEvent(event);
// }

void ToggleSwitch::setChecked(bool checked)
{
    QAbstractButton::setChecked(checked);
    _offset = _endOffset.value(checked);
}

int ToggleSwitch::offset() const
{
    return _offset;
}

void ToggleSwitch::setOffset(int value)
{
    _offset = value;
    update();
}

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE
