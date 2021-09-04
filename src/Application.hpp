#ifndef APPLICATION_H
#define APPLICATION_H


#include <QApplication>

#include <QTabWidget>
#include "tabs/RecorderWidget.hpp"
#include "tabs/RecordingsManagerWidget.hpp"
#include "tabs/ConverterWidget.hpp"
#include "tabs/OptionsWidget.hpp"


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
