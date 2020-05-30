#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H


#include <SFML/Audio.hpp>


class AudioRecorder : public sf::SoundRecorder
{
    public:
        AudioRecorder ();

        ~AudioRecorder ();


        void pause ();
        void resume ();

        bool paused ();
        bool recording ();

        bool setOutputStream (const std::string&, const unsigned int&, const unsigned int&);

        unsigned int recordingTime ();


    private:
        virtual bool onProcessSamples (const sf::Int16*, std::size_t);
        virtual void onStop ();
        virtual bool onStart ();


        bool _paused;
        bool _recording;

        unsigned long long int _samplesCount;

        sf::OutputSoundFile outputStream;
};

#endif // AUDIORECORDER_H
