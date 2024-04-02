#ifndef RECORDINGSMANAGERWIDGET_H
#define RECORDINGSMANAGERWIDGET_H


#include <QPushButton>
#include <QListWidget>

#include <QLabel>
#include "custom_widgets/playback_bar.hpp"
#include <QGridLayout>

#include <SFML/Audio.hpp>
#include "tools/player.hpp"
#include <QTimer>


class ConverterWidget;

class RecordingsManagerWidget : public QWidget
{
    Q_OBJECT

    public:
        RecordingsManagerWidget(QTabWidget*);
        ~RecordingsManagerWidget();

        void setConverter(ConverterWidget*);


    signals:
        void modifiedList();
        void modifiedPlayingStatus();


    public slots:
        void addRecordings(const QString& newRecording = "");
        bool remove(bool prompt = true, QString fileName = "");


    private slots:
        void loadRecording(const QString&);
        void updateActions();
        void updatePlaybackControls();

        void displayProperties();
        void showInExplorer();
        void move();
        void rename();
        void convert();
        void removeFromList();
        // bool remove(...) is a public slot to be used by RecorderWidget
        void removeAllFromList();
        void removeAll();

        void play();
        void pause();
        void stop();


    private:
        void initActions();
        void initPlaybackTools();

        virtual void dragEnterEvent(QDragEnterEvent*);
        virtual void dropEvent(QDropEvent*);


        QTabWidget* mainWindow;
        ConverterWidget* converter;

        RecordingPlayer* recording;


        QGridLayout* layout;

        QLabel* helpLabel;

        QListWidget* recordingsList;
          QPushButton* bAddRecordings;
          QPushButton* bProperties;
          QPushButton* bShowInExplorer;
          QPushButton* bMove;
          QPushButton* bRename;
          QPushButton* bConvert;
          QPushButton* bRemoveFromList;
          QPushButton* bRemove;
          QPushButton* bRemoveAllFromList;
          QPushButton* bRemoveAll;

        QWidget* playbackTools;
        QHBoxLayout* playbackLayout;
          QPushButton* bPlay;
          QPushButton* bPause;
          QPushButton* bStop;
          PlaybackBar* playbackBar;
};


#endif  // RECORDINGSMANAGERWIDGET_H
