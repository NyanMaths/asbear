#ifndef DIRECTJUMPSLIDER_H
#define DIRECTJUMPSLIDER_H


#include <QSlider>


class DirectJumpSlider : public QSlider
{
    Q_OBJECT

    public:
        explicit DirectJumpSlider();


    signals:
        void directJumpOperated();


    private:
        virtual void mousePressEvent(QMouseEvent*);
};


#endif  // DIRECTJUMPSLIDER_H
