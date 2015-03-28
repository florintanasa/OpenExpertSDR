#-------------------------------------------------
#
# Project created by QtCreator 2015-02-22T18:11:09
#
#-------------------------------------------------

QT       += core gui serialport opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ExpertSDR
TEMPLATE = app

include(Cat/QextSerialPort/qextserialport.pri)

SOURCES += main.cpp\
        expertsdr_va2_1.cpp \
        FreqScale/Didgit.cpp \
        Timer/Timer.cpp \
        Panarama.cpp \
        PanoramOpt.cpp \
        SmeterGUI/ColorWgt.cpp \
        SmeterGUI/SMeterOptions.cpp \
        SmeterGUI/SMeterSDR.cpp \
        GLFont/glfont.cpp \
        DttSP.cpp \
        ThreadFunc.cpp \
        dttsplibrary.cpp \
        SdrPlugin/PortAudio/pa19.cpp \
        SdrPlugin/SdrPlugin.cpp \
        SdrPlugin/PluginCtrl.cpp \
        SdrPlugin/extioplugin.cpp \
        scale/Scale.cpp \
        scale/ScaleNum.cpp \
        S-Meter/Number2.cpp \
        S-Meter/Number.cpp \
        S-Meter/Draw.cpp \
        S-Meter/S_Meter.cpp \
        Wav/fileList.cpp \
        Wav/wavSample.cpp \
        WavReader.cpp \
        Wav/VoiceRecorderWave.cpp \
        CalibrateSC/Calibrator/CalibrProc.cpp \
        CalibrateSC/Calibrator/Calibrator.cpp \
        CalibrateSC/progresscalibrate.cpp \
        Logger/VLogger.cpp \
        Logger/VLogger.h \
#        CW/CwCore.cpp \
#        CW/CwMacro.cpp \
#        CW/CwTimer.cpp \
#        CW/correctIQ.cpp \
        ringBuf.cpp \
        About.cpp \
        AudioThread.cpp \
        WdgGraph.cpp \
        WdgAddStation.cpp \
        WdgMem.cpp \
        Vac.cpp \
        Resampler.cpp \
        RingResampler/RingResampler.cpp \
        RingResampler/RingBuffer.cpp \
        Cat/CatManager.cpp \
        Options.cpp
#       Cat/ExtSerialPort/qextserialbase.cpp \
#        Cat/ExtSerialPort/qextserialenumerator.cpp \
#        Cat/ExtSerialPort/qextserialport.cpp \
#       Cat/ExtSerialPort/win_qextserialport.cpp \


HEADERS  += expertsdr_va2_1.h \
            FreqScale/Didgit.h \
            Timer/Timer.h \
            Defines.h \
            Panarama.h \
            PanaramDefines.h \
            PanoramOpt.h \
            SmeterGUI/ColorWgt.h \
            SmeterGUI/SMeterOptions.h \
            SmeterGUI/SMeterSDR.h \
            GLFont/glfont.h \
            DttSP.h \
            ThreadFunc.h \
            dttsplibrary.h \
            SdrPlugin/PortAudio/pa19.h \
            SdrPlugin/PortAudio/portaudio.h \
            SdrPlugin/SdrPlugin.h \
            SdrPlugin/PluginCtrl.h \
            SdrPlugin/extioplugin.h \
            scale/Scale.h \
            scale/ScaleNum.h \
            S-Meter/Number2.h \
            S-Meter/Number.h \
            S-Meter/Draw.h \
            S-Meter/S_Meter.h \
            Wav/fileList.h \
            Wav/wavSample.h \
            WavReader.h \
            Wav/VoiceRecorderWave.h \
            CalibrateSC/Calibrator/CalibrProc.h \
            CalibrateSC/Calibrator/Calibrator.h \
            CalibrateSC/progresscalibrate.h \
#            CW/CwCore.h \
#            CW/CwMacro.h \
#            CW/CwTimer.h \
#            CW/correctIQ.h \
            ringBuf.h\
            About.h \
            AudioThread.h \
            WdgGraph.h \
            WdgAddStation.h \
            WdgMem.h \
            Resampler.h \
            Vac.h \
            RingResampler/RingResampler.h \
            RingResampler/RingBuffer.h \
            Cat/CatManager.h \
            Options.h
#            Cat/ExtSerialPort/qextserialbase.h \
#            Cat/ExtSerialPort/qextserialenumerator.h \
#            Cat/ExtSerialPort/qextserialport.h \
#            Cat/ExtSerialPort/win_qextserialport.h \


FORMS    += expertsdr_va2_1.ui \
            PanoramOpt.ui \
            SmeterGUI/SMeterOptions.ui \
            scale/Scale.ui \
            S-Meter/Number2.ui \
            S-Meter/Number.ui \
            S-Meter/S_Meter.ui \
            Wav/fileList.ui \
            Wav/wavSample.ui \
            CalibrateSC/Calibrator/Calibrator.ui \
            CalibrateSC/progresscalibrate.ui \
#            CW/CwMacro.ui \
            About.ui \
            WdgGraph.ui \
            WdgAddStation.ui \
            WdgMem.ui \
            Options.ui

RESOURCES += images.qrc \
            bg.qrc

LIBS +=  -lDttSP -lGLU -lportaudio -lpthread -lm -lfftw3f -lm
QMAKE_CXXFLAGS += -Wno-unused-function
