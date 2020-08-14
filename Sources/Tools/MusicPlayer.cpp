#include "MusicPlayer.h"


MusicPlayer::MusicPlayer (QObject* parent) : QObject (parent)
{
    timer = new QTimer (this);

    timer->setTimerType (Qt::PreciseTimer);

    connect (timer, SIGNAL (timeout ()), this, SLOT (emitPosition ()));
}


bool MusicPlayer::setFile (const QString& fileName)
{
    if (file.openFromFile (std::string (fileName.toLocal8Bit ())))
    {
        _duration = file.getDuration ().asMilliseconds ();
        return true;
    }

    return false;
}


int MusicPlayer::duration ()
{
    return _duration;
}

int MusicPlayer::position ()
{
    return file.getPlayingOffset ().asMilliseconds ();
}

sf::SoundSource::Status MusicPlayer::state ()
{
    return file.getStatus ();
}

void MusicPlayer::setPosition (int newPosition)
{
    file.setPlayingOffset (sf::milliseconds (newPosition));

    emit positionChanged (0);
    emit step10Milliseconds (0);
}


void MusicPlayer::play ()
{
    file.play ();

    timer->start (10);
    counter = 0;
}

void MusicPlayer::pause ()
{
    file.pause ();
}

void MusicPlayer::stop ()
{
    file.stop ();

    timer->stop ();

    emit stopped ();
}


void MusicPlayer::close ()
{
    file.stop ();
    timer->stop ();
    file.openFromFile ("");
}


void MusicPlayer::emitPosition ()
{
    //if (file.getStatus () == sf::SoundSource::Playing)
        emit positionChanged (file.getPlayingOffset ().asMilliseconds ());


    if (counter == 9)
    {
        emit step10Milliseconds (file.getPlayingOffset ().asSeconds ());

        if (file.getStatus () == sf::SoundSource::Stopped)
            stop ();
    }
    else
        counter++;
}



