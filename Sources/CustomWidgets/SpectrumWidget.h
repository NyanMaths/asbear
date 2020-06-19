#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H


#include <QWidget>

#include <QPaintEvent>


class SpectrumWidget : public QWidget
{
    Q_OBJECT

    public:
        SpectrumWidget ();


    public slots:
        void addLevel (double);

        void clear ();


    private:
        void paintEvent (QPaintEvent*) override;

        QVector<double> levels;
        unsigned short int loopCount;
};


#endif // SPECTRUMWIDGET_H
