#ifndef MRECORDER_H
#define MRECORDER_H


#include <QApplication>

#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QLabel>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QMessageBox>
#include <QFileDialog>

#include "AudioRecorder.h"
#include "DevicesComboBox.h"


class MRecorder : public QTabWidget
{
    Q_OBJECT


    public:
        MRecorder ();

        ~MRecorder ();


    private slots:
        void start ();
        void stop ();
        void pause ();
        void abort ();

        void languageChanged ();
        void resetCaptureSettings ();

        void initUIPalette ();

        void deleteRecording ();
        void deleteAllRecordings ();
        void removeFromList ();
        void clearRecordingsList ();
        void playRecording ();

        void recordingClicked ();


    private:
        void initPalettes ();

        void readOptions ();
        void loadOptions ();

        void initRecorder ();
          void initRecordControlsBox ();
          void initOptionsBox ();

        void initRecordingsTab ();

        void initOthers ();


        void closeEvent (QCloseEvent*);


        std::vector<std::string> options;
        QPalette lightPalette;
        QPalette darkPalette;

        QTranslator* translator;
        QTranslator* messageBoxesTranslator;


   ///////////////  "Audio Recorder" tab  ///////////////


        AudioRecorder* recorder;
        QWidget* recorderTab;
        QVBoxLayout* recorderTabLayout;

          QGroupBox* optionsBox;
          QGridLayout* optionsBoxLayout;

            QLabel* chooseDeviceLabel;
            QComboBox* deviceSelecter;
            QTimer* devicesTimer;

            QLabel* chooseCodecLabel;
            QComboBox* codecSelecter;

            QLabel* chooseRateLabel;
            QComboBox* rateSelecter;

            QLabel* chooseChannelCountLabel;
            QComboBox* channelCountSelecter;

            QPushButton* bResetCaptureSettings;

            QLabel* nyanmathsImage;

          QWidget* recordControlsBox;
          QHBoxLayout* recordControlsLayout;

            QPushButton* bStart;
            QPushButton* bStop;
            QPushButton* bPause;
            QPushButton* bAbort;

            QLabel* startLabel;
            QLabel* stopLabel;
            QLabel* pauseLabel;
            QLabel* abortLabel;

          QString outputFileName;


   ///////////////  "Recordings" tab  ///////////////


       QWidget* recordingsTab;
       QGridLayout* recordingsTabLayout;
         QListWidget* recordingsList;
         QPushButton* bPlayRecording;
         QPushButton* bRemoveFromList;
         QPushButton* bClearRecordingsList;
         QPushButton* bDeleteRecording;
         QPushButton* bRemoveAllRecordings;


   ///////////////  "About and settings" tab  ///////////////


        QWidget* othersTab;
        QGridLayout* othersTabLayout;
          QLabel* aboutLabel;

          QGroupBox* UIOptionsBox;
          QGridLayout* UIOptionsBoxLayout;
            QLabel* chooseLanguageLabel;
            QComboBox* languageSelecter;
            QLabel* chooseThemeLabel;
            QComboBox* themeSelecter;
};


#endif // MRECORDER_H
