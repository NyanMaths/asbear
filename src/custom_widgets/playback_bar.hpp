#ifndef PLAYBACKBAR_H
#define PLAYBACKBAR_H


#include <QHBoxLayout>

#include <QWidget>
#include <QLabel>
#include "widgets/DirectJumpSlider.hpp"

#include <SFML/Audio.hpp>
#include "tools/RecordingPlayer.hpp"


class PlaybackBar : public QWidget
{
    Q_OBJECT


    public:
        explicit PlaybackBar(RecordingPlayer*);

        void loadRecording();


    private slots:
        void pressedSlider();
        void barValueChanged(int);
        void releasedSlider();

        void updateOffset();

        void stoppedRecording();


    private:
        RecordingPlayer* recording;
        sf::SoundSource::Status statusBeforeJump;

        QHBoxLayout* layout;
        QLabel* offsetLabel;
        DirectJumpSlider* bar;
        QLabel* durationLabel;
};


#endif  // PLAYBACKBAR_H
