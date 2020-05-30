#include <fstream>
#include <windows.h>

#include <QFile>
#include <QFileInfo>
#include <QDateTime>

#include <QMessageBox>

#include <SFML/Audio.hpp>

#include "RecordingsManagerWidget.h"


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
    layout->addWidget (bRemoveFromList, 3, 1);
    layout->addWidget (bClearRecordingsList, 4, 1);
    layout->addWidget (bDeleteRecording, 5, 1);
    layout->addWidget (bRemoveAllRecordings, 6, 1);
    layout->addWidget (recordingsList, 0, 0, 7, 1);
}

void RecordingsManagerWidget::initActions ()
{
    bPlayRecording = new QPushButton (tr("&Play"));
    bProperties = new QPushButton (tr("Pr&operties"));
    bShowInExplorer = new QPushButton (tr("&Show in explorer"));
    bRemoveFromList = new QPushButton (tr("&Remove from list"));
    bClearRecordingsList = new QPushButton (tr("&Clear list"));
    bDeleteRecording = new QPushButton (tr("&Delete"));
    bRemoveAllRecordings = new QPushButton (tr("Delete &all"));

    bPlayRecording->setEnabled (false);
    bProperties->setEnabled (false);
    bShowInExplorer->setEnabled (false);
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
    connect (bRemoveFromList, SIGNAL (clicked ()), this, SLOT (removeFromList ()));
    connect (bClearRecordingsList, SIGNAL (clicked ()), this, SLOT (clearRecordingsList ()));
    connect (bDeleteRecording, SIGNAL (clicked ()), this, SLOT (deleteRecording ()));
    connect (bRemoveAllRecordings, SIGNAL (clicked ()), this, SLOT (deleteAllRecordings ()));

    connect (recordingsList, SIGNAL (itemDoubleClicked (QListWidgetItem*)), this, SLOT (playRecording ()));
    connect (recordingsList, SIGNAL (itemClicked (QListWidgetItem*)), this, SLOT (recordingClicked ()));
}


RecordingsManagerWidget::~RecordingsManagerWidget ()
{
    std::ofstream recordingsFile ("Recordings.pastouche");

    for (int i = 0 ; i != recordingsList->count () ; i++)
        recordingsFile<<recordingsList->item (i)->text ().toStdString ()<<"\n";
}


////////////// Slots


void RecordingsManagerWidget::recordingClicked ()
{
    bPlayRecording->setEnabled (true);
    bProperties->setEnabled (true);
    bShowInExplorer->setEnabled (true);
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
        QMessageBox::critical (this, tr("Ooooops..."), tr("Impossible to load this file,\nit must be corrupted or deleted !"));

    else
    {
        QFile file (fileName);

        QString properties (tr("Name : ") + fileName +
                            tr("\nRecorded on : ") + QFileInfo (file).birthTime ().date ().toString (tr("MM/dd/yyyy")) +
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
