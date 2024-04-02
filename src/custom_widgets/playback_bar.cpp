#include "custom_widgets/playback_bar.hpp"


PlaybackBar::PlaybackBar (RecordingPlayer* recording) : QWidget()
{
    this->recording = recording;

    layout = new QHBoxLayout(this);

    offsetLabel = new QLabel("0:00");
    bar = new DirectJumpSlider;
    durationLabel = new QLabel("0:00");


    connect(bar, SIGNAL(sliderPressed()), this, SLOT(pressedSlider()));
    connect(bar, SIGNAL(directJumpFinished()), this, SLOT(updateOffset()));
    connect(bar, SIGNAL(sliderReleased()), this, SLOT(releasedSlider()));

    connect(bar, SIGNAL(valueChanged(int)), this, SLOT(barValueChanged(int)));

    connect(recording, SIGNAL(progression(int)), bar, SLOT(setValue(int)));
    connect(recording, SIGNAL(stopped()), this, SLOT(stoppedRecording()));


    layout->addWidget(offsetLabel);
    layout->addWidget(bar);
    layout->addWidget(durationLabel);
}


void PlaybackBar::loadRecording ()
{
    bar->setRange(0, recording->duration());
    bar->setValue(0);

    uint minutes(recording->duration() / 1000 / 60);
    uint seconds(recording->duration() / 1000 % 60);
    durationLabel->setText(QString::number(minutes) + ":" + (seconds < 10 ? "0" : "") + QString::number(seconds));
}


void PlaybackBar::pressedSlider ()
{
    if (recording->status() == sf::SoundSource::Playing)
        statusBeforeJump = sf::SoundSource::Playing;

    else
        statusBeforeJump = sf::SoundSource::Paused;

    recording->pause();
}

void PlaybackBar::barValueChanged (int value)
{
    uint minutes(value / 1000 / 60);
    uint seconds(value / 1000 % 60);

    offsetLabel->setText(QString::number(minutes) + ":" + (seconds < 10 ? "0" : "") + QString::number(seconds));
}

void PlaybackBar::releasedSlider ()
{
    if (statusBeforeJump == sf::SoundSource::Playing)
        recording->play();
}


void PlaybackBar::updateOffset ()
{
    recording->setOffset(bar->value());
}

void PlaybackBar::stoppedRecording ()
{
    bar->setValue(0);
}
