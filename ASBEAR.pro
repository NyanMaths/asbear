QT += core gui widgets

CONFIG += c++17

TARGET = ../release/bin/asbear


INCLUDEPATH += src


LIBS += -lsfml-audio
LIBS += -lsfml-system


TRANSLATIONS = lang/asbear-fr.ts

HEADERS += \
    src/Application.hpp \
    src/tabs/ConverterWidget.hpp \
    src/tabs/OptionsWidget.hpp \
    src/tabs/RecorderWidget.hpp \
    src/tabs/RecordingsManagerWidget.hpp \
    src/tools/AudioRecorder.hpp \
    src/tools/Converter.hpp \
    src/tools/RecordingPlayer.hpp \
    src/widgets/AudioLevelWidget.hpp \
    src/widgets/DevicesComboBox.hpp \
    src/widgets/DirectJumpSlider.hpp \
    src/widgets/PlaybackBar.hpp \
    src/widgets/SpectrumWidget.hpp

SOURCES += \
    src/Application.cpp \
    src/main.cpp \
    src/tabs/ConverterWidget.cpp \
    src/tabs/OptionsWidget.cpp \
    src/tabs/RecorderWidget.cpp \
    src/tabs/RecordingsManagerWidget.cpp \
    src/tools/AudioRecorder.cpp \
    src/tools/Converter.cpp \
    src/tools/RecordingPlayer.cpp \
    src/widgets/AudioLevelWidget.cpp \
    src/widgets/DevicesComboBox.cpp \
    src/widgets/DirectJumpSlider.cpp \
    src/widgets/PlaybackBar.cpp \
    src/widgets/SpectrumWidget.cpp
