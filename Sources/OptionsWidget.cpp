#include <QProcess>
#include <QStyleFactory>

#include <fstream>
#include <QFile>

#include "OptionsWidget.h"


////////////// Initialize widget


OptionsWidget::OptionsWidget () : QWidget ()
{
    layout = new QGridLayout (this);
    initPalettes ();


    aboutLabel = new QLabel (tr(
        "This opensource software was developed by NY4N_M4THS (MemeTech INC)<br/>"
        "under <a href = 'https://gnu.org/licenses/lgpl-3.0.en.html'>LGPL 3.0</a> with the framework <a href = 'https://qt.io'>Qt</a> and <a href = 'https://sfml-dev.org'>SFML</a> in C++ language.<br/>"
        "You can also visit <a href = 'https://memetech-inc.weebly.com'>our website</a> to check for updates,<br/>"
        "try other of our applications or <a href = 'https://github.com/NyanMaths/MRecorder/issues'>ask for new features</a> !<br/>"
        "Click <a href = 'https://github.com/NyanMaths/MRecorder'>here</a> to visit the GitHub page of the project."));

    aboutLabel->setOpenExternalLinks (true);
    aboutLabel->setTextInteractionFlags (Qt::TextBrowserInteraction);
    aboutLabel->setTextFormat (Qt::RichText);


    UIOptionsBox = new QGroupBox (tr("UI options"));
    UIOptionsBoxLayout = new QGridLayout (UIOptionsBox);
    UIOptionsBoxLayout->setAlignment (Qt::AlignLeft);

    chooseLanguageLabel = new QLabel (tr("Display language :"));

    languageSelecter = new QComboBox;
    languageSelecter->addItem ("English", QVariant ("en"));
    languageSelecter->addItem ("Français", QVariant ("fr"));


    chooseThemeLabel = new QLabel (tr("Interface theme :"));

    themeSelecter = new QComboBox;
    themeSelecter->addItem (tr("Classic light"));
    themeSelecter->addItem (tr("Fusion light"));
    themeSelecter->addItem (tr("Fusion dark"));
    connect (themeSelecter, SIGNAL (currentIndexChanged (int)), this, SLOT (changeTheme (int)));


    UIOptionsBoxLayout->addWidget (chooseLanguageLabel, 0, 0);
    UIOptionsBoxLayout->addWidget (languageSelecter, 0, 1);
    UIOptionsBoxLayout->addWidget (chooseThemeLabel, 1, 0);
    UIOptionsBoxLayout->addWidget (themeSelecter, 1, 1);


    NY4N_M4THS = new QLabel;
    NY4N_M4THS->setPixmap (QPixmap ("NY4N_M4THS.png"));


    layout->addWidget (aboutLabel, 0, 0, 1, 3);
    layout->addWidget (NY4N_M4THS, 0, 3, Qt::AlignRight);
    layout->addWidget (UIOptionsBox, 1, 0, 1, 2);

    loadOptions ();
}


void OptionsWidget::loadOptions ()
{
    QStringList settings {"en", "English", "0"};

    QFile settingsFile ("UI Options.pastouche");

    if (settingsFile.open (QIODevice::ReadOnly | QIODevice::Text))
        settings = QString (settingsFile.readAll ()).split ("\n");


    languageSelecter->setCurrentText (settings.at (1));
    themeSelecter->setCurrentIndex (settings.at (2).toUShort ());


    connect (languageSelecter, SIGNAL (currentIndexChanged (int)), this, SLOT (promptToRestart ()));
}


OptionsWidget::~OptionsWidget ()
{
    std::ofstream settingsFile ("UI Options.pastouche");

    if (settingsFile)
    {
        settingsFile<<languageSelecter->currentData ().toString ().toStdString ()<<"\n"
                    <<languageSelecter->currentText ().toStdString ()<<"\n"
                    <<themeSelecter->currentIndex ();
    }
}


////////////// Slots


void OptionsWidget::changeTheme (int themeIndex)
{
    if (themeIndex == 0)
    {
        qApp->setStyle (QStyleFactory::create ("WindowsVista"));
        qApp->setPalette (lightPalette);
    }
    else if (themeIndex == 1)
    {
        qApp->setStyle (QStyleFactory::create ("Fusion"));
        qApp->setPalette (lightPalette);
    }
    else
    {
        qApp->setStyle (QStyleFactory::create ("Fusion"));
        qApp->setPalette (darkPalette);
    }

    qApp->setFont (QFont ("Ubuntu", 12));
}

void OptionsWidget::promptToRestart ()
{
    if (QMessageBox::question (this, tr("Language changed"), tr("You need to reload the application to apply changes.\nDo you want to restart now ?")) == QMessageBox::Yes)
    {
        qApp->quit ();
        QProcess::startDetached (qApp->arguments ()[0], qApp->arguments ());
    }
}


////////////// Others


void OptionsWidget::initPalettes ()
{
    lightPalette = qApp->palette ();


    darkPalette.setColor (QPalette::Window, QColor (53,53,53));
    darkPalette.setColor (QPalette::WindowText, Qt::white);
    darkPalette.setColor (QPalette::Disabled, QPalette::WindowText, QColor (127,127,127));
    darkPalette.setColor (QPalette::Base, QColor (42,42,42));
    darkPalette.setColor (QPalette::AlternateBase, QColor (66,66,66));
    darkPalette.setColor (QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor (QPalette::ToolTipText, Qt::white);
    darkPalette.setColor (QPalette::Text, Qt::white);
    darkPalette.setColor (QPalette::Disabled, QPalette::Text, QColor (127,127,127));
    darkPalette.setColor (QPalette::Dark, QColor (35,35,35));
    darkPalette.setColor (QPalette::Shadow, QColor (20,20,20));
    darkPalette.setColor (QPalette::Button, QColor (53,53,53));
    darkPalette.setColor (QPalette::ButtonText, Qt::white);
    darkPalette.setColor (QPalette::Disabled, QPalette::ButtonText, QColor (127,127,127));
    darkPalette.setColor (QPalette::BrightText, Qt::red);
    darkPalette.setColor (QPalette::Link, QColor (42,130,218));
    darkPalette.setColor (QPalette::Highlight, QColor (42,130,218));
    darkPalette.setColor (QPalette::Disabled, QPalette::Highlight, QColor (80,80,80));
    darkPalette.setColor (QPalette::HighlightedText, Qt::white);
    darkPalette.setColor (QPalette::Disabled, QPalette::HighlightedText, QColor (127,127,127));
}

