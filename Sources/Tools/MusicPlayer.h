#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H


#include <QTimer>

#include <SFML/Audio.hpp>


class MusicPlayer : public QObject
{
    Q_OBJECT

    public:
        MusicPlayer (QObject*);


        int duration ();
        int position ();
        sf::SoundSource::Status state ();

        void setPosition (int);


    signals:
        void stopped ();
        void positionChanged (int);
        void step10Milliseconds (int);


    public slots:
        bool setFile (const QString&);

        void play ();
        void pause ();
        void stop ();
        void close ();

    private slots:
        void emitPosition ();


    private:
        sf::Music file;

        QTimer* timer;
        int counter;

        int _duration;
};


#endif // MUSICPLAYER_H
