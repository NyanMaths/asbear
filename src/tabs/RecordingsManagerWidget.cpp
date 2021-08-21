#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QMimeData>

#include <QDateTime>

#include <QDesktopServices>

#include <QDropEvent>

#include "tabs/ConverterWidget.hpp"
#include "tabs/RecordingsManagerWidget.hpp"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

#include <SFML/Audio.hpp>


////////////// Initialize widget


RecordingsManagerWidget::RecordingsManagerWidget (QTabWidget* parent) : QWidget ()
{
    mainWindow = parent;

    setAcceptDrops(true);

    layout = new QGridLayout(this);
    layout->setAlignment(Qt::AlignBottom);
    layout->setSpacing(20);

    helpLabel = new QLabel(tr(
        "This is your recording manager : all your recordings are listed here !\n"
        "You can easily play, delete, move them from this tab\n"
        "and add new recordings by drag and drop from your file explorer or using button \"Add recordings\"."));
    helpLabel->setAlignment(Qt::AlignCenter);

    recordingsList = new QListWidget;
    recordingsList->setSortingEnabled(true);


    QFile recordingsFile(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/ASBEAR/recordings.getout");

    if (recordingsFile.open(QFile::ReadOnly | QFile::Text))
    {
        QStringList filesList = QString(recordingsFile.readAll()).split("\n");

        for (ushort i = 0 ; i != filesList.length() ; i++)
            if (QFile::exists(filesList.at(i)))
                recordingsList->addItem(filesList.at(i));
    }

    recording = new RecordingPlayer(this);


    initActions();
    initPlaybackTools();


    layout->addWidget(helpLabel, 0, 0, 1, 2);
    layout->addWidget(recordingsList, 1, 0, 10, 1);

    layout->addWidget(bAddRecordings, 1, 1);
    layout->addWidget(bProperties, 2, 1);
    layout->addWidget(bShowInExplorer, 3, 1);
    layout->addWidget(bMove, 4, 1);
    layout->addWidget(bRename, 5, 1);
    layout->addWidget(bConvert, 6, 1);
    layout->addWidget(bRemoveFromList, 7, 1);
    layout->addWidget(bRemove, 8, 1);
    layout->addWidget(bRemoveAllFromList, 9, 1);
    layout->addWidget(bRemoveAll, 10, 1);


    layout->addWidget(playbackTools, 11, 0, 1, 2);


    // Impossible to emit in constructor, so...
    updateActions();
    updatePlaybackControls();
}

void RecordingsManagerWidget::initActions ()
{
    bAddRecordings = new QPushButton(tr("Add recordings"));
    bProperties = new QPushButton(tr("Properties"));
    bShowInExplorer = new QPushButton(tr("Open folder"));
    bMove = new QPushButton(tr("Move to..."));
    bRename = new QPushButton(tr("Rename"));
    bConvert = new QPushButton(tr("Convert"));
    bRemoveFromList = new QPushButton(tr("Remove from list"));
    bRemove = new QPushButton(tr("Delete"));
    bRemoveAllFromList = new QPushButton(tr("Clear list"));
    bRemoveAll = new QPushButton(tr("Delete all"));

    connect(bAddRecordings, SIGNAL(clicked()), this, SLOT(addRecordings()));
    connect(bProperties, SIGNAL(clicked()), this, SLOT(displayProperties()));
    connect(bShowInExplorer, SIGNAL(clicked()), this, SLOT(showInExplorer()));
    connect(bMove, SIGNAL(clicked()), this, SLOT(move()));
    connect(bRename, SIGNAL(clicked()), this, SLOT(rename()));
    connect(bConvert, SIGNAL(clicked()), this, SLOT(convert()));
    connect(bRemoveFromList, SIGNAL(clicked()), this, SLOT(removeFromList()));
    connect(bRemove, SIGNAL(clicked()), this, SLOT(remove()));
    connect(bRemoveAllFromList, SIGNAL(clicked()), this, SLOT(removeAllFromList()));
    connect(bRemoveAll, SIGNAL(clicked()), this, SLOT(removeAll()));

    connect(recordingsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(play()));
    connect(recordingsList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(loadRecording(const QString&)));
    connect(this, SIGNAL(modifiedList()), this, SLOT(updateActions()));
    connect(this, SIGNAL(modifiedPlayingStatus()), this, SLOT(updatePlaybackControls()));
    connect(recording, SIGNAL(progression(int)), this, SLOT(updateSlider(int)));
    connect(recording, SIGNAL(stopped()), this, SLOT(updatePlaybackControls()));
}


void RecordingsManagerWidget::initPlaybackTools ()
{
    playbackTools = new QWidget;
    playbackLayout = new QHBoxLayout(playbackTools);


    bPlay = new QPushButton(QIcon("data/gui/start-button.png"), "");
    bPause = new QPushButton(QIcon("data/gui/pause-button.png"), "");
    bStop = new QPushButton(QIcon("data/gui/stop-button.png"), "");

    bPlay->setToolTip(tr("Play"));
    bPause->setToolTip(tr("Pause"));
    bStop->setToolTip(tr("Stop"));

    connect(bPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(bPause, SIGNAL(clicked()), this, SLOT(pause()));
    connect(bStop, SIGNAL(clicked()), this, SLOT(stop()));
    bPlay->setShortcut(QKeySequence(Qt::Key_Space));
    bPause->setShortcut(QKeySequence(Qt::Key_Space));


    playbackBar = new DirectJumpSlider;
    connect(playbackBar, SIGNAL(sliderPressed()), this, SLOT(pressedSlider()));
    connect(playbackBar, SIGNAL(directJumpOperated()), this, SLOT(changeOffset()));
    connect(playbackBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(playbackBar, SIGNAL(sliderReleased()), this, SLOT(releasedSlider()));

    playbackTimerLabel = new QLabel("0:00");
    recordingDurationLabel = new QLabel("0:00");


    playbackLayout->addWidget(bPlay);
    playbackLayout->addWidget(bPause);
    playbackLayout->addWidget(bStop);
    playbackLayout->addWidget(playbackTimerLabel);
    playbackLayout->addWidget(playbackBar);
    playbackLayout->addWidget(recordingDurationLabel);
}


void RecordingsManagerWidget::setConverter (ConverterWidget* newConverter)
{
    converter = newConverter;
}


RecordingsManagerWidget::~RecordingsManagerWidget ()
{
    QFile recordingsFile(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/ASBEAR/recordings.getout");

    if (recordingsFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream stream(&recordingsFile);

        for (int i = 0 ; i != recordingsList->count() ; i++)
            stream<<recordingsList->item(i)->text()<<"\n";
    }
}


////////////// UI update slots


void RecordingsManagerWidget::loadRecording (const QString& currentFileName)
{
    recording->stop();
    playbackBar->setValue(0);


    if (recording->setFile(currentFileName))
    {
        playbackBar->setRange (0, recording->duration());

        uint minutes(recording->duration() / 1000 / 60);
        uint seconds(recording->duration() / 1000 % 60);

        recordingDurationLabel->setText(QString::number(minutes) + ":" + (seconds < 10 ? "0" : "") + QString::number(seconds));
    }
    else if (QMessageBox::question(this, tr("Missing file"), tr("It seems that I can not access to this file,\ndo you want to try to delete it ?")) == QMessageBox::Yes)
        remove(false);

    emit modifiedList();
}

void RecordingsManagerWidget::updateActions ()
{
    // Removes recordings that don't exist anymore
    for (ushort i(0) ; i != recordingsList->count() ; i++)
    {
        if (!QFile::exists(recordingsList->item(i)->text()))
        {
            recordingsList->takeItem(i);
            i--;
        }
    }

    if (recordingsList->currentItem() == nullptr)
    {
        bProperties->setEnabled(false);
        bShowInExplorer->setEnabled(false);
        bMove->setEnabled(false);
        bRename->setEnabled(false);
        bConvert->setEnabled(false);
        bRemoveFromList->setEnabled(false);
        bRemove->setEnabled(false);

        if (recordingsList->count() == 0)
        {
            bRemoveAllFromList->setEnabled(false);
            bRemoveAll->setEnabled(false);
        }
    }
    else
    {
        bProperties->setEnabled(true);
        bShowInExplorer->setEnabled(true);
        bMove->setEnabled(true);
        bRename->setEnabled(true);
        bConvert->setEnabled(true);
        bRemoveFromList->setEnabled(true);
        bRemove->setEnabled(true);
        bRemoveAllFromList->setEnabled(true);
        bRemoveAll->setEnabled(true);
    }
}

void RecordingsManagerWidget::updatePlaybackControls ()
{
    if (recordingsList->currentItem() == nullptr)
    {
        bPlay->setEnabled(false);
        bPause->setEnabled(false);
        bStop->setEnabled(false);
        playbackBar->setEnabled(false);
        playbackTimerLabel->setEnabled(false);
        recordingDurationLabel->setEnabled(false);
    }
    else
    {
        playbackBar->setEnabled(true);
        playbackTimerLabel->setEnabled(true);
        recordingDurationLabel->setEnabled(true);

        if (recording->status() == sf::SoundSource::Playing)
        {
            bPlay->setEnabled(false);
            bPause->setEnabled(true);
            bStop->setEnabled(true);
        }
        else if (recording->status() == sf::SoundSource::Paused)
        {
            bPlay->setEnabled(true);
            bPause->setEnabled(false);
            bStop->setEnabled(true);
        }
        else
        {
            bPlay->setEnabled(true);
            bPause->setEnabled(false);
            bStop->setEnabled(false);

            updateSlider(0);
        }
    }
}

void RecordingsManagerWidget::updateSlider (int value)
{
    playbackBar->setValue(value);
}


////////////// Playback slots


void RecordingsManagerWidget::pressedSlider ()
{
    if (recording->status() == sf::SoundSource::Playing)
    {
        statusBeforeOffsetJump = sf::SoundSource::Playing;

        recording->pause();
    }
    else
        statusBeforeOffsetJump = sf::SoundSource::Paused;
}

void RecordingsManagerWidget::sliderValueChanged (int value)
{
    uint minutes(value / 1000 / 60);
    uint seconds(value / 1000 % 60);

    playbackTimerLabel->setText(QString::number(minutes) + ":" + (seconds < 10 ? "0" : "") + QString::number(seconds));
}

void RecordingsManagerWidget::changeOffset ()
{
    recording->setOffset(playbackBar->value());
}

void RecordingsManagerWidget::releasedSlider ()
{
    if (statusBeforeOffsetJump == sf::SoundSource::Playing)
    {
        recording->play();
    }
}


void RecordingsManagerWidget::play ()
{
    recording->play();

    emit modifiedPlayingStatus();
}

void RecordingsManagerWidget::pause ()
{
    recording->pause();

    emit modifiedPlayingStatus();
}

void RecordingsManagerWidget::stop ()
{
    recording->stop();

    emit modifiedPlayingStatus();
}


//////////////  Actions


/*
Adds recordings to the list.
Triggers a file selection GUI if called without filename (typically with "Add recordings" button) or directly adds it if one is given.
Checks whether the recordings are already listed.
*/
void RecordingsManagerWidget::addRecordings (const QString& newRecording)
{
    QStringList newRecordings;

    if (newRecording.isEmpty())
        newRecordings = QFileDialog::getOpenFileNames(this, tr("Select files to add to your recordings"), "", tr("Audio files (*.ogg *.flac *.wav)"), nullptr, QFileDialog::DontUseNativeDialog);
    else
        newRecordings.append(newRecording);

    for (ushort i(0) ; i != newRecordings.length() ; i++)
    {
        bool notListedYet(true);
        for (ushort k(0) ; k != recordingsList->count() ; k++)
        {
            if (recordingsList->item(k)->text() == newRecordings.at(i))
            {
                notListedYet = false;
                break;
            }
        }
        if (notListedYet)
            recordingsList->addItem(newRecordings.at(i));
    }

    emit modifiedList();
}

/*
Too lazy to describe this...
*/
void RecordingsManagerWidget::displayProperties ()
{
    QString fileName(recordingsList->currentItem()->text());

    sf::Music music;
    if (!music.openFromFile(std::string(fileName.toLocal8Bit())))
    {
        if (QMessageBox::question(this, tr("Ooooops..."), tr("Impossible to load this file,\nit must be corrupted !\nDo you want to delete it ?")) == QMessageBox::Yes)
            QFile::remove (recordingsList->currentItem()->text());
    }
    else
    {
        QFile file(fileName);

        QString properties(tr("Name : ") + fileName +
                           tr("\nRecorded on : ") + QFileInfo(file).birthTime().date().toString(tr("MM/dd/yyyy")) +
                           tr("\n\nSample rate : ") + QString::number(music.getSampleRate()) + " Hz" +
                           tr("\nChannels : ") + QString::number(music.getChannelCount()) +
                           tr("\nSize : "));


        if (file.size() >= 1048576)
            properties += QString::number(file.size() / 1048576) + tr(" MB\nDuration : ");

        else
            properties += QString::number(file.size() / 1024) + tr(" KB\nDuration : ");


        uint minutes(music.getDuration().asSeconds() / 60);
        uint seconds(uint(music.getDuration().asSeconds()) % 60);

        if (!minutes)
            properties += seconds == 1 ? tr("1 second") : tr("%n seconds", "", seconds);

        else if (seconds)
            properties += (minutes == 1 ? tr("1 minute and ") : tr("%n minutes and ", "", minutes)) +
                          (seconds == 1 ? tr("1 second") : tr("%n seconds", "", seconds));

        else
            properties += minutes == 1 ? tr("1 minute") : tr("%n minutes", "", minutes);

        // All done, it is time to display the properties
        QMessageBox::information(this, tr("File properties"), properties);
    }
}

/*
Just opens the containing folder of the selected file.
*/
void RecordingsManagerWidget::showInExplorer ()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(recordingsList->currentItem()->text()).dir().path()));
}

/*
Allows the user to move the selected file.
*/
void RecordingsManagerWidget::move ()
{
    QString fileName = recordingsList->currentItem()->text();

    QString dest(QFileDialog::getExistingDirectory(this, tr("Select destination folder"), QFileInfo(fileName).dir().path(), QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly));

    QString newFileName(QString(dest + "/" + QFileInfo(fileName).fileName()).replace("//", "/"));


    if (dest.isEmpty () || newFileName == fileName)
        return;

    if (QFile::exists(newFileName))
    {
        if (QMessageBox::question(this, tr("Confirmation"), tr("A file with the same name already exists there,\ndo you want to replace it ?")) != QMessageBox::Yes)
            return;

        QFile::remove(newFileName);

        emit modifiedList(); // If it was a listed recording
    }

    if (!QFile::copy(fileName, newFileName))
    {
        QMessageBox::critical(this, tr("Error"), tr("Impossible to move the file for some reason,\nyou should know more than me about that, so good luck !"));
        return;
    }

    sf::SoundSource::Status status(recording->status());
    int offset(recording->offset());

    if (status != sf::SoundSource::Stopped)
        recording->stop(false);

    recordingsList->currentItem()->setText(newFileName);
    QFile::remove(fileName);

    recording->setFile(newFileName);
    recording->setOffset(offset);

    if (status == sf::SoundSource::Stopped)
        recording->stop();

    if (status == sf::SoundSource::Playing)
        recording->play();


    QMessageBox::information(this, tr("Operation successful !"), tr("File moved to\n") + dest);
}

/*
Allows the user to rename the selected file. Stops playing before and check whether there is no file with the same name.
*/
void RecordingsManagerWidget::rename ()
{
    QString fileName = recordingsList->currentItem()->text();


    bool ok;
    QString newFileName(QInputDialog::getText(this, tr("Rename file"), tr("Please input the new file name :"), QLineEdit::Normal, QFileInfo(fileName).baseName(), &ok).remove(QRegularExpression("^ +")).remove(QRegularExpression(" +$")));
    if (!ok)
        return;

    newFileName = QString(QFileInfo(fileName).dir().absolutePath() + "/" +
                          newFileName + "." + QFileInfo(fileName).suffix())
                          .replace("//", "/");  // Replace // by / for root folder

    // Useless to rename the file if it is the same name, logically...
    if (newFileName == fileName)
        return;


    sf::SoundSource::Status status(recording->status());
    int offset(recording->offset());

    if (status != sf::SoundSource::Stopped)
        recording->stop(false);


    bool successfullyRenamed(QFile::rename(fileName, newFileName));

    recording->setFile(newFileName);
    recording->setOffset(offset);

    if (status == sf::SoundSource::Stopped)
        recording->stop();

    if (status == sf::SoundSource::Playing)
        recording->play();

    if (successfullyRenamed)
    {
        recordingsList->currentItem()->setText(newFileName);

        QMessageBox::information(this, tr("Operation successful !"), tr("Your recording was renamed ") + QFileInfo(newFileName).baseName());
    }
    else
        QMessageBox::critical(this, tr("Error"), tr("Impossible to rename the file for some reason,\nyou should know more than me about that, so good luck !"));
}

/*
Adds the selected file to the conversion list and switches to the conversion tab.
*/
void RecordingsManagerWidget::convert ()
{
    converter->addFile(recordingsList->currentItem()->text());

    mainWindow->setCurrentIndex(2);
}


/*
Simply removes the currently selected recording from the list, no warns or verifications needed  :-)
*/
void RecordingsManagerWidget::removeFromList ()
{
    recordingsList->takeItem(recordingsList->currentRow());

    emit modifiedList();
}

/*
Prompts the user whether the selected file should be deleted, then stops and deletes it.
If prompt is set to false, the recording will be directly deleted (true by default).
If fileName is not specified (by default), the currently selected recording will be deleted.
*/
bool RecordingsManagerWidget::remove (bool prompt, QString fileName)
{
    if (prompt)
        if (QMessageBox::question(this, tr("Confirmation"), tr("Do you really want to permanently delete this file ?")) == QMessageBox::No)
            return false;

    if (fileName.isEmpty())
        fileName = recordingsList->currentItem()->text();


    if (recordingsList->currentItem()->text() == fileName)
        stop();

    if (!QFile::remove(fileName))
    {
        QMessageBox::critical(this, tr("Error"), tr("Impossible to delete the file for some reason,\nyou should know more than me about that, no my bad please do not beat me !"));
        return false;
    }

    removeFromList();
    emit modifiedList();

    return true;
}

/*
Warns the user before clearing the recordings list.
*/
void RecordingsManagerWidget::removeAllFromList ()
{
    if (QMessageBox::question(this, tr("Confirmation"), tr("Do you really want to clear the list ?\nThis won't remove your recordings from your PC.")) == QMessageBox::Yes)
    {
        stop();

        recordingsList->clear();
        emit modifiedList();
    }
}

/*
Prompts the user whether all files should be deleted, then stops the currently playing and deletes them all.
*/
void RecordingsManagerWidget::removeAll ()
{
    if (QMessageBox::question(this, tr("Confirmation"), tr("Do you really want to remove all your recordings ?\nThis action cannot be undone.")) == QMessageBox::Yes)
    {
        stop();

        for (ushort i = 0 ; i != recordingsList->count() ; i++)
        {
            if (!QFile::remove(recordingsList->item(i)->text()))
                QMessageBox::critical(this, tr("Error"), tr("Impossible to delete ") + recordingsList->item(i)->text());
        }

        emit modifiedList();
    }
}


////////////// Drag'n'drop actions


/*
Just checks if dragged items are files, the codec checkup is performed by dropEvent().
*/
void RecordingsManagerWidget::dragEnterEvent (QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();

    else
        event->ignore();
}

/*
Checks whether the containers/codecs are supported and then adds the files to the list or warns the user.
*/
void RecordingsManagerWidget::dropEvent (QDropEvent* event)
{
    QList<QUrl> droppedFiles(event->mimeData()->urls());
    QVector<QString> supportedContainers{"ogg", "flac", "wav"};
    QString currentFile;

    for (ushort i(0) ; i != droppedFiles.length() ; i++)
    {
        currentFile = droppedFiles.at(i).toLocalFile();

        if (supportedContainers.contains(QFileInfo(currentFile).suffix().toLower()))
            addRecordings(currentFile);

        else
            QMessageBox::warning(this, tr("Error"), tr("Impossible to import ") + currentFile + tr(",\nyou can only import OGG, FLAC and WAV files !"));
    }

    emit modifiedList();
    event->acceptProposedAction();
}
