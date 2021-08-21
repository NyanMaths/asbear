#include <QStyle>
#include <QStyleOptionSlider>
#include <QMouseEvent>

#include "widgets/DirectJumpSlider.hpp"


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
        setValue (style()->sliderValueFromPosition (minimum(), maximum(), event->position().x(), width()));

        emit directJumpOperated();

        event->accept();
    }

    QSlider::mousePressEvent(event);
}
