#ifndef RECORDINGSMANAGERWIDGET_H
#define RECORDINGSMANAGERWIDGET_H


#include <QPushButton>
#include <QListWidget>

#include <QLabel>
#include "CustomWidgets/DirectJumpSlider.h"
#include <QGridLayout>

#include <SFML/Audio.hpp>
#include <QTimer>


class ConverterWidget;

class RecordingsManagerWidget : public QWidget
{
    Q_OBJECT

    public:
        RecordingsManagerWidget (QTabWidget*);
        ~RecordingsManagerWidget ();

        void addRecording (const QString&);
        void removeCurrentFromList ();

        void setConverter (ConverterWidget*);


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

        void convert ();


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

        virtual void dragEnterEvent (QDragEnterEvent*);
        virtual void dropEvent (QDropEvent*);


        QTabWidget* mainWindow;
        ConverterWidget* converter;

        QTimer* musicTimer;
        sf::Music recording;
        sf::SoundSource::Status oldStatus;


        QGridLayout* layout;

        QLabel* helpLabel;

        QListWidget* recordingsList;
          QPushButton* bAddRecordings;
          QPushButton* bProperties;
          QPushButton* bShowInExplorer;
          QPushButton* bMove;
          QPushButton* bRename;
          QPushButton* bRemoveFromList;
          QPushButton* bClearRecordingsList;
          QPushButton* bDeleteRecording;
          QPushButton* bRemoveAllRecordings;
          QPushButton* bConvert;

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
