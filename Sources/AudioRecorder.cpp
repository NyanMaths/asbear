#include "AudioRecorder.h"


////////////////////////////////////////  Constructor / Destructor


AudioRecorder::AudioRecorder () : sf::SoundRecorder ()
{
    _recording = false;
    _paused = false;
}

AudioRecorder::~AudioRecorder ()  // Stop and clean recorder before removal
{
    stop ();
}


////////////////////////////////////////  Controls


void AudioRecorder::pause ()
{
    _paused = true;
}

void AudioRecorder::resume ()
{
    _paused = false;
}


void AudioRecorder::onStop ()  // Called if the user want to stop recording
{
    _recording = false;
    _paused = false;
}

bool AudioRecorder::onStart ()  // Called if the user want to start recording
{
    _samplesCount = 0;
    _paused = false;
    _recording = true;

    return true;
}


////////////////////////////////////////  Others


bool AudioRecorder::setOutputStream (const std::string& filename, const unsigned int& sampleRate, const unsigned int& channelCount)
{
    return outputStream.openFromFile (filename, sampleRate, channelCount);
}


bool AudioRecorder::paused ()
{
    return _paused;
}

bool AudioRecorder::recording ()
{
    return _recording;
}

unsigned int AudioRecorder::recordingTime ()
{
    return _samplesCount / getSampleRate () / getChannelCount ();
}


bool AudioRecorder::onProcessSamples (const sf::Int16* samples, std::size_t samplesCount)  // Saves samples if not paused
{
    if (!_paused)
    {
        outputStream.write (&samples[0], samplesCount);

        _samplesCount += samplesCount;
    }

    return true;
}

