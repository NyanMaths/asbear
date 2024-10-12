#include <QApplication>

#include <QPainter>

#include "custom_widgets/level_timeline.hpp"


SpectrumWidget::SpectrumWidget () : QWidget ()
{
    setFixedHeight(102);

    loopCount = 2;
}


void SpectrumWidget::clear ()
{
    levels.clear ();
    levels.shrink_to_fit ();

    update ();
}


void SpectrumWidget::addLevel (double newLevel)
{
    if (loopCount == 2)
    {
        if (levels.size () == width ())
            levels.removeFirst ();

        levels.push_back (newLevel);

        loopCount = 0;
        update ();
    }
    else
        loopCount++;
}

void SpectrumWidget::paintEvent (QPaintEvent*)
{
    QPainter painter (this);

    painter.fillRect (0, 0, width (), 100, qApp->palette ().window ());
    painter.fillRect (0, 49, width (), 2, qApp->palette ().link ());


    unsigned short int picHeight;

    for (short int i = 0 ; i != levels.length () ; i++)
    {
        picHeight = qRound (50 * levels.at (i));

        painter.fillRect (i, 50 - picHeight, 1, picHeight * 2, qApp->palette ().link ());
    }
}
