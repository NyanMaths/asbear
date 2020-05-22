#include "DevicesComboBox.h"
#include "AudioRecorder.h"


DevicesComboBox::DevicesComboBox () : QComboBox ()
{
    devices = AudioRecorder::getAvailableDevices ();

    for (unsigned short int i = 0 ; i != devices.size () ; i++)
        addItem (QString::fromStdString (devices.at (i)));


    setCurrentIndex (0);
}

void DevicesComboBox::mouseReleaseEvent (QMouseEvent* event)
{
    clear ();


    devices = AudioRecorder::getAvailableDevices ();

    for (unsigned short int i = 0 ; i != devices.size () ; i++)
        addItem (QString::fromStdString (devices.at (i)));


    showPopup ();
    hidePopup ();
    event->accept ();
}
