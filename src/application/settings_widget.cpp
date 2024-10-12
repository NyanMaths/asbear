#include <QProcess>
#include <QThread>

#include <QFile>

#include "application/settings_widget.hpp"


////////////// Initialize widget


OptionsWidget::OptionsWidget (const QMap<QString, QString>& config) : QWidget ()
{
    layout = new QGridLayout(this);


    aboutLabel = new QLabel(tr(
        "This opensource software was developed by NY4N_M4THS<br/>"
        "under <a href = 'https://gnu.org/licenses/gpl-3.0.en.html'>GPL 3.0</a> with the framework <a href = 'https://qt.io'>Qt</a> and <a href = 'https://sfml-dev.org'>SFML</a> in C++ language.<br/>"
        "You can also visit <a href = 'https://memetech-inc.weebly.com'>our website</a> to check for updates,<br/>"
        "try other of our applications or <a href = 'https://github.com/NyanMaths/MRecorder/issues'>ask for new features</a> !<br/>"
        "Click <a href = 'https://github.com/NyanMaths/MRecorder'>here</a> to visit the GitHub page of the project."));

    aboutLabel->setOpenExternalLinks(true);
    aboutLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    aboutLabel->setTextFormat(Qt::RichText);


    initOptionsBox();


    NY4N_M4THS = new QLabel;
    NY4N_M4THS->setPixmap(QPixmap("data/gui/author.png"));

    layout->addWidget(aboutLabel, 0, 0, 1, 3);
    layout->addWidget(NY4N_M4THS, 0, 3, Qt::AlignRight);
    layout->addWidget(UIOptionsBox, 1, 0, 1, 2);

    loadConfig(config);
}

void OptionsWidget::initOptionsBox ()
{
    UIOptionsBox = new QGroupBox(tr("UI options"));
    UIOptionsBoxLayout = new QGridLayout(UIOptionsBox);
    UIOptionsBoxLayout->setAlignment(Qt::AlignLeft);

    chooseLanguageLabel = new QLabel(tr("Display language :"));

    languageSelecter = new QComboBox;
    languageSelecter->addItem("English", QVariant("en"));
    languageSelecter->addItem("Français", QVariant("fr"));


    themeSwitcher = new QPushButton(tr("Join the dark side, Luke"));
    themeSwitcher->setCheckable(true);


    UIOptionsBoxLayout->addWidget(chooseLanguageLabel, 0, 0);
    UIOptionsBoxLayout->addWidget(languageSelecter, 0, 1);
    UIOptionsBoxLayout->addWidget(themeSwitcher, 1, 0, 1, 2);
}


void OptionsWidget::loadConfig (const QMap<QString, QString>& config)
{
    languageSelecter->setCurrentText(config["options-language"]);

    themeSwitcher->setChecked(config["dark-theme"] == "1" ? true : false);
    if (config["dark-theme"] == "1")
    {
        loadDarkPalette();
        themeSwitcher->setText(tr("To light theme, if you have no taste"));
    }


    connect(languageSelecter, SIGNAL(currentIndexChanged(int)), this, SLOT(promptToRestart()));
    connect(themeSwitcher, SIGNAL(toggled(bool)), this, SLOT(promptToRestart()));
}

void OptionsWidget::getConfig (QMap<QString, QString>& config)
{
    config["options-language"] = languageSelecter->currentText();

    // These both do not have the option- prefix because they can be used globally, it is just a convention problem
    config["dark-theme"] = QString::number(themeSwitcher->isChecked());
    config["locale"] = languageSelecter->currentData().toString();
}


////////////// Slots


void OptionsWidget::promptToRestart ()
{
    if (QMessageBox::question(this, tr("Required restart"), tr("You need to reload the application to apply changes.\nDo you want to do it now ?")) == QMessageBox::Yes)
    {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
    else
    {
        if (themeSwitcher->isChecked())
            themeSwitcher->setText(tr("To light theme, if you have no taste"));
        else
            themeSwitcher->setText(tr("Join the dark side, Luke"));
    }
}


////////////// Others


void OptionsWidget::loadDarkPalette ()
{
    QPalette darkPalette;

    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127,127,127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127,127,127));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127,127,127));
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127,127,127));

    qApp->setPalette(darkPalette);
}

