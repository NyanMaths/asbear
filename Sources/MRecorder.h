#ifndef MRECORDER_H
#define MRECORDER_H


#include <QApplication>
#include <QProcess>
#include <QCloseEvent>
#include <QFontDatabase>
#include <QTranslator>
#include <QScreen>

#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QMessageBox>
#include <QFileDialog>

#include "AudioRecorder.h"


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
        void refreshDevicesList ();
        void resetCaptureSettings ();


    private:
        void initOptions ();

        void initRecorder ();
          void initRecordControlsBox ();
          void initOptionsBox ();

        void initOthers ();


        void closeEvent (QCloseEvent*);


        QFont UIFont;

        std::vector<std::string> options;

        QTranslator* translator;
        QTranslator* messageBoxesTranslator;


        AudioRecorder* recorder;
        QWidget* recorderTab;
        QVBoxLayout* recorderTabLayout;

          QGroupBox* optionsBox;
          QGridLayout* optionsBoxLayout;

            QLabel* chooseDeviceLabel;
            QComboBox* deviceSelecter;
            QPushButton* bRefreshDevices;

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


        QWidget* othersTab;
        QVBoxLayout* othersTabLayout;

          QLabel* aboutLabel;
          QComboBox* languageSelecter;
};


#endif // MRECORDER_H
