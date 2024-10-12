#include <QStyle>
#include <QStyleOptionSlider>
#include <QMouseEvent>

#include "custom_widgets/direct_jump_slider.hpp"


DirectJumpSlider::DirectJumpSlider () : QSlider ()
{
    setOrientation(Qt::Horizontal);
}


void DirectJumpSlider::mousePressEvent (QMouseEvent* event)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect sr(style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this));


    if (event->button() == Qt::LeftButton && !sr.contains(event->pos()))
    {
        setValue(style()->sliderValueFromPosition(minimum(), maximum(), event->x(), width()));
        event->accept();
    }

    QSlider::mousePressEvent(event);
}

void DirectJumpSlider::mouseReleaseEvent (QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit directJumpFinished();
        event->accept();
    }

    QSlider::mouseReleaseEvent(event);
}
