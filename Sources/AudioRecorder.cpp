#include "AudioRecorder.h"


AudioRecorder::AudioRecorder () : sf::SoundRecorder ()  { }

AudioRecorder::~AudioRecorder ()  // Stop and clean recorder before removal
{
    stop ();
}

bool AudioRecorder::setOutputStream (const std::string& filename, const unsigned int& sampleRate, const unsigned int& channelCount)
{
    return outputStream.openFromFile (filename, sampleRate, channelCount);
}


bool AudioRecorder::onProcessSamples (const sf::Int16* samples, std::size_t samplesCount)  // Saves samples if not paused
{
    if (!paused)
        outputStream.write (&samples[0], samplesCount);

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

