#include <QFile>
#include <fstream>

#include <QDropEvent>
#include <QMimeData>

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

#include "ConverterWidget.h"


////////////// Initialize widget


ConverterWidget::ConverterWidget (RecordingsManagerWidget* fileManagerTab) : QWidget ()
{
    fileManager = fileManagerTab;

    setAcceptDrops (true);

    converter = new Converter;
    connect (converter, SIGNAL (duplicateFile (QString&, bool&)), this, SLOT (askForNewFileName (QString&, bool&)), Qt::BlockingQueuedConnection);
    connect (converter, SIGNAL (progress (unsigned short int)), this, SLOT (updateProgressBar (unsigned short int)), Qt::BlockingQueuedConnection);
    connect (converter, SIGNAL (finishedConverting (const QStringList&)), this, SLOT (reactivateUI (const QStringList&)));
    connect (converter, SIGNAL (nextFileReached (const QString&)), this, SLOT (updateCurrentFileLabel (const QString&)));

    layout = new QVBoxLayout (this);


    helpLabel = new QLabel (tr("You can easily convert your audio files thanks to this tool.\nSelect the files to convert or just drag and drop them to the window !"));
    helpLabel->setAlignment (Qt::AlignCenter);


    initFilesWidget ();

    initOptionsBox ();


    bStart = new QPushButton (tr("&Start"));
    connect (bStart, SIGNAL (clicked ()), this, SLOT (start ()));

    currentFileLabel = new QLabel;
    currentFileLabel->setAlignment (Qt::AlignCenter);
    currentFileLabel->hide ();

    progressBar = new QProgressBar;
    progressBar->setRange (0, 100);
    progressBar->hide ();


    layout->addWidget (helpLabel);

    layout->addWidget (filesWidget);

    layout->addWidget (optionsBox);

    layout->addWidget (bStart);
    layout->addWidget (currentFileLabel);
    layout->addWidget (progressBar);


    loadOptions ();
    updateUI ();
}

void ConverterWidget::initFilesWidget ()
{
    filesWidget = new QWidget;
    filesWidgetLayout = new QGridLayout (filesWidget);


    filesList = new QListWidget;
    filesList->setSortingEnabled (true);
    connect (filesList, SIGNAL (itemClicked (QListWidgetItem*)), this, SLOT (onFileClicked ()));

    bAddFiles = new QPushButton (tr("&Add files"));
    connect (bAddFiles, SIGNAL (clicked ()), this, SLOT (addFiles ()));

    bRemoveFile = new QPushButton (tr("&Remove"));
    connect (bRemoveFile, SIGNAL (clicked ()), this, SLOT (removeFile ()));

    bClear = new QPushButton (tr("&Clear list"));
    connect (bClear, SIGNAL (clicked ()), this, SLOT (clear ()));


    filesWidgetLayout->addWidget (filesList, 0, 0, 3, 1);
    filesWidgetLayout->addWidget (bAddFiles, 0, 1);
    filesWidgetLayout->addWidget (bRemoveFile, 1, 1);
    filesWidgetLayout->addWidget (bClear, 2, 1);
}

void ConverterWidget::initOptionsBox ()
{
    optionsBox = new QGroupBox (tr("Options"));
    optionsBoxLayout = new QGridLayout (optionsBox);
    optionsBoxLayout->setAlignment (Qt::AlignLeft);


    chooseCodecLabel = new QLabel (tr("Destination codec :"));
    codecSelecter = new QComboBox;
    codecSelecter->addItem (tr("Vorbis (OGG) : compressed, good quality"), QVariant ("ogg"));
    codecSelecter->addItem (tr("FLAC : compressed, best quality"), QVariant ("flac"));
    codecSelecter->addItem (tr("PCM (WAV) : not compressed, best quality"), QVariant ("wav"));

    chooseSpeedLabel = new QLabel (tr("Conversion speed :"));
    speedSelecter = new QSpinBox;
    speedSelecter->setMinimum (100);
    speedSelecter->setMaximum (10000);

    bResetSettings = new QPushButton (tr("Reset conversion settings"));
    connect (bResetSettings, SIGNAL (clicked ()), this, SLOT (resetSettings ()));


    optionsBoxLayout->addWidget (chooseCodecLabel, 0, 0);
    optionsBoxLayout->addWidget (codecSelecter, 0, 1);
    optionsBoxLayout->addWidget (chooseSpeedLabel, 1, 0);
    optionsBoxLayout->addWidget (speedSelecter, 1, 1);
    optionsBoxLayout->addWidget (bResetSettings, 2, 0);
}


void ConverterWidget::loadOptions ()
{
    QStringList settings = {"0", "1000"};


    QFile settingsFile ("Converter Options.pastouche");

    if (settingsFile.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList readSettings = QString (settingsFile.readAll ()).split ("\n");

        if (readSettings.length () == settings.length ())
            settings = readSettings;
    }

    codecSelecter->setCurrentIndex (settings.at (0).toUShort ());
    speedSelecter->setValue (settings.at (1).toUInt ());
}

ConverterWidget::~ConverterWidget ()
{
    std::ofstream settingsFile ("Converter Options.pastouche");

    if (settingsFile)
        settingsFile<<codecSelecter->currentIndex ()<<"\n"
                    <<speedSelecter->value ();
}


////////////// Slots


void ConverterWidget::resetSettings ()
{
    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to go back\nto recommanded settings ?")) == QMessageBox::Yes)
    {
        codecSelecter->setCurrentIndex (0);
        speedSelecter->setValue (1000);
    }
}


void ConverterWidget::onFileClicked ()
{
    bRemoveFile->setEnabled (true);
}


void ConverterWidget::addFile (const QString& newFile)
{
    if (filesList->findItems (newFile, Qt::MatchExactly).length () == 0)
        filesList->addItem (newFile);

    updateUI ();
}

void ConverterWidget::addFiles ()
{
    QStringList files = QFileDialog::getOpenFileNames (this, tr("Add files to conversion list"), "", tr("Audio files (*.ogg *.flac *.wav)"), nullptr, QFileDialog::DontUseNativeDialog);

    for (short int i = 0 ; i != files.length () ; i++)
    {
        if (filesList->findItems (files.at (i), Qt::MatchExactly).length () == 0)
            filesList->addItem (files.at (i));
    }

    updateUI ();
}

void ConverterWidget::removeFile ()
{
    filesList->takeItem (filesList->currentRow ());

    updateUI ();
}

void ConverterWidget::clear ()
{
    filesList->clear ();

    updateUI ();
}


void ConverterWidget::start ()
{
    progressBar->setValue (0);


    QStringList files;

    for (unsigned short int i = 0 ; i != filesList->count () ; i++)
        files += filesList->item (i)->text ();


    QStringList outputFiles (files);
    bool ok = true;

    for (unsigned short int i = 0 ; i != files.count () ; i++)
    {
        outputFiles[i] = QFileInfo (files.at (i)).dir ().path () + "/" +
                         QFileInfo (files.at (i)).baseName () + " -conv." + codecSelecter->currentData ().toString ();

        while (QFile::exists (outputFiles.at (i)))
        {
            outputFiles[i] = QFileInfo (files.at (i)).dir ().path () + "/" +
                             QInputDialog::getText (this, tr("Confirmation"), outputFiles.at (i) + tr("\nalready exists, could you give it another name ?"), QLineEdit::Normal, QFileInfo (outputFiles.at (i)).baseName (), &ok) +
                             "." + codecSelecter->currentData ().toString ();

            if (!ok)
            {
                outputFiles[i] = "";
                files[i] = "";
            }
        }
    }
    outputFiles.removeAll ("");
    files.removeAll ("");


    if (files.length () > 0)
    {
        progressBar->show ();
        currentFileLabel->show ();
        setOptionsEnabled (false);

        converter->convert (files, outputFiles, speedSelecter->value ());
    }
}


void ConverterWidget::dragEnterEvent (QDragEnterEvent* event)
{
    if (event->mimeData ()->hasUrls ())
        event->acceptProposedAction ();

    else
        event->ignore ();
}

void ConverterWidget::dropEvent (QDropEvent* event)
{
    if (event->mimeData ()->hasUrls ())
    {
        QList<QUrl> droppedFiles (event->mimeData ()->urls ());
        QString currentFile;

        for (short int i = 0 ; i != droppedFiles.length () ; i++)
        {
            currentFile = droppedFiles.at (i).toString ().remove ("file:///");

            if (QStringList ({"ogg", "flac", "wav"}).contains (QFileInfo (currentFile).suffix ().toLower ()))
                addFile (currentFile);

            else
                QMessageBox::warning (this, tr("Error"), tr("Impossible to import ") + currentFile + tr(",\nyou can only import OGG, FLAC and WAV files !"));
        }

        updateUI ();
        event->acceptProposedAction ();
    }
    else
        event->ignore ();
}


void ConverterWidget::updateUI ()
{
    if (filesList->count () == 0)
    {
        bRemoveFile->setEnabled (false);
        bClear->setEnabled (false);

        bStart->setEnabled (false);
    }
    else
    {
        bClear->setEnabled (true);

        bStart->setEnabled (true);
    }
}

void ConverterWidget::setOptionsEnabled (bool state)
{
    helpLabel->setEnabled (state);

    filesList->setEnabled (state);
    bAddFiles->setEnabled (state);
    bRemoveFile->setEnabled (state);
    bClear->setEnabled (state);

    optionsBox->setEnabled (state);

    bStart->setEnabled (state);

    if (filesList->currentItem () == nullptr)
        bRemoveFile->setEnabled (false);
}

void ConverterWidget::reactivateUI (const QStringList& outputFiles)
{
    if (QMessageBox::question (this, tr("Conversion's finished !"), tr("All your files are converted !\nDo you want to add them to your recordings ?")) == QMessageBox::Yes)
        for (short int i = 0 ; i != outputFiles.length () ; i++)
            fileManager->addRecording (outputFiles.at (i));

    setOptionsEnabled (true);

    progressBar->hide ();
    currentFileLabel->hide ();
}


////////////// Conversion slots


void ConverterWidget::updateProgressBar (unsigned short int progression)
{
    progressBar->setValue (progression);
}

void ConverterWidget::updateCurrentFileLabel (const QString& currentFile)
{
    currentFileLabel->setText (tr("Processing file ") + currentFile);
}

