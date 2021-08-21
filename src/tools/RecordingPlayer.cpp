#include "tools/RecordingPlayer.hpp"


RecordingPlayer::RecordingPlayer (QObject* parent) : QObject (parent)
{
    progressionTimer = new QTimer(this);
    progressionTimer->setInterval(10);
    connect(progressionTimer, SIGNAL(timeout()), this, SLOT(progressionTimerTimeout()));
}

bool RecordingPlayer::setFile (const QString& fileName)
{
    if (file.openFromFile(std::string(fileName.toLocal8Bit())))
    {
        _duration = file.getDuration().asMilliseconds();
        return true;
    }

    return false;
}


sf::SoundSource::Status RecordingPlayer::status () const
{
    return file.getStatus();
}

int RecordingPlayer::duration () const
{
    return _duration;
}

int RecordingPlayer::offset () const
{
    return file.getPlayingOffset().asMilliseconds();
}

void RecordingPlayer::setOffset (int newOffset)
{
    file.setPlayingOffset(sf::milliseconds(newOffset));
}


void RecordingPlayer::play ()
{
    file.play();
    progressionTimer->start();
}

void RecordingPlayer::pause ()
{
    file.pause();
}

/*
Logically stops the recording.
There is a signal parameter to prevent the GUI from updating, use with care (obviously false by default) !
*/
void RecordingPlayer::stop (bool signal)
{
    file.stop();
    progressionTimer->stop();

    if (signal)
        emit stopped();
}


void RecordingPlayer::progressionTimerTimeout ()
{
    if (file.getStatus() == sf::SoundSource::Stopped)
        stop();

    if (file.getStatus() == sf::SoundSource::Playing)
        emit progression(file.getPlayingOffset().asMilliseconds());
}
