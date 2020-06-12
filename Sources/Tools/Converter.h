#ifndef CONVERTER_H
#define CONVERTER_H


#include <QThread>

#include <SFML/Audio.hpp>


class Converter : public QThread
{
    Q_OBJECT

    public:
        Converter ();

        void convert (const QStringList&, const QStringList&, unsigned int);


    signals:
        void duplicateFile (QString&, bool&);

        void progress (unsigned short int);

        void nextFileReached (const QString&);
        void finishedConverting (const QStringList&);


    private:
        virtual void run () override;

        void writeFile (short int[]);


        unsigned int maxCount;
        unsigned short int currentProgression;

        QStringList files;
        QStringList outputFiles;

        sf::InputSoundFile inputStream;
        sf::OutputSoundFile outputStream;
};


#endif // CONVERTER_H
