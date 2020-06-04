#include "DevicesComboBox.h"

#include <SFML/Audio.hpp>


DevicesComboBox::DevicesComboBox () : QComboBox ()
{
    devices = sf::SoundRecorder::getAvailableDevices ();

    for (unsigned short int i = 0 ; i != devices.size () ; i++)
        addItem (QString::fromStdString (devices.at (i)));


    setCurrentIndex (0);
}

void DevicesComboBox::mousePressEvent (QMouseEvent* event)
{
    QString currentDevice (currentText ());
    devices = sf::SoundRecorder::getAvailableDevices ();

    clear ();

    for (unsigned short int i = 0 ; i != devices.size () ; i++)
        addItem (QString::fromStdString (devices.at (i)));

    setCurrentText (currentDevice);

    showPopup ();


    event->accept ();
}
