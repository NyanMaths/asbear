#include <thread>

#include "MRecorder.h"


////////////////////////////////////////  Initialize app


MRecorder::MRecorder () : QTabWidget ()
{
    setWindowIcon (QIcon ("Window Icon.png"));


    QFontDatabase::addApplicationFont ("Ubuntu.ttf");
    UIFont = QFont ("Ubuntu", 12);

    qApp->setFont (UIFont);


    initOptions ();


    translator = new QTranslator;
    translator->load("mrecorder_" + QString::fromStdString (options.at (0)));
    qApp->installTranslator (translator);

    messageBoxesTranslator = new QTranslator;
    messageBoxesTranslator->load ("qtbase_" + QString::fromStdString (options.at (0)));
    qApp->installTranslator (messageBoxesTranslator);


    recorderTab = new QWidget;
    othersTab = new QWidget;


    initRecorder (); // Initialize Recorder tab
    initOthers ();   // Initialize About and Settings tab


    show ();

    QSize screenSize = QGuiApplication::screens().at (0)->size ();
    move (screenSize.width () / 2 - width () / 2, screenSize.height () / 2 - height () / 2);

    setFixedSize (size ());
}

void MRecorder::initOptions ()
{
    // Initialize default settings

    options.push_back ("en");
    options.push_back (tr ("Vorbis (OGG) : compressed, good quality, low RAM usage").toStdString ());
    options.push_back ("44.1 kHz (CD)");
    options.push_back ("Language : English");
    options.push_back ("1");


    // Initialize available user settings

    std::ifstream optionsStream ("Options.pastouche");
    if (optionsStream)
    {
        std::string settingValue;

        for (unsigned short int i = 0 ; i != 4 && getline (optionsStream, settingValue) ; i++)
            options[i] = settingValue;
    }
}


MRecorder::~MRecorder ()
{
    std::ofstream optionsStream ("Options.pastouche");

    if (optionsStream)
    {
        optionsStream<<languageSelecter->currentData ().toString ().toStdString ()<<"\n"
                     <<codecSelecter->currentText ().toStdString ()<<"\n"
                     <<rateSelecter->currentText ().toStdString ()<<"\n"
                     <<languageSelecter->currentText ().toStdString ()<<"\n"
                     <<languageSelecter->currentIndex ();

    }
}


/////////////////////////  Tabs


void MRecorder::initRecorder ()
{
    addTab (recorderTab, tr("Audio Recorder"));
    recorderTabLayout = new QVBoxLayout (recorderTab);

    recorder = new AudioRecorder;


    initOptionsBox ();


    optionsBoxLayout->addWidget (chooseDeviceLabel, 0, 0);
    optionsBoxLayout->addWidget (deviceSelecter, 0, 1);
    optionsBoxLayout->addWidget (bRefreshDevices, 0, 2);

    optionsBoxLayout->addWidget (chooseCodecLabel, 1, 0);
    optionsBoxLayout->addWidget (codecSelecter, 1, 1);

    optionsBoxLayout->addWidget (chooseRateLabel, 2, 0);
    optionsBoxLayout->addWidget (rateSelecter, 2, 1);

    optionsBoxLayout->addWidget (chooseChannelCountLabel, 3, 0);
    optionsBoxLayout->addWidget (channelCountSelecter, 3, 1);

    optionsBoxLayout->addWidget (bResetCaptureSettings, 4, 0);

    optionsBoxLayout->addWidget (nyanmathsImage, 1, 2, 3, 1);


    initRecordControlsBox ();
}

void MRecorder::initOptionsBox ()
{
    optionsBox = new QGroupBox (tr("Options"));
    recorderTabLayout->addWidget (optionsBox);
    optionsBoxLayout = new QGridLayout (optionsBox);


    chooseDeviceLabel = new QLabel (tr("Choose input device :"));

    deviceSelecter = new QComboBox;
    refreshDevicesList ();

    bRefreshDevices = new QPushButton (tr("Refresh"));
    connect (bRefreshDevices, SIGNAL (clicked ()), this, SLOT (refreshDevicesList ()));


    chooseCodecLabel = new QLabel (tr("Choose audio codec :"));
    codecSelecter = new QComboBox;

    codecSelecter->addItem (tr("Vorbis (OGG) : compressed, good quality, low RAM usage"), QVariant ("ogg"));
    codecSelecter->addItem (tr("FLAC : compressed, best quality, medium RAM usage"), QVariant ("flac"));
    codecSelecter->addItem (tr("PCM (WAV) : not compressed, best quality, high RAM usage"), QVariant ("wav"));

    codecSelecter->setCurrentText (QString::fromStdString (options.at (1)));


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

        rateSelecter->setCurrentText (QString::fromStdString (options.at (2)));
    }

    chooseChannelCountLabel = new QLabel (tr("Channels :"));
    channelCountSelecter = new QComboBox;
    channelCountSelecter->addItem ("Mono (1)", QVariant (1));
    channelCountSelecter->addItem (tr("Stereo (2)"), QVariant (2));

    channelCountSelecter->setCurrentIndex (QString::fromStdString (options.at (4)).toUInt ());


    bResetCaptureSettings = new QPushButton (tr("Reset capture settings"));
    connect (bResetCaptureSettings, SIGNAL (clicked ()), this, SLOT (resetCaptureSettings ()));


    nyanmathsImage = new QLabel;
    nyanmathsImage->setPixmap (QPixmap ("NY4N_M4THS.png"));
}

void MRecorder::initRecordControlsBox ()
{
    recordControlsBox = new QWidget;
    recorderTabLayout->addWidget (recordControlsBox);
    recordControlsLayout = new QHBoxLayout (recordControlsBox);


    bStart = new QPushButton (QIcon ("Start button.png"), tr("Start recording"));
    bStop = new QPushButton (QIcon ("Stop button.png"), tr("Stop recording"));
    bPause = new QPushButton (QIcon ("Pause button.png"), tr("Pause recording"));
    bAbort = new QPushButton (QIcon ("Abort button.png"), tr("Stop without saving"));

    connect (bStart, SIGNAL (clicked ()), this, SLOT (start ()));
    connect (bStop, SIGNAL (clicked ()), this, SLOT (stop ()));
    connect (bPause, SIGNAL (clicked ()), this, SLOT (pause ()));
    connect (bAbort, SIGNAL (clicked ()), this, SLOT (abort ()));


    recordControlsLayout->addWidget (bStart);
    recordControlsLayout->addWidget (bStop);
    recordControlsLayout->addWidget (bPause);
    recordControlsLayout->addWidget (bAbort);


    bStop->setEnabled (false);
    bPause->setEnabled (false);
    bAbort->setEnabled (false);
}


void MRecorder::initOthers ()
{
    addTab (othersTab, tr("About and settings"));
    othersTabLayout = new QVBoxLayout (othersTab);


    aboutLabel = new QLabel (tr("This opensource software was developed by NY4N_M4THS (MemeTech INC)<br/>"
                                "under <a href = 'https://gnu.org/licenses/lgpl-3.0.en.html'>LGPL 3.1</a> with the framework <a href = 'https://qt.io'>Qt</a> and <a href = 'https://sfml-dev.org'>SFML</a> in C++ language.<br/>"
                                "You can also visit <a href = 'https://memetech-inc.weebly.com'>our website</a> to check for updates,<br/>"
                                "try other of our applications or ask for new features !<br/>"
                                "Click <a href = 'https://github.com/NyanMaths/MRecorder'>here</a> to visit the GitHub page of the project.<br/><br/>"));

    aboutLabel->setOpenExternalLinks (true);
    aboutLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    aboutLabel->setTextFormat(Qt::RichText);


    languageSelecter = new QComboBox;
    languageSelecter->addItem ("Language : English", QVariant ("en"));
    languageSelecter->addItem ("Langue : Français", QVariant ("fr"));

    languageSelecter->setCurrentText (QString::fromStdString (options.at (3)));

    connect (languageSelecter, SIGNAL (currentIndexChanged (int)), this, SLOT (languageChanged (int)));


    othersTabLayout->addWidget (aboutLabel);
    othersTabLayout->addWidget (languageSelecter);
}


////////////////////////////////////////  Slots


void MRecorder::refreshDevicesList ()
{
    deviceSelecter->clear ();
    std::vector<std::string> devices = sf::SoundRecorder::getAvailableDevices ();

    for (unsigned short int i = 0 ; i != devices.size () ; i++)
        deviceSelecter->addItem (QString::fromStdString (devices.at (i)));

    deviceSelecter->setCurrentIndex (0);
}


void MRecorder::languageChanged (int)
{
    if (QMessageBox::question (this, tr("Language changed"), tr("You need to reload the application to apply changes.\nDo you want to restart now ?")) == QMessageBox::Yes)
    {
        qApp->quit ();
        QProcess::startDetached (qApp->arguments ()[0], qApp->arguments ());
    }
}


void MRecorder::resetCaptureSettings ()
{
    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to reset these setings ?")) == QMessageBox::Yes)
    {
        deviceSelecter->setCurrentText (QString::fromStdString (sf::SoundRecorder::getAvailableDevices ().at (0)));

        codecSelecter->setCurrentText (tr("Vorbis (OGG) : compressed, good quality, low RAM usage"));

        rateSelecter->setCurrentText ("44.1 kHz (CD)");

        channelCountSelecter->setCurrentText (tr("Stereo (2)"));
    }
}

void MRecorder::reactivateUI ()
{
    setEnabled (true);
    optionsBox->setEnabled (true);

    bStart->setEnabled (true);
    bStop->setEnabled (false);
    bPause->setEnabled (false);
    bAbort->setEnabled (false);
}


void MRecorder::start ()
{
    bStart->setText (tr("Start recording"));

    bStart->setEnabled (false);
    bStop->setEnabled (true);
    bPause->setEnabled (true);
    bAbort->setEnabled (true);

    optionsBox->setEnabled (false);


    if (recorder->isPaused ())
        recorder->resume ();

    else
    {
        recorder->setDevice (deviceSelecter->currentText ().toStdString ());
        recorder->setChannelCount (channelCountSelecter->currentData ().toUInt ());

        recorder->start (rateSelecter->currentData ().toUInt ());
    }
}

void MRecorder::stop ()
{
    recorder->pause ();


    QString fileName = QFileDialog::getSaveFileName (this, tr("Choose path to save your recording"));

    if (!fileName.isEmpty ())
    {
        recorder->stop ();
        setEnabled (false);  // Saving may take a while, it's better to disable the window


        saveThread = QThread::create (&MRecorder::save, this, fileName + "." + codecSelecter->currentData ().toString ());
        saveThread->start ();

        connect (saveThread, SIGNAL (finished ()), this, SLOT (reactivateUI ()));

        bStart->setText (tr("Start recording"));
    }
    else
        recorder->resume ();
}


void MRecorder::pause ()
{
    bStart->setText (tr("Resume recording"));

    bPause->setEnabled (false);
    bStart->setEnabled (true);


    recorder->pause ();
}

void MRecorder::abort ()
{
    recorder->pause ();

    if (QMessageBox::question (this, tr("Careful !"), tr("Do you really want to abort recording ?")) == QMessageBox::Yes)
    {
        recorder->stop ();
        recorder->cleanup ();

        bStart->setText (tr("Start recording"));

        optionsBox->setEnabled (true);

        bStart->setEnabled (true);
        bStop->setEnabled (false);
        bPause->setEnabled (false);
        bAbort->setEnabled (false);
    }
    else
        recorder->resume ();
}


////////////////////////////////////////  Others


void MRecorder::save (QString fileName)
{
    recorder->saveRecording (fileName);
}

