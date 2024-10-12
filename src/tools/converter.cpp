#include "tools/converter.hpp"


Converter::Converter () : QThread() { }


void Converter::convert (const QStringList& files, const QStringList& outputFiles, unsigned int speed, QThread::Priority conversionPriority)
{
    maxCount = speed;
    threadPriority = conversionPriority;

    this->files = files;
    this->outputFiles = outputFiles;


    start (threadPriority);
}

void Converter::run ()
{
    short int samples [maxCount];

    for (unsigned short int i(0) ; i != files.length() ; i++)
    {
        emit nextFile(files.at(i));
        emit progress(0);

        inputStream.openFromFile(std::string(files.at(i).toLocal8Bit()));
        outputStream.openFromFile(std::string(outputFiles.at(i).toLocal8Bit()), inputStream.getSampleRate(), inputStream.getChannelCount());

        writeFile(&samples[0]);
    }

    inputStream.openFromFile("");
    outputStream.openFromFile("", 0, 0);


    emit finishedConverting(outputFiles);
}

void Converter::writeFile (short int samples[])
{
    currentProgression = 0;

    short int readSamples(inputStream.read(&samples[0], maxCount));
    unsigned long long int samplesCount(0);


    while (readSamples != 0)
    {
        samplesCount += readSamples;
        if (double(samplesCount) / double(inputStream.getSampleCount()) * 100.0 > currentProgression)
        {
            currentProgression = double(samplesCount) / double(inputStream.getSampleCount()) * 100.0;

            emit progress(currentProgression);
        }

        outputStream.write(&samples[0], readSamples);

        readSamples = inputStream.read(&samples[0], maxCount);
    }
}
