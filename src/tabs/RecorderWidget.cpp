#include <QApplication>
#include <QDateTime>

#include <QFile>
#include <QStandardPaths>

#include <QMessageBox>
#include <QFileDialog>

#include "tabs/RecorderWidget.hpp"


////////////// Initialize widget


RecorderWidget::RecorderWidget (QTabWidget* parent, RecordingsManagerWidget* displayTab, const QMap<QString, QString>& config) : QWidget ()
{
    recordingsTab = displayTab;
    mainWindow = parent;

    layout = new QGridLayout(this);


    recorder = new AudioRecorder;

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimerLabel()));

    timerLabel = new QLabel(tr("Begin by clicking on \"Start recording\"..."));
    timerLabel->setAlignment(Qt::AlignCenter);


    initOptionsBox();


    initControlsBox();


    levelWidget = new AudioLevelWidget;
    connect(recorder, SIGNAL(audioLevel(double)), levelWidget, SLOT(setLevel(double)));

    spectrum = new SpectrumWidget;
    connect(recorder, SIGNAL(audioLevel(double)), spectrum, SLOT(addLevel(double)));
    connect(recorder, SIGNAL(started()), spectrum, SLOT(clear()));


    layout->addWidget(optionsBox, 0, 0);
    layout->addWidget(levelWidget, 0, 1);
    layout->addWidget(filesOptionsWidget, 1, 0, 1, 2);
    layout->addWidget(controlsWidget, 2, 0, 1, 2);
    layout->addWidget(timerLabel, 3, 0, 1, 2);
    layout->addWidget(spectrum, 4, 0, 1, 2);


    loadConfig(config);
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

    overamplificationWarning = new QLabel (tr("Values higher than 100% may degrade quality ! You don't need to change it if you have an AGC microphone."));
    overamplificationWarning->setStyleSheet("QLabel{ color: red; font-style: italic; font-size: 13px; }");


    initAdvancedOptionsBox ();


    bResetCaptureSettings = new QPushButton (tr("Reset capture settings"));
    connect (bResetCaptureSettings, SIGNAL (clicked ()), this, SLOT (resetCaptureSettings ()));

    recorderImage = new QLabel;
    recorderImage->setPixmap (QPixmap ("data/gui/recorder-image.png"));


    optionsBoxLayout->addWidget (chooseDeviceLabel, 1, 0);
    optionsBoxLayout->addWidget (deviceSelecter, 1, 1);
    optionsBoxLayout->addWidget (chooseVolumeLabel, 2, 0);
    optionsBoxLayout->addWidget (volumeSelecter, 2, 1);
    optionsBoxLayout->addWidget (overamplificationWarning, 3, 0, 1, 3);

    optionsBoxLayout->addWidget (advancedOptionsBox, 4, 0, 1, 2);

    advancedOptionsBoxLayout->addWidget (chooseCodecLabel, 0, 0);
    advancedOptionsBoxLayout->addWidget (codecSelecter, 0, 1);
    advancedOptionsBoxLayout->addWidget (chooseSampleRateLabel, 1, 0);
    advancedOptionsBoxLayout->addWidget (sampleRateSelecter, 1, 1);
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

    codecSelecter->addItem (tr("Vorbis (OGG) : small size, good quality"), QVariant ("ogg"));
    codecSelecter->addItem (tr("FLAC : medium size, best quality"), QVariant ("flac"));
    codecSelecter->addItem (tr("Raw PCM (WAV) : huge size, best quality"), QVariant ("wav"));


    chooseSampleRateLabel = new QLabel (tr("Sample rate :"));
    sampleRateSelecter = new QComboBox;


    // to file database, maybe
    QVector<uint> sampleRates{8000, 11025, 22050, 44100, 48000, 96000, 192000};

    QVector<QString> displayedSampleRates
    {
        tr("8 kHz (if you know what you do)"),
        tr("11 kHz (same)"),
        tr("22 kHz (same)"),
        tr("44.1 kHz (CD quality, standard)"),
        tr("48 kHz (DVD quality)"),
        tr("96 kHz (Studio quality)"),
        tr("192 kHz (Turbo studio quality)")
    };

    for (ushort i = 0 ; i != sampleRates.length() ; i++)
        sampleRateSelecter->addItem(displayedSampleRates.at(i), QVariant(sampleRates.at(i)));


    chooseChannelCountLabel = new QLabel (tr("Channels :"));
    channelCountSelecter = new QComboBox;
    channelCountSelecter->addItem ("Mono (1)", QVariant (1));
    channelCountSelecter->addItem (tr("Stereo (2)"), QVariant (2));
}

void RecorderWidget::initControlsBox ()
{
    controlsWidget = new QWidget;
    controlsLayout = new QHBoxLayout (controlsWidget);


    bStart = new QPushButton (QIcon ("data/gui/start-button.png"), tr("Start &recording"));
    bPause = new QPushButton (QIcon ("data/gui/pause-button.png"), tr("&Pause recording"));
    bStop = new QPushButton (QIcon ("data/gui/stop-button.png"), tr("&Stop recording"));
    bAbort = new QPushButton (QIcon ("data/gui/abort-button.png"), tr("Stop without s&aving"));


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


void RecorderWidget::loadConfig (const QMap<QString, QString>& config)
{
    volumeSelecter->setValue(config["recorder-input-volume"].toUShort());
    setVolume(volumeSelecter->value());


    advancedOptionsBox->setChecked(config["recorder-advanced-options"].toUShort());
    codecSelecter->setCurrentIndex(config["recorder-codec"].toUShort());
    sampleRateSelecter->setCurrentIndex(config["recorder-sample-rate"].toUShort());
    channelCountSelecter->setCurrentIndex(config["recorder-channel-count"].toUShort());


    if (!QFile::exists(config["recorder-default-dir"]))
        config["recorder-default-dir"] = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0);

    defaultDir = config["recorder-default-dir"];
    defaultDirLabel->setText(tr("Default directory : ") + defaultDir);

    autoNameRecordings->setChecked(config["recorder-auto-name"].toUShort());
}

void RecorderWidget::getConfig (QMap<QString, QString>& config)
{
    config["recorder-input-volume"] = QString::number(volumeSelecter->value());

    config["recorder-advanced-options"] = QString::number(advancedOptionsBox->isChecked());
    config["recorder-codec"] = QString::number(codecSelecter->currentIndex());
    config["recorder-sample-rate"] = QString::number(sampleRateSelecter->currentIndex());
    config["recorder-channel-count"] = QString::number(channelCountSelecter->currentIndex());

    config["recorder-default-dir"] = defaultDir;
    config["recorder-auto-name"] = QString::number(autoNameRecordings->isChecked());
}

RecorderWidget::~RecorderWidget ()
{
    delete recorder;
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
        sampleRateSelecter->setCurrentIndex (3);
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
        sampleRate = sampleRateSelecter->currentData ().toUInt ();
        channelCount = channelCountSelecter->currentData ().toUInt ();
    }

    if (autoNameRecordings->isChecked ())
        outputFileName = defaultDir + QDateTime::currentDateTime ().toString ().replace (":", ".");

    else
        outputFileName = QFileDialog::getSaveFileName (this, tr("Choose output file"), defaultDir, tr("Audio files (*.") + codec + ")", nullptr, QFileDialog::DontConfirmOverwrite | QFileDialog::DontUseNativeDialog);


    if (outputFileName.isEmpty ())
        return false;

    if (!outputFileName.contains (QRegularExpression(QString("\\." + codec + "$"), QRegularExpression::CaseInsensitiveOption)))
        outputFileName += "." + codec;


    if (QFile::exists (outputFileName))
    {
        if (QMessageBox::question (this, tr("Wait a second !"), tr("This file already exists,\nDo you want to replace it ?")) == QMessageBox::No)
        {
            mainWindow->setWindowIcon (QIcon ("data/gui/icon.png"));
            return false;
        }
        else
        {
            if (!recordingsTab->remove(false, outputFileName))
            {
                QMessageBox::critical(this, tr("Not my bad..."), tr("Impossible to remove this annoying file, you will have to try another name !"));

                mainWindow->setWindowIcon (QIcon ("data/gui/icon.png"));
                return false;
            }
        }
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
        mainWindow->setWindowIcon (QIcon ("data/gui/status-recording.png"));

        bStart->setEnabled (false);
        bPause->setEnabled (true);
    }
    else
    {
        unsigned int sampleRate = 44100;
        unsigned short int channelCount = 2;

        if (getFileInfos (sampleRate, channelCount))
        {
            recorder->setOutputStream (std::string (outputFileName.toLocal8Bit ()), sampleRate, channelCount);
            recorder->setDevice (deviceSelecter->currentText ().toStdString ());
            recorder->setChannelCount (channelCount);

            recorder->start (sampleRate);
            timer->start (100);


            mainWindow->setWindowIcon (QIcon ("data/gui/status-recording.png"));

            bStart->setEnabled (false);
            bPause->setEnabled (true);
            bStop->setEnabled (true);
            bAbort->setEnabled (true);

            optionsBox->setEnabled (false);
        }
    }
}


void RecorderWidget::pause ()
{
    recorder->pause ();
    timer->stop ();


    bStart->setText (tr("&Resume recording"));
    mainWindow->setWindowIcon (QIcon ("data/gui/status-paused.png"));

    bStart->setEnabled (true);
    bPause->setEnabled (false);
}


void RecorderWidget::stop ()
{
    bool wasPaused = recorder->paused ();

    recorder->pause ();
    mainWindow->setWindowIcon (QIcon ("data/gui/status-paused.png"));


    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to stop recording ?")) == QMessageBox::Yes)
    {
        timer->stop ();
        recorder->stop ();
        recordingsTab->addRecordings(outputFileName);


        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowIcon (QIcon ("data/gui/icon.png"));

        optionsBox->setEnabled (true);

        bStart->setEnabled (true);
        bPause->setEnabled (false);
        bStop->setEnabled (false);
        bAbort->setEnabled (false);
    }
    else
    {
        if (wasPaused)
            mainWindow->setWindowIcon (QIcon ("data/gui/status-paused.png"));

        else
        {
            recorder->resume ();
            mainWindow->setWindowIcon (QIcon ("data/gui/status-recording.png"));
        }
    }
}


void RecorderWidget::abort ()
{
    bool wasPaused = recorder->paused ();

    recorder->pause ();
    mainWindow->setWindowIcon (QIcon ("data/gui/status-paused.png"));


    if (QMessageBox::question (this, tr("Beware !"), tr("Do you really want to abort recording ?")) == QMessageBox::Yes)
    {
        timer->stop ();
        recorder->stop ();

        QFile::remove (outputFileName);


        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowIcon (QIcon ("data/gui/icon.png"));

        optionsBox->setEnabled (true);

        bStart->setEnabled (true);
        bPause->setEnabled (false);
        bStop->setEnabled (false);
        bAbort->setEnabled (false);
    }
    else
    {
        if (wasPaused)
            mainWindow->setWindowIcon (QIcon ("data/gui/status-paused.png"));

        else
        {
            recorder->resume ();
            mainWindow->setWindowIcon (QIcon ("data/gui/status-recording.png"));
        }
    }
}


// If the user closed the app during recording, prompt him/her to save or abort

bool RecorderWidget::beforeExit ()
{
    if (recorder->recording ())
    {
        bool wasPaused = recorder->paused ();

        recorder->pause ();
        mainWindow->setWindowIcon (QIcon ("data/gui/status-paused.png"));


        QMessageBox::StandardButton answer = QMessageBox::question (this, tr("Wait a second !"), tr("It seems you are still recording,\nDo you want to save before close ?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (answer == QMessageBox::Yes)
        {
            recorder->stop ();
            recordingsTab->addRecordings(outputFileName);

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
            if (wasPaused)
                mainWindow->setWindowIcon (QIcon ("data/gui/status-paused.png"));

            else
            {
                recorder->resume ();
                mainWindow->setWindowIcon (QIcon ("data/gui/status-recording.png"));
            }

            return false;
        }
    }

    return true;
}
