#ifndef MRECORDER_H
#define MRECORDER_H


#include <QApplication>

#include <QTabWidget>
#include "RecorderWidget.h"
#include "RecordingsManagerWidget.h"
#include "ConverterWidget.h"
#include "OptionsWidget.h"


class Application : public QTabWidget
{
    Q_OBJECT


    public:
        Application ();


    private:
        void closeEvent (QCloseEvent*);

        QTranslator* translator;
        QTranslator* messageBoxesTranslator;

        RecorderWidget* recorderTab;

        RecordingsManagerWidget* recordingsTab;

        ConverterWidget* converterTab;

        OptionsWidget* optionsTab;
};


#endif // MRECORDER_H
