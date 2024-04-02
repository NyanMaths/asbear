#ifndef CONVERTERWIDGET_H
#define CONVERTERWIDGET_H


#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QSpinBox>
#include "tabs/RecordingsManagerWidget.hpp"

#include <QGroupBox>
#include <QVBoxLayout>

#include "tools/Converter.hpp"


class ConverterWidget : public QWidget
{
    Q_OBJECT

    public:
        ConverterWidget(RecordingsManagerWidget*, const QMap<QString, QString>&);
        ~ConverterWidget();

        void addFile(const QString&);

        void getConfig(QMap<QString, QString>&) const;

        bool converting() const;


    private slots:
        void onFileClicked();

        void addFiles();
        void removeFile();
        void clear();

        void start();

        void reactivateUI(const QStringList&);
        void updateCurrentFileLabel(const QString&);
        void updateProgressBar(unsigned short int);

        void resetSettings();


    private:
        void initFilesWidget();
        void initOptionsBox();
        void loadConfig(const QMap<QString, QString>&);

        void updateUI();
        void setOptionsEnabled(bool);

        virtual void dragEnterEvent(QDragEnterEvent*);
        virtual void dropEvent(QDropEvent*);


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

          QLabel* choosePriorityLabel;
          QComboBox* prioritySelecter;

          QPushButton* bResetSettings;

        QLabel* currentFileLabel;
        QProgressBar* progressBar;
        QPushButton* bStart;
};


#endif  // CONVERTERWIDGET_H
