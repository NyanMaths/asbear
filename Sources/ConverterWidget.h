#ifndef CONVERTERWIDGET_H
#define CONVERTERWIDGET_H


#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QSpinBox>
#include "RecordingsManagerWidget.h"

#include <QGroupBox>
#include <QVBoxLayout>

#include "Tools/Converter.h"


class ConverterWidget : public QWidget
{
    Q_OBJECT

    public:
        ConverterWidget (RecordingsManagerWidget*);
        ~ConverterWidget ();

        void addFile (const QString&);


    private slots:
        void onFileClicked ();

        void addFiles ();
        void removeFile ();
        void clear ();

        void start ();

        void reactivateUI (const QStringList&);
        void updateCurrentFileLabel (const QString&);
        void updateProgressBar (unsigned short int);

        void resetSettings ();


    private:
        void initFilesWidget ();
        void initOptionsBox ();
        void loadOptions ();

        void updateUI ();
        void setOptionsEnabled (bool);

        virtual void dragEnterEvent (QDragEnterEvent*);
        virtual void dropEvent (QDropEvent*);


        RecordingsManagerWidget* fileManager;
        Converter* converter;

        QVBoxLayout* layout;

        QLabel* helpLabel;

        QWidget* filesWidget;
        QGridLayout* filesWidgetLayout;
          QListWidget* filesList;
          QPushButton* bAddFiles;
          QPushButton* bRemoveFile;
          QPushButton* bClear;

        QGroupBox* optionsBox;
        QGridLayout* optionsBoxLayout;

          QLabel* chooseCodecLabel;
          QComboBox* codecSelecter;

          QLabel* chooseSpeedLabel;
          QSpinBox* speedSelecter;

          QPushButton* bResetSettings;

        QLabel* currentFileLabel;
        QProgressBar* progressBar;
        QPushButton* bStart;
};


#endif // CONVERTERWIDGET_H
