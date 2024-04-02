#include <QApplication>

#include <QPainter>

#include "custom_widgets/audio_level_bar.hpp"


AudioLevelWidget::AudioLevelWidget () : QWidget ()
{
    _level = 0;
    setMinimumWidth (30);
}


void AudioLevelWidget::setLevel (double level)
{
    if (level != _level)
    {
        _level = level;
        update ();
    }
}


void AudioLevelWidget::paintEvent (QPaintEvent*)
{
    QPainter painter (this);

    painter.fillRect (0, 0, width (), height () - height () * _level, qApp->palette ().window ());
    painter.fillRect (0, height () - height () * _level, width (), height (), qApp->palette ().link ());
}
