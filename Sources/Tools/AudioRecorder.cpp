#include "AudioRecorder.h"


////////////////////////////////////////  Constructor / Destructor


AudioRecorder::AudioRecorder () : QObject (), sf::SoundRecorder ()
{
    _recording = false;
    _paused = false;
    _volume = 1;

    setProcessingInterval (sf::milliseconds (10));
}

AudioRecorder::~AudioRecorder ()  // Stop and clean recorder before removal
{
    stop ();
}


////////////////////////////////////////  Controls


void AudioRecorder::pause ()
{
    _paused = true;
    emit audioLevel (0);
}

void AudioRecorder::resume ()
{
    _paused = false;
}


bool AudioRecorder::onStart ()  // Called if the user want to start recording
{
    _samplesCount = 0;
    _paused = false;
    _recording = true;

    emit started ();
    return true;
}

void AudioRecorder::onStop ()  // Called if the user want to stop recording
{
    _recording = false;
    _paused = false;

    emit audioLevel (0);

    outputStream.openFromFile ("", 0, 0);
}


////////////////////////////////////////  Others


bool AudioRecorder::setOutputStream (std::string fileName, unsigned int sampleRate, unsigned int channelCount)
{
    return outputStream.openFromFile (fileName, sampleRate, channelCount);
}

void AudioRecorder::setVolume (unsigned short int volume)
{
    _volume = volume;
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


bool AudioRecorder::onProcessSamples (const sf::Int16* samples, std::size_t samplesCount)
{
    if (!_paused)
    {
        if (_volume != 100)
        {
            float volumeCoefficient = float (_volume) / 100.0;

            short int amplifiedSamples[samplesCount];
            int amplifiedSample;

            for (unsigned long long int i = 0 ; i != samplesCount ; i++)
            {
                amplifiedSample = samples[i] * volumeCoefficient;

                if (amplifiedSample > 32767)
                    amplifiedSamples[i] = 32767;

                else if (amplifiedSample < -32768)
                    amplifiedSamples[i] = -32768;

                else
                    amplifiedSamples[i] = amplifiedSample;
            }

            outputStream.write (amplifiedSamples, samplesCount);

            emit audioLevel (computeLevel (amplifiedSamples, samplesCount) / 32767.0);
        }
        else
        {
            outputStream.write (samples, samplesCount);

            emit audioLevel (computeLevel (samples, samplesCount) / 32767.0);
        }

        _samplesCount += samplesCount;
    }

    return true;
}


unsigned short int AudioRecorder::computeLevel (const short int samples[], std::size_t samplesCount)
{
    unsigned long long int level = 0;

    for (unsigned int i = 0 ; i != samplesCount ; i++)
        level += abs (samples[i]);


    return level / samplesCount;
}

