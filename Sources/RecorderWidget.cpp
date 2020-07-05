#include <QApplication>
#include <QDateTime>

#include <QFile>
#include <fstream>
#include <QStandardPaths>

#include <QMessageBox>
#include <QFileDialog>

#include "RecorderWidget.h"


////////////// Initialize widget


RecorderWidget::RecorderWidget (QTabWidget* parent, RecordingsManagerWidget* displayTab) : QWidget ()
{
    recordingsTab = displayTab;
    mainWindow = parent;

    layout = new QGridLayout (this);


    recorder = new AudioRecorder;

    timer = new QTimer (this);
    timer->setTimerType (Qt::PreciseTimer);
    connect (timer, SIGNAL (timeout ()), this, SLOT (updateTimerLabel ()));

    timerLabel = new QLabel (tr("Begin by clicking on \"Start recording\"..."));
    timerLabel->setAlignment (Qt::AlignCenter);


    initOptionsBox ();


    initControlsBox ();


    levelWidget = new AudioLevelWidget;
    connect (recorder, SIGNAL (audioLevel (double)), levelWidget, SLOT (setLevel (double)));

    spectrum = new SpectrumWidget;
    connect (recorder, SIGNAL (audioLevel (double)), spectrum, SLOT (addLevel (double)));
    connect (recorder, SIGNAL (started ()), spectrum, SLOT (clear ()));


    layout->addWidget (optionsBox, 0, 0);
    layout->addWidget (levelWidget, 0, 1);
    layout->addWidget (filesOptionsWidget, 1, 0, 1, 2);
    layout->addWidget (controlsWidget, 2, 0, 1, 2);
    layout->addWidget (timerLabel, 3, 0, 1, 2);
    layout->addWidget (spectrum, 4, 0, 1, 2);


    loadOptions ();
}

void RecorderWidget::initOptionsBox ()
{
    optionsBox = new QGroupBox (tr("Options"));
    optionsBoxLayout = new QGridLayout (optionsBox);
    optionsBoxLayout->setSpacing (15);

    initFilesOptionsBox ();

    chooseDeviceLabel = new QLabel (tr("Choose microphone :"));
    deviceSelecter = new DevicesComboBox;

    chooseVolumeLabel = new QLabel;
    volumeSelecter = new DirectJumpSlider;
    volumeSelecter->setRange (0, 400);
    connect (volumeSelecter, SIGNAL (valueChanged (int)), this, SLOT (setVolume (int)));

    overamplificationWarning = new QLabel (tr("Values higher than 100% may degrade quality !"));
    overamplificationWarning->setStyleSheet("QLabel{ color : red; font-style : italic; font-size : 13px; }");


    initAdvancedOptionsBox ();


    bResetCaptureSettings = new QPushButton (tr("Reset capture settings"));
    connect (bResetCaptureSettings, SIGNAL (clicked ()), this, SLOT (resetCaptureSettings ()));

    recorderImage = new QLabel;
    recorderImage->setPixmap (QPixmap ("Recorder Image.png"));


    optionsBoxLayout->addWidget (chooseDeviceLabel, 1, 0);
    optionsBoxLayout->addWidget (deviceSelecter, 1, 1);
    optionsBoxLayout->addWidget (chooseVolumeLabel, 2, 0);
    optionsBoxLayout->addWidget (volumeSelecter, 2, 1);
    optionsBoxLayout->addWidget (overamplificationWarning, 3, 0, 1, 2);

    optionsBoxLayout->addWidget (advancedOptionsBox, 4, 0, 1, 2);

    advancedOptionsBoxLayout->addWidget (chooseCodecLabel, 0, 0);
    advancedOptionsBoxLayout->addWidget (codecSelecter, 0, 1);
    advancedOptionsBoxLayout->addWidget (chooseRateLabel, 1, 0);
    advancedOptionsBoxLayout->addWidget (rateSelecter, 1, 1);
    advancedOptionsBoxLayout->addWidget (chooseChannelCountLabel, 2, 0);
    advancedOptionsBoxLayout->addWidget (channelCountSelecter, 2, 1);

    optionsBoxLayout->addWidget (bResetCaptureSettings, 5, 0);

    optionsBoxLayout->addWidget (recorderImage, 4, 2);
}

void RecorderWidget::initFilesOptionsBox ()
{
    filesOptionsWidget = new QWidget;
    filesOptionsWidgetLayout = new QGridLayout (filesOptionsWidget);


    defaultDirLabel = new QLabel;
    bChooseDefaultDir = new QPushButton (tr("Change"));
    connect (bChooseDefaultDir, SIGNAL (clicked ()), this, SLOT (changeDefaultDir ()));

    autoNameRecordings = new QCheckBox (tr("Give a default name to the recordings (date and time)"));


    filesOptionsWidgetLayout->addWidget (defaultDirLabel, 0, 0);
    filesOptionsWidgetLayout->addWidget (bChooseDefaultDir, 0, 1);
    filesOptionsWidgetLayout->addWidget (autoNameRecordings, 1, 0, 1, 2);
}

void RecorderWidget::initAdvancedOptionsBox ()
{
    advancedOptionsBox = new QGroupBox (tr("Advanced coding options"));
    advancedOptionsBox->setCheckable (true);
    advancedOptionsBoxLayout = new QGridLayout (advancedOptionsBox);
    advancedOptionsBoxLayout->setSpacing (15);
    advancedOptionsBoxLayout->setAlignment (Qt::AlignLeft);

    chooseCodecLabel = new QLabel (tr("Audio encoder :"));
    codecSelecter = new QComboBox;

    codecSelecter->addItem (tr("Vorbis (OGG) : compressed, good quality"), QVariant ("ogg"));
    codecSelecter->addItem (tr("FLAC : compressed, best quality"), QVariant ("flac"));
    codecSelecter->addItem (tr("PCM (WAV) : not compressed, best quality"), QVariant ("wav"));


    chooseRateLabel = new QLabel (tr("Sample rate :"));
    rateSelecter = new QComboBox;

    std::ifstream datFile ("Rates.pastouche");
    if (!datFile)
        QMessageBox::critical (this, tr("Fatal error !"), tr("Impossible to load data files, please reinstall this app to fix this !"));

    else
    {
        std::string displayItem, value;

        while (getline (datFile, displayItem) && getline (datFile, value))
            rateSelecter->addItem (QString::fromStdString (displayItem), QVariant (QString::fromStdString (value)));
    }

    chooseChannelCountLabel = new QLabel (tr("Channels :"));
    channelCountSelecter = new QComboBox;
    channelCountSelecter->addItem ("Mono (1)", QVariant (1));
    channelCountSelecter->addItem (tr("Stereo (2)"), QVariant (2));
}

void RecorderWidget::initControlsBox ()
{
    controlsWidget = new QWidget;
    controlsLayout = new QHBoxLayout (controlsWidget);


    bStart = new QPushButton (QIcon ("Start button.png"), tr("Start &recording"));
    bPause = new QPushButton (QIcon ("Pause button.png"), tr("&Pause recording"));
    bStop = new QPushButton (QIcon ("Stop button.png"), tr("&Stop recording"));
    bAbort = new QPushButton (QIcon ("Abort button.png"), tr("Stop without s&aving"));


    connect (bStart, SIGNAL (clicked ()), this, SLOT (start ()));
    connect (bPause, SIGNAL (clicked ()), this, SLOT (pause ()));
    connect (bStop, SIGNAL (clicked ()), this, SLOT (stop ()));
    connect (bAbort, SIGNAL (clicked ()), this, SLOT (abort ()));


    bPause->setEnabled (false);
    bStop->setEnabled (false);
    bAbort->setEnabled (false);


    controlsLayout->addWidget (bStart);
    controlsLayout->addWidget (bPause);
    controlsLayout->addWidget (bStop);
    controlsLayout->addWidget (bAbort);
}


void RecorderWidget::loadOptions ()
{
    QStringList settings = {"0", "3", "1", "0", "100", "Invalid folder", "1"};


    QFile settingsFile ("Recorder Options.pastouche");

    if (settingsFile.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList readSettings = QString (settingsFile.readAll ()).split ("\n");

        if (readSettings.length () == settings.length ())
            settings = readSettings;
    }


    if (!QFile::exists (settings.at (5)))
        settings[5] = QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation).at (0);

    defaultDir = settings.at (5);
    defaultDirLabel->setText (tr("Default directory : ") + defaultDir);

    autoNameRecordings->setChecked (settings.at (6).toUShort ());
    codecSelecter->setCurrentIndex (settings.at (0).toUShort ());
    rateSelecter->setCurrentIndex (settings.at (1).toUShort ());
    channelCountSelecter->setCurrentIndex (settings.at (2).toUShort ());
    advancedOptionsBox->setChecked (settings.at (3).toUShort ());

    volumeSelecter->setValue (settings.at (4).toUShort ());
    setVolume (settings.at (4).toUShort ());
}

RecorderWidget::~RecorderWidget ()
{
    std::ofstream settingsFile ("Recorder Options.pastouche");

    if (settingsFile)
    {
        settingsFile<<codecSelecter->currentIndex ()<<"\n"
                    <<rateSelecter->currentIndex ()<<"\n"
                    <<channelCountSelecter->currentIndex ()<<"\n"
                    <<advancedOptionsBox->isChecked ()<<"\n"
                    <<volumeSelecter->value ()<<"\n"
                    <<defaultDir.toStdString ()<<"\n"
                    <<autoNameRecordings->isChecked ();
    }
}


////////////// Slots


void RecorderWidget::changeDefaultDir ()
{
    QString newDir = QFileDialog::getExistingDirectory (this, tr("Choose the new recordings directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);

    if (!newDir.isEmpty ())
    {
        defaultDir = newDir;
        defaultDirLabel->setText (tr("Default directory : ") + newDir);
    }
}

void RecorderWidget::setVolume (int newVolume)
{
    recorder->setVolume (newVolume);

    chooseVolumeLabel->setText (tr("Input volume (") + QString::number (newVolume) + "%)");
}

void RecorderWidget::resetCaptureSettings ()
{
    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to reset these setings ?")) == QMessageBox::Yes)
    {
        deviceSelecter->setCurrentText (QString::fromStdString (sf::SoundRecorder::getAvailableDevices ().at (0)));

        volumeSelecter->setValue (100);
        codecSelecter->setCurrentIndex (0);
        rateSelecter->setCurrentIndex (3);
        channelCountSelecter->setCurrentIndex (1);
    }
}


void RecorderWidget::updateTimerLabel ()
{
    unsigned int seconds = recorder->durationAsMilliseconds ();
    unsigned int minutes = seconds / 60;
    seconds %= 60;

    if (!minutes)
        timerLabel->setText (seconds == 1 ? tr("Recording time : 1 second") : tr("Recording time : %n seconds", "", seconds));

    else if (seconds)
        timerLabel->setText ((minutes == 1 ? tr("Recording time : 1 minute ") : tr("Recording time : %n minutes ", "", minutes)) +
                             (seconds == 1 ? tr("and 1 second") : tr("and %n seconds", "", seconds)));

    else
        timerLabel->setText (minutes == 1 ? tr("Recording time : 1 minute") : tr("Recording time : %n minutes", "", minutes));
}


bool RecorderWidget::getFileInfos (unsigned int& sampleRate, unsigned short int& channelCount)
{
    QString codec ("ogg");

    if (advancedOptionsBox->isChecked ())
    {
        codec = codecSelecter->currentData ().toString ();
        sampleRate = rateSelecter->currentData ().toUInt ();
        channelCount = channelCountSelecter->currentData ().toUInt ();
    }

    if (autoNameRecordings->isChecked ())
        outputFileName = defaultDir + "/" + QDateTime::currentDateTime ().toString ().replace (":", ".");

    else
        outputFileName = QFileDialog::getSaveFileName (this, tr("Choose output file"), defaultDir, tr("Audio files (*.") + codec + ")", nullptr, QFileDialog::DontConfirmOverwrite | QFileDialog::DontUseNativeDialog);


    if (outputFileName.isEmpty ())
        return false;

    if (!outputFileName.contains (QRegExp ("\\." + codec + "$", Qt::CaseInsensitive)))
        outputFileName += "." + codec;


    if (QFile::exists (outputFileName))
    {
        if (QMessageBox::question (this, tr("Wait a second !"), tr("This file already exists,\nDo you want to replace it ?")) == QMessageBox::No)
        {
            mainWindow->setWindowTitle (tr("MRecorder - Home"));
            return false;
        }
        else
            recordingsTab->removeCurrentFromList ();
    }

    return true;
}

void RecorderWidget::start ()
{
    if (recorder->paused ())
    {
        recorder->resume ();
        timer->start (100);

        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowTitle (tr("MRecorder - Recording..."));

        bStart->setEnabled (false);
        bPause->setEnabled (true);
    }
    else
    {
        mainWindow->setWindowTitle (tr("MRecorder - Start recording ?"));


        unsigned int sampleRate = 44100;
        unsigned short int channelCount = 2;

        if (getFileInfos (sampleRate, channelCount))
        {
            recorder->setOutputStream (std::string (outputFileName.toLocal8Bit ()), sampleRate, channelCount);
            recorder->setDevice (deviceSelecter->currentText ().toStdString ());
            recorder->setChannelCount (channelCount);

            recorder->start (sampleRate);
            timer->start (100);


            mainWindow->setWindowTitle (tr("MRecorder - Recording..."));
            mainWindow->setTabIcon (mainWindow->currentIndex (), QIcon ("Recorder Image.png"));

            bStart->setEnabled (false);
            bPause->setEnabled (true);
            bStop->setEnabled (true);
            bAbort->setEnabled (true);

            optionsBox->setEnabled (false);
        }
        else
            mainWindow->setWindowTitle (tr("MRecorder - Home"));
    }
}


void RecorderWidget::pause ()
{
    recorder->pause ();
    timer->stop ();


    bStart->setText (tr("&Resume recording"));
    mainWindow->setWindowTitle (tr("MRecorder - Paused"));

    bStart->setEnabled (true);
    bPause->setEnabled (false);
}


void RecorderWidget::stop ()
{
    recorder->pause ();

    mainWindow->setWindowTitle (tr("MRecorder - Stop now ?"));


    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to stop recording ?")) == QMessageBox::Yes)
    {
        timer->stop ();
        recorder->stop ();
        recordingsTab->addRecording (outputFileName);


        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowTitle (tr("MRecorder - Home"));
        mainWindow->setTabIcon (mainWindow->currentIndex (), QIcon ());

        optionsBox->setEnabled (true);

        bStart->setEnabled (true);
        bPause->setEnabled (false);
        bStop->setEnabled (false);
        bAbort->setEnabled (false);
    }
    else
    {
        recorder->resume ();

        mainWindow->setWindowTitle (tr("MRecorder - Recording..."));
    }
}


void RecorderWidget::abort ()
{
    recorder->pause ();

    mainWindow->setWindowTitle (tr("MRecorder - Stop without saving ?"));


    if (QMessageBox::question (this, tr("Beware !"), tr("Do you really want to abort recording ?")) == QMessageBox::Yes)
    {
        timer->stop ();
        recorder->stop ();

        QFile::remove (outputFileName);


        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowTitle (tr("MRecorder - Home"));
        mainWindow->setTabIcon (mainWindow->currentIndex (), QIcon ());

        optionsBox->setEnabled (true);

        bStart->setEnabled (true);
        bPause->setEnabled (false);
        bStop->setEnabled (false);
        bAbort->setEnabled (false);
    }
    else
    {
        recorder->resume ();

        mainWindow->setWindowTitle (tr("MRecorder - Recording..."));
    }
}


// If the user closed the app during recording, prompt him/her to save or abort

bool RecorderWidget::beforeExit ()
{
    if (recorder->recording ())
    {
        recorder->pause ();
        mainWindow->setWindowTitle (tr("MRecorder - Paused"));

        QMessageBox::StandardButton answer = QMessageBox::question (this, tr("Wait a second !"), tr("It seems you are still recording,\nDo you want to save before close ?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (answer == QMessageBox::Yes)
        {
            recorder->stop ();
            recordingsTab->addRecording (outputFileName);

            return true;
        }
        else if (answer == QMessageBox::No)
        {
            recorder->stop ();

            QFile::remove (outputFileName);

            return true;
        }
        else
        {
            mainWindow->setWindowTitle (tr("MRecorder - Recording..."));
            recorder->resume ();

            return false;
        }
    }

    return true;
}
