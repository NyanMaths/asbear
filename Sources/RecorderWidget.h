#ifndef RECORDERWIDGET_H
#define RECORDERWIDGET_H


#include <QTimer>

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include "CustomWidgets/DevicesComboBox.h"
#include "CustomWidgets/AudioLevelWidget.h"
#include "CustomWidgets/SpectrumWidget.h"
#include "CustomWidgets/DirectJumpSlider.h"
#include "RecordingsManagerWidget.h"

#include <QGroupBox>
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

        void changeDefaultDir ();

        void setVolume (int);

        void resetCaptureSettings ();

        void updateTimerLabel ();


    private:
      void loadOptions ();

      void initOptionsBox ();
      void initFilesOptionsBox ();
      void initAdvancedOptionsBox ();
      void initControlsBox ();

      bool getFileInfos (unsigned int&, unsigned short int&);


      RecordingsManagerWidget* recordingsTab;
      QTabWidget* mainWindow;

      AudioRecorder* recorder;
      QTimer* timer;
      QLabel* timerLabel;

      QString outputFileName;
      QString defaultDir;

      QGridLayout* layout;


      QGroupBox* optionsBox;
      QGridLayout* optionsBoxLayout;

        QLabel* chooseDeviceLabel;
        QComboBox* deviceSelecter;

        QLabel* chooseVolumeLabel;
        DirectJumpSlider* volumeSelecter;
        QLabel* overamplificationWarning;

        QLabel* recorderImage;

        QGroupBox* advancedOptionsBox;
        QGridLayout* advancedOptionsBoxLayout;

          QLabel* chooseCodecLabel;
          QComboBox* codecSelecter;

          QLabel* chooseRateLabel;
          QComboBox* rateSelecter;

          QLabel* chooseChannelCountLabel;
          QComboBox* channelCountSelecter;

        QPushButton* bResetCaptureSettings;

      AudioLevelWidget* levelWidget;

      QWidget* filesOptionsWidget;
      QGridLayout* filesOptionsWidgetLayout;
        QLabel* defaultDirLabel;
        QPushButton* bChooseDefaultDir;
        QCheckBox* autoNameRecordings;

      QWidget* controlsWidget;
      QHBoxLayout* controlsLayout;
        QPushButton* bStart;
        QPushButton* bStop;
        QPushButton* bPause;
        QPushButton* bAbort;

      SpectrumWidget* spectrum;
};


#endif // RECORDERWIDGET_H
