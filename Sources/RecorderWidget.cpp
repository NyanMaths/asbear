#include <QCloseEvent>
#include <QStandardPaths>
#include <QFile>
#include <fstream>

#include <QMessageBox>
#include <QFileDialog>

#include "RecorderWidget.h"


RecorderWidget::RecorderWidget (QWidget* parent, RecordingsManagerWidget* displayTab) : QWidget ()
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

    optionsBoxLayout->addWidget (chooseDeviceLabel, 0, 0);
    optionsBoxLayout->addWidget (deviceSelecter, 0, 1);

    optionsBoxLayout->addWidget (chooseCodecLabel, 1, 0);
    optionsBoxLayout->addWidget (codecSelecter, 1, 1);

    optionsBoxLayout->addWidget (chooseRateLabel, 2, 0);
    optionsBoxLayout->addWidget (rateSelecter, 2, 1);

    optionsBoxLayout->addWidget (chooseChannelCountLabel, 3, 0);
    optionsBoxLayout->addWidget (channelCountSelecter, 3, 1);

    optionsBoxLayout->addWidget (bResetCaptureSettings, 4, 0);

    optionsBoxLayout->addWidget (recorderImage, 0, 2, 4, 1);


    initControlsBox ();


    layout->addWidget (optionsBox, 0, 0, 1, 4);
    layout->addWidget (bStart, 1, 0);
    layout->addWidget (bPause, 1, 1);
    layout->addWidget (bStop, 1, 2);
    layout->addWidget (bAbort, 1, 3);
    layout->addWidget (timerLabel, 2, 0, 1, 4);


    loadOptions ();
}

void RecorderWidget::initOptionsBox ()
{
    optionsBox = new QGroupBox (tr("Options"));
    optionsBoxLayout = new QGridLayout (optionsBox);
    optionsBoxLayout->setSpacing (15);


    chooseDeviceLabel = new QLabel (tr("Choose input device :"));
    deviceSelecter = new DevicesComboBox;

    chooseCodecLabel = new QLabel (tr("Choose audio codec :"));
    codecSelecter = new QComboBox;

    codecSelecter->addItem (tr("Vorbis (OGG) : compressed, good quality"), QVariant ("ogg"));
    codecSelecter->addItem (tr("FLAC : compressed, best quality"), QVariant ("flac"));
    codecSelecter->addItem (tr("PCM (WAV) : not compressed, best quality"), QVariant ("wav"));


    chooseRateLabel = new QLabel (tr("Choose sample rate :"));
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


    bResetCaptureSettings = new QPushButton (tr("Reset capture settings"));
    connect (bResetCaptureSettings, SIGNAL (clicked ()), this, SLOT (resetCaptureSettings ()));


    recorderImage = new QLabel;
    recorderImage->setPixmap (QPixmap ("Recorder Image.png"));
}

void RecorderWidget::initControlsBox ()
{
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
}

void RecorderWidget::loadOptions ()
{
    QFile optionsFile ("Recorder Options.pastouche");

    if (optionsFile.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList settings = QString (optionsFile.readAll ()).split ("\n");

        codecSelecter->setCurrentIndex (settings.at (0).toUShort ());
        rateSelecter->setCurrentIndex (settings.at (1).toUShort ());
        channelCountSelecter->setCurrentIndex (settings.at (2).toUShort ());
    }
    else
    {
        codecSelecter->setCurrentIndex (0);
        rateSelecter->setCurrentIndex (3);
        channelCountSelecter->setCurrentIndex (1);
    }
}


RecorderWidget::~RecorderWidget ()
{
    std::ofstream optionsFile ("Recorder Options.pastouche");

    if (optionsFile)
    {
        optionsFile<<codecSelecter->currentIndex ()<<"\n"
                   <<rateSelecter->currentIndex ()<<"\n"
                   <<channelCountSelecter->currentIndex ();
    }
}


////////////// Slots


void RecorderWidget::resetCaptureSettings ()
{
    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to reset these setings ?")) == QMessageBox::Yes)
    {
        deviceSelecter->setCurrentText (QString::fromStdString (sf::SoundRecorder::getAvailableDevices ().at (0)));

        codecSelecter->setCurrentIndex (0);
        rateSelecter->setCurrentIndex (3);
        channelCountSelecter->setCurrentIndex (1);
    }
}

void RecorderWidget::updateTimerLabel ()
{
    unsigned int seconds = recorder->recordingTime ();
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


void RecorderWidget::start ()
{
    if (recorder->paused ())
    {
        recorder->resume ();


        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowTitle (tr("MRecorder - Recording..."));

        bStart->setEnabled (false);
        bPause->setEnabled (true);
    }
    else
    {
        mainWindow->setWindowTitle (tr("MRecorder - Start recording ?"));


        outputFileName = QFileDialog::getSaveFileName (this, tr("Choose output file"));

        if (!outputFileName.isEmpty ())
        {
            outputFileName += "." + codecSelecter->currentData ().toString ();

            if (QFile::exists (outputFileName))
            {
                if (QMessageBox::question (this, tr("Wait a second !"), tr("This file already exists,\nDo you want to replace it ?")) == QMessageBox::No)
                {
                    mainWindow->setWindowTitle (tr("MRecorder - Home"));
                    return;
                }
            }

            recorder->setOutputStream (outputFileName.toStdString (), rateSelecter->currentData ().toUInt (), channelCountSelecter->currentData ().toUInt ());

            recorder->setDevice (deviceSelecter->currentText ().toStdString ());
            recorder->setChannelCount (channelCountSelecter->currentData ().toUInt ());

            recorder->start (rateSelecter->currentData ().toUInt ());

            timer->start (100);


            mainWindow->setWindowTitle (tr("MRecorder - Recording..."));

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
        recorder->setOutputStream ("", 0, 0);
        recordingsTab->addRecording (outputFileName);


        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowTitle (tr("MRecorder - Home"));

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
        recorder->setOutputStream ("", 0, 0);

        QFile::remove (outputFileName);


        bStart->setText (tr("Start &recording"));
        mainWindow->setWindowTitle (tr("MRecorder - Home"));

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

void RecorderWidget::beforeExit (QCloseEvent* event)
{
    if (recorder->recording ())
    {
        mainWindow->setWindowTitle (tr("MRecorder - Paused"));
        QMessageBox::StandardButton answer = QMessageBox::question (this, tr("Wait a second !"), tr("It seems you are still recording,\nDo you want to save before close ?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (answer == QMessageBox::Yes)
        {
            recorder->stop ();
            recordingsTab->addRecording (outputFileName);

            event->accept ();
        }
        else if (answer == QMessageBox::No)
        {
            recorder->stop ();
            recorder->setOutputStream ("", 0, 0);

            QFile::remove (outputFileName);

            event->accept ();
        }
        else
        {
            mainWindow->setWindowTitle (tr("MRecorder - Recording..."));

            event->ignore ();
        }
    }
}
