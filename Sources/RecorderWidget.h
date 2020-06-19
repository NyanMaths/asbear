#ifndef RECORDERWIDGET_H
#define RECORDERWIDGET_H


#include <QTimer>

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "CustomWidgets/DevicesComboBox.h"
#include "CustomWidgets/AudioLevelWidget.h"
#include "CustomWidgets/SpectrumWidget.h"
#include "CustomWidgets/DirectJumpSlider.h"
#include "RecordingsManagerWidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "Tools/AudioRecorder.h"


class RecorderWidget : public QWidget
{
    Q_OBJECT

    public:
        RecorderWidget (QTabWidget*, RecordingsManagerWidget*);
        ~RecorderWidget ();

        bool beforeExit ();  // Prompt user to save before exit


    private slots:
        void start ();
        void stop ();
        void pause ();
        void abort ();

        void setVolume (int);

        void resetCaptureSettings ();

        void updateTimerLabel ();


    private:
      void loadOptions ();

      void initControlsBox ();
      void initOptionsBox ();
      void initAdvancedOptionsBox ();

      void getFileInfos (unsigned int&, unsigned short int&);


      RecordingsManagerWidget* recordingsTab;
      QTabWidget* mainWindow;

      AudioRecorder* recorder;
      QTimer* timer;
      QLabel* timerLabel;

      QString outputFileName;


      QGridLayout* layout;

      QGroupBox* optionsBox;
      QGridLayout* optionsBoxLayout;

      QLabel* chooseDeviceLabel;
      QComboBox* deviceSelecter;

      QLabel* chooseVolumeLabel;
      DirectJumpSlider* volumeSelecter;
      QLabel* overamplificationWarning;

        QGroupBox* advancedOptionsBox;
        QGridLayout* advancedOptionsBoxLayout;

          QLabel* chooseCodecLabel;
          QComboBox* codecSelecter;

          QLabel* chooseRateLabel;
          QComboBox* rateSelecter;

          QLabel* chooseChannelCountLabel;
          QComboBox* channelCountSelecter;

        QPushButton* bResetCaptureSettings;

        QLabel* recorderImage;

      AudioLevelWidget* levelWidget;

      QWidget* controlsWidget;
      QHBoxLayout* controlsLayout;
        QPushButton* bStart;
        QPushButton* bStop;
        QPushButton* bPause;
        QPushButton* bAbort;

      SpectrumWidget* spectrum;
};


#endif // RECORDERWIDGET_H
