#include "AudioRecorder.h"


AudioRecorder::AudioRecorder () : sf::SoundRecorder ()
{
    recordingSamples = new std::vector<short int>;
}

AudioRecorder::~AudioRecorder ()  // Stop and clean recorder before removal
{
    stop ();

    cleanup ();
    delete recordingSamples;
}


bool AudioRecorder::onProcessSamples (const sf::Int16* samples, std::size_t samplesCount)  // Saves samples if not paused
{
    if (!paused)
    {
        for (unsigned int i = 0 ; i != samplesCount ; i++)
            recordingSamples->push_back (samples[i]);
    }

    return true;
}

bool AudioRecorder::onStart ()  // Called if the user want to start recording
{
    paused = false;
    return true;
}

void AudioRecorder::onStop ()  // Called if the user want to stop recording
{
    paused = false;
}


void AudioRecorder::saveRecording (const QString& fileName)
{
    // Save recording

    sf::SoundBuffer recordingBuffer;
    recordingBuffer.loadFromSamples (&recordingSamples->at (0), recordingSamples->size (), getChannelCount (), getSampleRate ());

    recordingBuffer.saveToFile (fileName.toStdString ());


    cleanup ();  // And clean !
}

void AudioRecorder::cleanup ()
{
    recordingSamples->clear ();
    recordingSamples->shrink_to_fit ();
    recordingSamples->resize (0);
}


void AudioRecorder::pause ()
{
    paused = true;
}

void AudioRecorder::resume ()
{
    paused = false;
}


bool AudioRecorder::isPaused ()
{
    return paused;
}

