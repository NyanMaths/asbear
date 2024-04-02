#ifndef APPLICATION_H
#define APPLICATION_H


#include <QApplication>

#include <QTabWidget>
#include "application/recorder_widget.hpp"
#include "application/recording_manager_widget.hpp"
#include "application/converter_widget.hpp"
#include "application/settings_widget.hpp"


class Application : public QTabWidget
{
    Q_OBJECT


    public:
        Application();
        virtual ~Application();


    private:
        void readConfig();
        void saveConfig();


        void closeEvent(QCloseEvent*);

        QTranslator* translator;
        QTranslator* messageBoxesTranslator;

        QMap<QString, QString> config;


        RecorderWidget* recorder;

        RecordingsManagerWidget* recordingsManager;

        ConverterWidget* converter;

        OptionsWidget* options;
};


#endif  // APPLICATION_H
