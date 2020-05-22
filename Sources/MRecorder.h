#ifndef MRECORDER_H
#define MRECORDER_H


#include <QApplication>

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

        void initPalette ();


    private:
        void readOptions ();
        void loadOptions ();

        void initRecorder ();
          void initRecordControlsBox ();
          void initOptionsBox ();

        void initOthers ();


        void closeEvent (QCloseEvent*);


        std::vector<std::string> options;
        QPalette lightPalette;

        QTranslator* translator;
        QTranslator* messageBoxesTranslator;


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
