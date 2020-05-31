#include <QProcess>
#include <QStyleFactory>

#include <fstream>
#include <QFile>

#include "OptionsWidget.h"


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


    layout->addWidget (aboutLabel, 0, 0);
    layout->addWidget (NY4N_M4THS, 0, 1, Qt::AlignRight);
    layout->addWidget (UIOptionsBox, 1, 0);

    loadOptions ();
}


void OptionsWidget::loadOptions ()
{
    QFile settingsFile ("UI Options.pastouche");

    if (settingsFile.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList data = QString (settingsFile.readAll ()).split ("\n");

        languageSelecter->setCurrentText (data.at (1));
        themeSelecter->setCurrentIndex (data.at (2).toUShort ());
    }

    connect (languageSelecter, SIGNAL (currentIndexChanged (int)), this, SLOT (changeLanguage ()));
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

void OptionsWidget::changeLanguage ()
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
}

