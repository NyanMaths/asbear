#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <SFML/Audio.hpp>

#include <QString>

#include <vector>
#include <fstream>


class AudioRecorder : public sf::SoundRecorder
{
    public:
        AudioRecorder ();

        ~AudioRecorder ();


        void pause ();
        void resume ();

        bool isPaused ();


        bool setOutputStream (const std::string&, const unsigned int&, const unsigned int&);


    private:
        virtual bool onProcessSamples (const sf::Int16*, std::size_t);
        virtual void onStop ();
        virtual bool onStart ();


        bool paused;

        sf::OutputSoundFile outputStream;
};

#endif // AUDIORECORDER_H
