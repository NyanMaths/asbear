#include <QProcess>
#include <QCloseEvent>

#include <QFontDatabase>
#include <QStyleFactory>
#include <QTranslator>
#include <QScreen>

#include <fstream>

#include "MRecorder.h"


////////////////////////////////////////  Initialize app


MRecorder::MRecorder () : QTabWidget ()
{
    QFontDatabase::addApplicationFont ("Ubuntu.ttf");


    readOptions ();  // Read options file

    translator = new QTranslator;
    translator->load("mrecorder_" + QString::fromStdString (options.at (0)));
    qApp->installTranslator (translator);

    messageBoxesTranslator = new QTranslator;
    messageBoxesTranslator->load ("qtbase_" + QString::fromStdString (options.at (0)));
    qApp->installTranslator (messageBoxesTranslator);


    initRecorder (); // Initialize Recorder tab
    initOthers ();   // Initialize About and Settings tab

    loadOptions ();  // Load user's settings


    setWindowTitle (tr("MRecorder - Home"));
    setWindowIcon (QIcon ("Window Icon.png"));


    initPalette ();


    show ();

    QSize screenSize = QGuiApplication::screens ().at (0)->size ();
    move (screenSize.width () / 2 - width () / 2, screenSize.height () / 2 - height () / 2);

    setFixedSize (size ());
}


void MRecorder::initPalette ()
{
    options[5] = themeSelecter->currentText ().toStdString ();

    if (options.at (5) == tr("Classic Light").toStdString ())
    {
        qApp->setStyle (QStyleFactory::create ("WindowsVista"));
        qApp->setPalette (lightPalette);
    }
    else if (options.at (5) == tr("Fusion Light").toStdString ())
    {
        qApp->setStyle (QStyleFactory::create ("Fusion"));
        qApp->setPalette (lightPalette);
    }
    else
    {
        qApp->setStyle (QStyleFactory::create ("Fusion"));

        QPalette darkPalette;
        QColor darkColor = QColor (45, 45, 45);
        QColor disabledColor = QColor (127, 127, 127);

        darkPalette.setColor (QPalette::Window, darkColor);
        darkPalette.setColor (QPalette::WindowText, Qt::white);
        darkPalette.setColor (QPalette::Base, QColor (18, 18, 18));
        darkPalette.setColor (QPalette::AlternateBase, darkColor);
        darkPalette.setColor (QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor (QPalette::ToolTipText, Qt::white);
        darkPalette.setColor (QPalette::Text, Qt::white);
        darkPalette.setColor (QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor (QPalette::Button, darkColor);
        darkPalette.setColor (QPalette::ButtonText, Qt::white);
        darkPalette.setColor (QPalette::Disabled, QPalette::ButtonText, disabledColor);
        darkPalette.setColor (QPalette::BrightText, Qt::red);
        darkPalette.setColor (QPalette::Link, QColor (42, 130, 218));

        darkPalette.setColor (QPalette::Highlight, QColor (42, 130, 218));
        darkPalette.setColor (QPalette::HighlightedText, Qt::black);
        darkPalette.setColor (QPalette::Disabled, QPalette::HighlightedText, disabledColor);

        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);

        qApp->setPalette (darkPalette);
    }
    qApp->setFont (QFont ("Ubuntu", 12));
}


//////////////  Options


void MRecorder::readOptions ()
{
    // Initialize default settings

    options.push_back ("en");
    options.push_back (tr("Vorbis (OGG) : compressed, good quality").toStdString ());
    options.push_back ("44.1 kHz (CD)");
    options.push_back ("English");
    options.push_back ("1");
    options.push_back (tr("Classic Light").toStdString ());


    // Initialize available user settings

    std::ifstream optionsStream ("Options.pastouche");
    if (optionsStream)
    {
        std::string settingValue;

        for (unsigned short int i = 0 ; i != 6 && getline (optionsStream, settingValue) ; i++)
            options[i] = settingValue;
    }

    lightPalette = qApp->palette ();
}

void MRecorder::loadOptions ()
{
    codecSelecter->setCurrentText (QString::fromStdString (options.at (1)));
    rateSelecter->setCurrentText (QString::fromStdString (options.at (2)));
    languageSelecter->setCurrentText (QString::fromStdString (options.at (3)));
    channelCountSelecter->setCurrentIndex (QString::fromStdString (options.at (4)).toUInt ());
    themeSelecter->setCurrentText (QString::fromStdString (options.at (5)));


    connect (languageSelecter, SIGNAL (currentIndexChanged (int)), this, SLOT (languageChanged ()));

    connect (themeSelecter, SIGNAL (currentIndexChanged (int)), this, SLOT (initPalette ()));
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
                     <<channelCountSelecter->currentIndex ()<<"\n"
                     <<themeSelecter->currentText ().toStdString ();
    }
}


/////////////////////////  "Audio recorder" tab


void MRecorder::initRecorder ()
{
    recorderTab = new QWidget;
    addTab (recorderTab, tr("Audio Recorder"));
    recorderTabLayout = new QVBoxLayout (recorderTab);

    recorder = new AudioRecorder;


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

    optionsBoxLayout->addWidget (nyanmathsImage, 1, 2, 3, 1);


    initRecordControlsBox ();
}

void MRecorder::initOptionsBox ()
{
    optionsBox = new QGroupBox (tr("Options"));
    recorderTabLayout->addWidget (optionsBox);
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


    nyanmathsImage = new QLabel;
    nyanmathsImage->setPixmap (QPixmap ("NY4N_M4THS.png"));
}

void MRecorder::initRecordControlsBox ()
{
    recordControlsBox = new QWidget;
    recorderTabLayout->addWidget (recordControlsBox);

    recordControlsLayout = new QHBoxLayout (recordControlsBox);


    bStart = new QPushButton (QIcon ("Start button.png"), tr("Start &recording"));
    bPause = new QPushButton (QIcon ("Pause button.png"), tr("&Pause recording"));
    bStop = new QPushButton (QIcon ("Stop button.png"), tr("&Stop recording"));
    bAbort = new QPushButton (QIcon ("Abort button.png"), tr("Stop without s&aving"));

    connect (bStart, SIGNAL (clicked ()), this, SLOT (start ()));
    connect (bPause, SIGNAL (clicked ()), this, SLOT (pause ()));
    connect (bStop, SIGNAL (clicked ()), this, SLOT (stop ()));
    connect (bAbort, SIGNAL (clicked ()), this, SLOT (abort ()));


    recordControlsLayout->addWidget (bStart);
    recordControlsLayout->addWidget (bPause);
    recordControlsLayout->addWidget (bStop);
    recordControlsLayout->addWidget (bAbort);


    bPause->setEnabled (false);
    bStop->setEnabled (false);
    bAbort->setEnabled (false);
}


////////////// Slots


void MRecorder::resetCaptureSettings ()
{
    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to reset these setings ?")) == QMessageBox::Yes)
    {
        deviceSelecter->setCurrentText (QString::fromStdString (sf::SoundRecorder::getAvailableDevices ().at (0)));

        codecSelecter->setCurrentText (tr("Vorbis (OGG) : compressed, good quality"));

        rateSelecter->setCurrentText ("44.1 kHz (CD)");

        channelCountSelecter->setCurrentText (tr("Stereo (2)"));
    }
}


void MRecorder::start ()
{
    if (recorder->paused ())
    {
        recorder->resume ();


        bStart->setText (tr("Start &recording"));
        setWindowTitle (tr("MRecorder - Recording..."));

        bStart->setEnabled (false);
        bPause->setEnabled (true);
    }
    else
    {
        setWindowTitle (tr("MRecorder - Start recording ?"));


        outputFileName = QFileDialog::getSaveFileName (this, tr("Choose output file"));

        if (!outputFileName.isEmpty ())
        {
            recorder->setOutputStream ((outputFileName + "." + codecSelecter->currentData ().toString ()).toStdString (), rateSelecter->currentData ().toUInt (), channelCountSelecter->currentData ().toUInt ());

            recorder->setDevice (deviceSelecter->currentText ().toStdString ());
            recorder->setChannelCount (channelCountSelecter->currentData ().toUInt ());

            recorder->start (rateSelecter->currentData ().toUInt ());


            setWindowTitle (tr("MRecorder - Recording..."));

            bStart->setEnabled (false);
            bPause->setEnabled (true);
            bStop->setEnabled (true);
            bAbort->setEnabled (true);

            optionsBox->setEnabled (false);
        }
    }
}

void MRecorder::pause ()
{
    recorder->pause ();


    bStart->setText (tr("&Resume recording"));
    setWindowTitle (tr("MRecorder - Paused"));

    bStart->setEnabled (true);
    bPause->setEnabled (false);
}

void MRecorder::stop ()
{
    recorder->pause ();

    setWindowTitle (tr("MRecorder - Stop now ?"));


    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to stop recording ?")) == QMessageBox::Yes)
    {
        recorder->stop ();
        recorder->setOutputStream ("", 0, 0);


        bStart->setText (tr("Start &recording"));
        setWindowTitle (tr("MRecorder - Home"));

        optionsBox->setEnabled (true);

        bStart->setEnabled (true);
        bPause->setEnabled (false);
        bStop->setEnabled (false);
        bAbort->setEnabled (false);
    }
    else
    {
        recorder->resume ();

        setWindowTitle (tr("MRecorder - Recording..."));
    }
}

void MRecorder::abort ()
{
    recorder->pause ();

    setWindowTitle (tr("MRecorder - Stop without saving ?"));


    if (QMessageBox::question (this, tr("Beware !"), tr("Do you really want to abort recording ?")) == QMessageBox::Yes)
    {
        recorder->stop ();
        recorder->setOutputStream ("", 0, 0);

        QFile::remove (outputFileName + "." + codecSelecter->currentData ().toString ());


        bStart->setText (tr("Start &recording"));
        setWindowTitle (tr("MRecorder - Home"));

        optionsBox->setEnabled (true);

        bStart->setEnabled (true);
        bPause->setEnabled (false);
        bStop->setEnabled (false);
        bAbort->setEnabled (false);
    }
    else
    {
        recorder->resume ();

        setWindowTitle (tr("MRecorder - Recording..."));
    }
}


// If the user closed the app during recording, prompt him/her to save or abort

void MRecorder::closeEvent (QCloseEvent* event)
{
    if (recorder->recording ())
    {
        QMessageBox::StandardButton answer = QMessageBox::question (this, tr("Wait a second !"), tr("It seems you are still recording,\nDo you want to save before close ?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (answer == QMessageBox::Yes)
        {
            recorder->stop ();
            event->accept ();
        }
        else if (answer == QMessageBox::No)
        {
            recorder->stop ();
            recorder->setOutputStream ("", 0, 0);

            QFile::remove (outputFileName + "." + codecSelecter->currentData ().toString ());

            event->accept ();
        }
        else
            event->ignore ();
    }
}


////////////////////////////////////////  "About and settings" tab


void MRecorder::initOthers ()
{
    othersTab = new QWidget;
    addTab (othersTab, tr("About and settings"));
    othersTabLayout = new QGridLayout (othersTab);


    aboutLabel = new QLabel (tr(
        "This opensource software was developed by NY4N_M4THS (MemeTech INC)<br/>"
        "under <a href = 'https://gnu.org/licenses/lgpl-3.0.en.html'>LGPL 3.0</a> with the framework <a href = 'https://qt.io'>Qt</a> and <a href = 'https://sfml-dev.org'>SFML</a> in C++ language.<br/>"
        "You can also visit <a href = 'https://memetech-inc.weebly.com'>our website</a> to check for updates,<br/>"
        "try other of our applications or <a href = 'https://github.com/NyanMaths/MRecorder/issues'>ask for new features</a> !<br/>"
        "Click <a href = 'https://github.com/NyanMaths/MRecorder'>here</a> to visit the GitHub page of the project.<br/><br/>"));

    aboutLabel->setOpenExternalLinks (true);
    aboutLabel->setTextInteractionFlags (Qt::TextBrowserInteraction);
    aboutLabel->setTextFormat (Qt::RichText);


    UIOptionsBox = new QGroupBox (tr("UI options"));
    UIOptionsBoxLayout = new QGridLayout (UIOptionsBox);
    UIOptionsBoxLayout->setAlignment (Qt::AlignLeft);

    chooseLanguageLabel = new QLabel (tr("Choose UI language :"));

    languageSelecter = new QComboBox;
    languageSelecter->addItem ("English", QVariant ("en"));
    languageSelecter->addItem ("Français", QVariant ("fr"));


    chooseThemeLabel = new QLabel (tr("Choose UI theme :"));

    themeSelecter = new QComboBox;
    themeSelecter->addItem (tr("Classic Light"));
    themeSelecter->addItem (tr("Fusion Light"));
    themeSelecter->addItem (tr("Fusion Dark"));

    UIOptionsBoxLayout->addWidget (chooseLanguageLabel, 0, 0, Qt::AlignRight);
    UIOptionsBoxLayout->addWidget (languageSelecter, 0, 1);
    UIOptionsBoxLayout->addWidget (chooseThemeLabel, 1, 0, Qt::AlignRight);
    UIOptionsBoxLayout->addWidget (themeSelecter, 1, 1);


    othersTabLayout->addWidget (aboutLabel, 0, 0, 1, 2);
    othersTabLayout->addWidget (UIOptionsBox, 1, 0);
}


////////////// Slots


void MRecorder::languageChanged ()
{
    if (QMessageBox::question (this, tr("Language changed"), tr("You need to reload the application to apply changes.\nDo you want to restart now ?\nSome of your settings will be reseted.")) == QMessageBox::Yes)
    {
        qApp->quit ();
        QProcess::startDetached (qApp->arguments ()[0], qApp->arguments ());
    }
}
