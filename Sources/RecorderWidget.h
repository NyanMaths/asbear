#ifndef RECORDERWIDGET_H
#define RECORDERWIDGET_H


#include <QTimer>

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "DevicesComboBox.h"
#include "RecordingsManagerWidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "AudioRecorder.h"


class RecorderWidget : public QWidget
{
    Q_OBJECT

    public:
        RecorderWidget (QWidget*, RecordingsManagerWidget*);
        ~RecorderWidget ();

        void beforeExit (QCloseEvent*);  // Prompt user to save before exit


    private slots:
        void start ();
        void stop ();
        void pause ();
        void abort ();

        void resetCaptureSettings ();

        void updateTimerLabel ();


    private:
      void loadOptions ();

      void initRecordControlsBox ();
      void initOptionsBox ();


      RecordingsManagerWidget* recordingsTab;
      QWidget* mainWindow;

      AudioRecorder* recorder;
      QTimer* timer;
      QLabel* timerLabel;

      QVBoxLayout* layout;

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
};


#endif // RECORDERWIDGET_H
