#include <QTranslator>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "Application.hpp"


Application::Application () : QTabWidget ()
{
    readConfig();

    qApp->setFont(QFont("Ubuntu"));


    translator = new QTranslator;
    Q_UNUSED(translator->load("data/lang/asbear-" + config["locale"]));
    qApp->installTranslator(translator);

    messageBoxesTranslator = new QTranslator;
    Q_UNUSED(messageBoxesTranslator->load("data/lang/qt-" + config["locale"]));
    qApp->installTranslator(messageBoxesTranslator);


    setWindowTitle("Another Simple But Efficient Audio Recorder");
    setWindowIcon(QIcon("data/gui/logo.png"));

    options = new OptionsWidget(config);
    recordingsManager = new RecordingsManagerWidget(this);
    converter = new ConverterWidget(recordingsManager, config);
    recorder = new RecorderWidget(this, recordingsManager, config);

    recordingsManager->setConverter(converter);

    addTab(recorder, tr("Audio recorder"));
    addTab(recordingsManager, tr("My recordings"));
    addTab(converter, tr("File converter"));
    addTab(options, tr("About and settings"));

    setTabIcon(0, QIcon("data/gui/recorder-image.png"));
    setTabIcon(1, QIcon("data/gui/start-button.png"));
    setTabIcon(2, QIcon("data/gui/convert.png"));
    setTabIcon(3, QIcon("data/gui/settings.png"));


    setFixedSize(size());
    showMaximized();
}

Application::~Application ()
{
    saveConfig();
}

/*
The app configuration is stored in the standard path for user configuration,
typically ~/.config over Linux and C:/Users/username/AppData/Local over Windows,
...
And I do not have to answer to mac OS
*/
void Application::readConfig ()
{
    QFile configFile(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/asbear/general.conf");

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (!QFile::exists(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/asbear"))
            QDir(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0)).mkdir("asbear");

        configFile.setFileName("data/default-config.getout");

        if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
            QMessageBox::critical(nullptr, tr("Not my bad !"), tr("My installation seems to be broken, you may not give me the file access permission."));
    }

    QStringList fileContent(QString(configFile.readAll()).split("\n"));

    for (ushort i = 0 ; i + 1 < fileContent.length() ; i += 2)
        config[fileContent.at(i)] = fileContent.at(i + 1);
}

void Application::saveConfig ()
{
    recorder->getConfig(config);
    converter->getConfig(config);
    options->getConfig(config);

    QFile configFile(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/asbear/general.conf");

    if (configFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream fileStream(&configFile);

        QStringList configKeys(config.keys());
        for (ushort i = 0 ; i != configKeys.length() ; i++)
            fileStream<<configKeys.at(i)<<"\n"<<config[configKeys.at(i)]<<(i + 1 == configKeys.length() ? "" : "\n");
    }
}


void Application::closeEvent (QCloseEvent* event)
{
    if (recorder->beforeExit())
    {
        if (converter->converting())
        {
            if (QMessageBox::question(this, tr("Hep hep hep !"), tr("You are still converting some files, if you abort the operation, you could damage them and even your installation.\nDo you really want to forcefully quit ?")) != QMessageBox::Yes)
            {
                event->ignore();
                return;
            }
        }
        event->accept();
    }
    else
        event->ignore();
}
