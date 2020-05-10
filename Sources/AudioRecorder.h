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


        void saveRecording (const QString&);
        void cleanup ();

        bool isPaused ();


    private:
        virtual bool onProcessSamples (const sf::Int16*, std::size_t);
        virtual void onStop ();
        virtual bool onStart ();


        bool paused;

        std::vector<short int>* recordingSamples;
};

#endif // AUDIORECORDER_H
