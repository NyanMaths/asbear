#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H


#include <QApplication>

#include <QComboBox>
#include <QLabel>

#include <QGroupBox>
#include <QGridLayout>

#include <QMessageBox>


class OptionsWidget : public QWidget
{
    Q_OBJECT

    public:
        OptionsWidget ();
        ~OptionsWidget ();


    private slots:
        void promptToRestart ();
        void changeTheme (int);


    private:
        void initPalettes ();
        void initOptionsBox ();

        void loadOptions ();


        QPalette lightPalette;
        QPalette darkPalette;


        QGridLayout* layout;

        QLabel* aboutLabel;

        QGroupBox* UIOptionsBox;
        QGridLayout* UIOptionsBoxLayout;

          QLabel* chooseLanguageLabel;
          QComboBox* languageSelecter;

          QLabel* chooseThemeLabel;
          QComboBox* themeSelecter;

        QLabel* NY4N_M4THS;
};


#endif // OPTIONSWIDGET_H
