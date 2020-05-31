#include <windows.h>
#include <fstream>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>

#include "RecordingsManagerWidget.h"

#include <QMessageBox>
#include <QFileDialog>

#include <SFML/Audio.hpp>


////////////// Initialize widget


RecordingsManagerWidget::RecordingsManagerWidget () : QWidget ()
{
    layout = new QGridLayout (this);


    recordingsList = new QListWidget;
    recordingsList->setSortingEnabled (true);

    std::ifstream recordingsFile ("Recordings.pastouche");
    if (recordingsFile)
    {
        std::string recordingPath;

        while (getline (recordingsFile, recordingPath))
            if (QFile::exists (QString::fromStdString (recordingPath)))
                recordingsList->addItem (QString::fromStdString (recordingPath));
    }

    connect (this, SIGNAL (modifiedList ()), this, SLOT (updateUI ()));


    initActions ();


    layout->addWidget (bPlayRecording, 0, 1);
    layout->addWidget (bProperties, 1, 1);
    layout->addWidget (bShowInExplorer, 2, 1);
    layout->addWidget (bMove, 3, 1);
    layout->addWidget (bRemoveFromList, 4, 1);
    layout->addWidget (bClearRecordingsList, 5, 1);
    layout->addWidget (bDeleteRecording, 6, 1);
    layout->addWidget (bRemoveAllRecordings, 7, 1);
    layout->addWidget (recordingsList, 0, 0, 8, 1);
}

void RecordingsManagerWidget::initActions ()
{
    bPlayRecording = new QPushButton (tr("&Play"));
    bProperties = new QPushButton (tr("Pr&operties"));
    bShowInExplorer = new QPushButton (tr("Op&en folder"));
    bMove = new QPushButton (tr("&Move to..."));
    bRemoveFromList = new QPushButton (tr("&Remove from list"));
    bClearRecordingsList = new QPushButton (tr("&Clear list"));
    bDeleteRecording = new QPushButton (tr("&Delete"));
    bRemoveAllRecordings = new QPushButton (tr("Delete &all"));

    bPlayRecording->setEnabled (false);
    bProperties->setEnabled (false);
    bShowInExplorer->setEnabled (false);
    bMove->setEnabled (false);
    bDeleteRecording->setEnabled (false);
    bRemoveFromList->setEnabled (false);

    if (recordingsList->count () == 0)
    {
        bClearRecordingsList->setEnabled (false);
        bRemoveAllRecordings->setEnabled (false);
    }

    connect (bPlayRecording, SIGNAL (clicked ()), this, SLOT (playRecording ()));
    connect (bProperties, SIGNAL (clicked ()), this, SLOT (displayProperties ()));
    connect (bShowInExplorer, SIGNAL (clicked ()), this, SLOT (showInExplorer ()));
    connect (bMove, SIGNAL (clicked ()), this, SLOT (move ()));
    connect (bRemoveFromList, SIGNAL (clicked ()), this, SLOT (removeFromList ()));
    connect (bClearRecordingsList, SIGNAL (clicked ()), this, SLOT (clearRecordingsList ()));
    connect (bDeleteRecording, SIGNAL (clicked ()), this, SLOT (deleteRecording ()));
    connect (bRemoveAllRecordings, SIGNAL (clicked ()), this, SLOT (deleteAllRecordings ()));

    connect (recordingsList, SIGNAL (itemDoubleClicked (QListWidgetItem*)), this, SLOT (playRecording ()));
    connect (recordingsList, SIGNAL (itemClicked (QListWidgetItem*)), this, SLOT (recordingClicked ()));
}


RecordingsManagerWidget::~RecordingsManagerWidget ()
{
    if (recordingsList->count () != 0)  // If the user recorded something
    {
        std::ofstream recordingsFile ("Recordings.pastouche");

        for (int i = 0 ; i != recordingsList->count () ; i++)
            recordingsFile<<recordingsList->item (i)->text ().toStdString ()<<"\n";
    }
}


////////////// Slots


void RecordingsManagerWidget::recordingClicked ()
{
    bPlayRecording->setEnabled (true);
    bProperties->setEnabled (true);
    bShowInExplorer->setEnabled (true);
    bMove->setEnabled (true);
    bDeleteRecording->setEnabled (true);
    bRemoveFromList->setEnabled (true);
}

void RecordingsManagerWidget::updateUI ()
{
    if (recordingsList->count () == 0)
    {
        bPlayRecording->setEnabled (false);
        bProperties->setEnabled (false);
        bShowInExplorer->setEnabled (false);
        bMove->setEnabled (false);
        bDeleteRecording->setEnabled (false);
        bRemoveFromList->setEnabled (false);

        bClearRecordingsList->setEnabled (false);
        bRemoveAllRecordings->setEnabled (false);
    }
    else
    {
        bClearRecordingsList->setEnabled (true);
        bRemoveAllRecordings->setEnabled (true);
    }
}


void RecordingsManagerWidget::playRecording ()
{
    ShellExecuteA (nullptr, "open", recordingsList->currentItem ()->text ().replace ("/", "\\").toLocal8Bit (), "", "", SW_SHOWDEFAULT);
}


void RecordingsManagerWidget::displayProperties ()
{
    QString fileName (recordingsList->currentItem ()->text ());

    sf::Music music;
    if (!music.openFromFile (fileName.toStdString ()))
    {
        if (QMessageBox::question (this, tr("Ooooops..."), tr("Impossible to load this file,\nit must be corrupted !\nDo you want to delete it ?")) == QMessageBox::Yes)
        {
            QFile::remove (recordingsList->currentItem ()->text ());
            removeCurrentFromList ();
        }
    }
    else
    {
        QFile file (fileName);

        QString properties (tr("Name : ") + fileName +
                            tr("\nRecorded on : ") + QFileInfo (file).birthTime ().date ().toString (tr("MM/dd/yyyy")) +
                            tr("\n\nSample rate : ") + QString::number (music.getSampleRate ()) + " Hz" +
                            tr("\nChannels : ") + QString::number (music.getChannelCount ()) +
                            tr("\nSize : "));


        if (file.size () >= 1048576)
            properties += QString::number (file.size () / 1048576) + tr(" MB\nDuration : ");

        else
            properties += QString::number (file.size () / 1024) + tr(" KB\nDuration : ");


        unsigned int minutes = int (music.getDuration ().asSeconds ()) / 60;
        unsigned int seconds = int (music.getDuration ().asSeconds ()) % 60;

        if (!minutes)
            properties += seconds == 1 ? tr("1 second") : tr("%n seconds", "", seconds);

        else if (seconds)
            properties += (minutes == 1 ? tr("1 minute and ") : tr("%n minutes and ", "", minutes)) +
                          (seconds == 1 ? tr("1 second") : tr("%n seconds", "", seconds));

        else
            properties += minutes == 1 ? tr("1 minute") : tr("%n minutes", "", minutes);

        QMessageBox::information (this, tr("File properties"), properties);
    }
}

void RecordingsManagerWidget::showInExplorer ()
{
    QString fileName (recordingsList->currentItem ()->text ());
    QString fileFolder;

    int lastSlashIndex = fileName.lastIndexOf ("/") + 1;

    for (int i = 0 ; i != lastSlashIndex ; i++)
        fileFolder += fileName.at (i);


    ShellExecuteA (nullptr, "explore", fileFolder.replace ("/", "\\").toUtf8 (), "", "", SW_SHOWDEFAULT);
}

void RecordingsManagerWidget::move ()
{
    QString fileName = recordingsList->currentItem ()->text ();

    QString dest = QFileDialog::getExistingDirectory (this, tr("Select destination folder"), QFileInfo (fileName).dir ().path());

    QString destFileName = dest + "/" + QFileInfo (fileName).fileName ();


    if (dest.isEmpty () || destFileName == fileName)
        return;

    if (QFile::exists (destFileName))
    {
        if (QMessageBox::question (this, tr("Confirmation"), tr("A file with the same name already exists there,\ndo you want to replace it ?")) != QMessageBox::Yes)
            return;

        QFile::remove (destFileName);
        removeCurrentFromList ();
    }


    QFile::copy (fileName, destFileName);

    QFile::remove (fileName);
    recordingsList->currentItem ()->setText (destFileName);


    QMessageBox::information (this, tr("Operation successful !"), tr("File ") + fileName + tr("\nmoved to ") + dest);
}


void RecordingsManagerWidget::removeFromList ()
{
    removeCurrentFromList ();
}

void RecordingsManagerWidget::clearRecordingsList ()
{
    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to clear the list ?\nThis won't remove your recordings.")) == QMessageBox::Yes)
    {
        recordingsList->clear ();

        updateUI ();
    }
}


void RecordingsManagerWidget::deleteRecording ()
{
    QString fileName (recordingsList->currentItem ()->text ());

    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to permanently delete\n") + fileName + " ?") == QMessageBox::Yes)
    {
        if (!QFile::remove (fileName))
            QMessageBox::critical (this, tr("Error"), tr("Impossible to delete this file,\nyou must already did it."));

        removeCurrentFromList ();
    }
}

void RecordingsManagerWidget::deleteAllRecordings ()
{
    if (QMessageBox::question (this, tr("Confirmation"), tr("Do you really want to remove all your recordings ?\nThis action cannot be undone.")) == QMessageBox::Yes)
    {
        recordingsList->setCurrentRow (0);

        while (recordingsList->count ())
        {
            if (!QFile::remove (recordingsList->currentItem ()->text ()))
                QMessageBox::critical (this, tr("Error"), tr("Impossible to delete ") + recordingsList->currentItem ()->text () + tr("\nYou must already did it."));

            recordingsList->takeItem (0);
        }

        updateUI ();
    }
}


////////////// Others


void RecordingsManagerWidget::addRecording (const QString& fileName)
{
    recordingsList->addItem (fileName);

    emit modifiedList ();
}

void RecordingsManagerWidget::removeCurrentFromList ()
{
    recordingsList->takeItem (recordingsList->currentRow ());

    emit modifiedList ();
}
