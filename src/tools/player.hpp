#ifndef RECORDINGPLAYER_H
#define RECORDINGPLAYER_H


#include <QTimer>

#include <SFML/Audio.hpp>


class RecordingPlayer : public QObject
{
    Q_OBJECT

    public:
        RecordingPlayer(QObject*);


        sf::SoundSource::Status status() const;

        int duration() const;

        int offset() const;
        void setOffset(int);


    signals:
        void stopped();
        void progression(int);


    public slots:
        bool setFile(const QString&);

        void play();
        void pause();
        void stop(bool signal = true);


    private slots:
        void progressionTimerTimeout();


    private:
        sf::Music file;
        int _duration;

        QTimer* progressionTimer;
};


#endif  // RECORDINGPLAYER_H
