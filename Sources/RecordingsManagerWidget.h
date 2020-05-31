#ifndef RECORDINGSMANAGERWIDGET_H
#define RECORDINGSMANAGERWIDGET_H


#include <QPushButton>
#include <QListWidget>

#include <QGridLayout>


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

        void playRecording ();
        void recordingClicked ();

        void displayProperties ();
        void showInExplorer ();
        void move ();

        void deleteRecording ();
        void deleteAllRecordings ();

        void removeFromList ();
        void clearRecordingsList ();


    private:
        void initActions ();

        QGridLayout* layout;

        QListWidget* recordingsList;

        QPushButton* bPlayRecording;
        QPushButton* bProperties;
        QPushButton* bShowInExplorer;
        QPushButton* bMove;
        QPushButton* bRemoveFromList;
        QPushButton* bClearRecordingsList;
        QPushButton* bDeleteRecording;
        QPushButton* bRemoveAllRecordings;
};


#endif // RECORDINGSMANAGERWIDGET_H
