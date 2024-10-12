#ifndef AUDIOLEVELWIDGET_H
#define AUDIOLEVELWIDGET_H


#include <QWidget>

#include <QPaintEvent>


class AudioLevelWidget : public QWidget
{
    Q_OBJECT

    public:
        AudioLevelWidget();


    public slots:
        void setLevel(double);


    private:
        void paintEvent(QPaintEvent*) override;

        double _level;
};


#endif  // AUDIOLEVELWIDGET_H
