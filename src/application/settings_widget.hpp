#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H


#include <QApplication>

#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include <QGroupBox>
#include <QGridLayout>

#include <QMessageBox>


class OptionsWidget : public QWidget
{
    Q_OBJECT

    public:
        OptionsWidget(const QMap<QString, QString>&);


        void getConfig(QMap<QString, QString>&);


    private slots:
        void promptToRestart();


    private:
        void loadDarkPalette();
        void initOptionsBox();

        void loadConfig(const QMap<QString, QString>&);


        QGridLayout* layout;

        QLabel* aboutLabel;

        QGroupBox* UIOptionsBox;
        QGridLayout* UIOptionsBoxLayout;

          QLabel* chooseLanguageLabel;
          QComboBox* languageSelecter;

          QPushButton* themeSwitcher;

        QLabel* NY4N_M4THS;
};


#endif  // OPTIONSWIDGET_H
