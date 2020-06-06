#ifndef RECORDINGSMANAGERWIDGET_H
#define RECORDINGSMANAGERWIDGET_H


#include <QPushButton>
#include <QListWidget>

#include <QLabel>
#include "CustomWidgets/DirectJumpSlider.h"
#include <QGridLayout>

#include <SFML/Audio.hpp>
#include <QTimer>


class RecordingsManagerWidget : public QWidget
{
    Q_OBJECT

    public:
        RecordingsManagerWidget ();
        ~RecordingsManagerWidget ();

        void addRecording (const QString&);
        void removeCurrentFromList ();


    signals:
        void modifiedList ();


    private slots:
        void updateUI ();
        void updateSlider ();

        void loadCurrentRecording (const QString&);


        void displayProperties ();
        void showInExplorer ();
        void move ();
        void rename ();

        void addRecordings ();
        void deleteRecording ();
        void deleteAllRecordings ();
        void removeFromList ();
        void clearRecordingsList ();


        void play ();
        void stop ();
        void stepForward ();
        void stepBack ();

        void onPressedSlider ();
        void onSliderValueChanged (int);
        void changePlayingOffset ();
        void onReleasedSlider ();


    private:
        void initActions ();
        void initPlaybackTools ();


        QTimer* musicTimer;
        sf::Music recording;
        sf::SoundSource::Status oldStatus;


        QGridLayout* layout;

        QListWidget* recordingsList;
          QPushButton* bProperties;
          QPushButton* bShowInExplorer;
          QPushButton* bMove;
          QPushButton* bRename;
          QPushButton* bAddRecordings;
          QPushButton* bRemoveFromList;
          QPushButton* bClearRecordingsList;
          QPushButton* bDeleteRecording;
          QPushButton* bRemoveAllRecordings;

        QWidget* playbackTools;
        QHBoxLayout* playbackLayout;
          QPushButton* bPlay;
          QPushButton* bStop;
          QPushButton* bStepBack;
          QPushButton* bStepForward;
          DirectJumpSlider* playbackBar;
          QLabel* playbackTimerLabel;
          QLabel* recordingDurationLabel;
};


#endif // RECORDINGSMANAGERWIDGET_H
