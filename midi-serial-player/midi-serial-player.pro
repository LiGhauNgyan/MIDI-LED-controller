#-------------------------------------------------
#
# Project created by QtCreator 2021-04-18T23:44:06
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = midi-serial-player
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        3rdparty/midifile/src/Binasc.cpp \
        3rdparty/midifile/src/MidiEvent.cpp \
        3rdparty/midifile/src/MidiEventList.cpp \
        3rdparty/midifile/src/MidiFile.cpp \
        3rdparty/midifile/src/MidiMessage.cpp \
        3rdparty/midifile/src/Options.cpp \
        main.cpp \
        mainwindow.cpp
HEADERS += \
        3rdparty/midifile/include/Binasc.h \
        3rdparty/midifile/include/MidiEvent.h \
        3rdparty/midifile/include/MidiEventList.h \
        3rdparty/midifile/include/MidiFile.h \
        3rdparty/midifile/include/MidiMessage.h \
        3rdparty/midifile/include/Options.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += \
        3rdparty/midifile/include


