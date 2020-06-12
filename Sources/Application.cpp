#include <QFontDatabase>
#include <QTranslator>
#include <QScreen>

#include <fstream>

#include "Application.h"


Application::Application () : QTabWidget ()
{
    QFontDatabase::addApplicationFont ("Ubuntu.ttf");
    qApp->setFont (QFont ("Ubuntu", 12));


    std::string lang ("en");

    std::ifstream languageFile ("UI Options.pastouche");
    if (languageFile)
        getline (languageFile, lang);

    translator = new QTranslator;
    translator->load("mrecorder_" + QString::fromStdString (lang));
    qApp->installTranslator (translator);

    messageBoxesTranslator = new QTranslator;
    messageBoxesTranslator->load ("qtbase_" + QString::fromStdString (lang));
    qApp->installTranslator (messageBoxesTranslator);


    setWindowTitle (tr("MRecorder - Home"));
    setWindowIcon (QIcon ("Window Icon.png"));


    optionsTab = new OptionsWidget;
    recordingsTab = new RecordingsManagerWidget (this);
    converterTab = new ConverterWidget (recordingsTab);
    recorderTab = new RecorderWidget (this, recordingsTab);

    recordingsTab->setConverter (converterTab);

    addTab (recorderTab, tr("Audio recorder"));
    addTab (recordingsTab, tr("Your recordings"));
    addTab (converterTab, tr("File converter"));
    addTab (optionsTab, tr("About and settings"));


    show ();

    QSize screenSize = QGuiApplication::screens ().at (0)->size ();
    move (screenSize.width () / 2 - width () / 2, screenSize.height () / 2 - height () / 2);

    setFixedSize (size ());
}


void Application::closeEvent (QCloseEvent* event)
{
    recorderTab->beforeExit (event);
}
