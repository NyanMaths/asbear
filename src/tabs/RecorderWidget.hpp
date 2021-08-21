#ifndef RECORDERWIDGET_H
#define RECORDERWIDGET_H


#include <QTimer>

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include "widgets/DevicesComboBox.hpp"
#include "widgets/AudioLevelWidget.hpp"
#include "widgets/SpectrumWidget.hpp"
#include "widgets/DirectJumpSlider.hpp"
#include "tabs/RecordingsManagerWidget.hpp"

#include <QGroupBox>
#include <QGridLayout>

#include "tools/AudioRecorder.hpp"


class RecorderWidget : public QWidget
{
    Q_OBJECT

    public:
        RecorderWidget(QTabWidget*, RecordingsManagerWidget*, const QMap<QString, QString>&);
        ~RecorderWidget();


        bool beforeExit();  // Prompt user to save before exit

        void getConfig(QMap<QString, QString>&);


    private slots:
        void start();
        void stop();
        void pause();
        void abort();

        void changeDefaultDir();

        void setVolume(int);

        void resetCaptureSettings();

        void updateTimerLabel();


    private:
      void loadConfig(const QMap<QString, QString>&);

      void initOptionsBox();
      void initFilesOptionsBox();
      void initAdvancedOptionsBox();
      void initControlsBox();

      bool getFileInfos(unsigned int&, unsigned short int&);


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

          QLabel* chooseSampleRateLabel;
          QComboBox* sampleRateSelecter;

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


#endif  // RECORDERWIDGET_H
