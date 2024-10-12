#ifndef DIRECTJUMPSLIDER_H
#define DIRECTJUMPSLIDER_H


#include <QSlider>


class DirectJumpSlider : public QSlider
{
    Q_OBJECT

    public:
        explicit DirectJumpSlider();


    signals:
        void directJumpFinished();


    private:
        virtual void mousePressEvent(QMouseEvent*);
        virtual void mouseReleaseEvent(QMouseEvent*);
};


#endif  // DIRECTJUMPSLIDER_H
