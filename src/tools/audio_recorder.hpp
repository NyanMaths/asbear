#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H


#include <SFML/Audio.hpp>

#include <QObject>


class AudioRecorder : public QObject, public sf::SoundRecorder
{
    Q_OBJECT

    public:
        AudioRecorder();
        virtual ~AudioRecorder();


        void pause();
        void resume();

        bool paused();
        bool recording();

        bool setOutputStream(std::string, unsigned int, unsigned int);
        void setVolume(unsigned short int);

        unsigned int durationAsMilliseconds();


    signals:
        void started();

        void audioLevel(double);


    private:
        virtual bool onProcessSamples(const sf::Int16*, std::size_t);
        virtual void onStop();
        virtual bool onStart();

        double computeLevel(const short int[], std::size_t);


        bool _paused;
        bool _recording;

        unsigned long long int _samplesCount;
        unsigned short int _volume;

        sf::OutputSoundFile outputStream;
};


#endif // AUDIORECORDER_H
