#ifndef DEVICESCOMBOBOX_H
#define DEVICESCOMBOBOX_H


#include <QComboBox>
#include <QMouseEvent>


class DevicesComboBox : public QComboBox
{
    public:
        DevicesComboBox ();


    private:
       virtual void mousePressEvent (QMouseEvent*);

       std::vector<std::string> devices;
};


#endif // DEVICESCOMBOBOX_H
