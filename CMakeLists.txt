cmake_minimum_required(VERSION 2.8.11)
project(ExpertSDR)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
# Find the QtOpenGL library
find_package(Qt5OpenGL REQUIRED)

set(expertsdr_SRC
        main.cpp
        expertsdr_va2_1.cpp
        FreqScale/Didgit.cpp
        Timer/Timer.cpp
        Panarama.cpp
        PanoramOpt.cpp
        SmeterGUI/ColorWgt.cpp
        SmeterGUI/SMeterOptions.cpp
        SmeterGUI/SMeterSDR.cpp
        GLFont/glfont.cpp
        DttSP.cpp
        ThreadFunc.cpp
        dttsplibrary.cpp
        SdrPlugin/PortAudio/pa19.cpp
        SdrPlugin/SdrPlugin.cpp
        SdrPlugin/PluginCtrl.cpp
        SdrPlugin/extioplugin.cpp
        scale/Scale.cpp
        scale/ScaleNum.cpp
        S-Meter/Number2.cpp
        S-Meter/Number.cpp
        S-Meter/Draw.cpp
        S-Meter/S_Meter.cpp
        Wav/fileList.cpp
        Wav/wavSample.cpp
        WavReader.cpp
        Wav/VoiceRecorderWave.cpp
        CalibrateSC/Calibrator/CalibrProc.cpp
        CalibrateSC/Calibrator/Calibrator.cpp
        CalibrateSC/progresscalibrate.cpp
        Logger/VLogger.cpp
        Logger/VLogger.h
        CW/CwCore.cpp
        CW/CwMacro.cpp
        CW/CwTimer.cpp
        CW/correctIQ.cpp
        ringBuf.cpp
        About.cpp
        AudioThread.cpp
        WdgGraph.cpp
        WdgAddStation.cpp
        WdgMem.cpp
        Vac.cpp
        Resampler.cpp
        RingResampler/RingResampler.cpp
        RingResampler/RingBuffer.cpp
        Cat/CatManager.cpp
        Options.cpp
        Cat/QextSerialPort/qextserialport.cpp
        Cat/QextSerialPort/qextserialport_unix.cpp
        Cat/QextSerialPort/qextserialenumerator.cpp
        Cat/QextSerialPort/qextserialenumerator_linux.cpp
#       Cat/ExtSerialPort/qextserialbase.cpp
#        Cat/ExtSerialPort/qextserialenumerator.cpp
#        Cat/ExtSerialPort/qextserialport.cpp
#       Cat/ExtSerialPort/win_qextserialport.cpp
)

# Create code from a list of Qt designer ui files.
#set(CMAKE_AUTOUIC ON) # use this if you have CMake 3.x instead of the following
qt5_wrap_ui(expertsdr_SRC expertsdr_va2_1.ui
            PanoramOpt.ui
            SmeterGUI/SMeterOptions.ui
            scale/Scale.ui
            S-Meter/Number2.ui
            S-Meter/Number.ui
            S-Meter/S_Meter.ui
            Wav/fileList.ui
            Wav/wavSample.ui
            CalibrateSC/Calibrator/Calibrator.ui
            CalibrateSC/progresscalibrate.ui
            CW/CwMacro.ui
            About.ui
            WdgGraph.ui
            WdgAddStation.ui
            WdgMem.ui
            Options.ui
)

SET(expertsdr_RESOURCES images.qrc bg.qrc)

qt5_ADD_RESOURCES(expertsdr_RESOURCES_RCC ${expertsdr_RESOURCES})

# Tell CMake to create the helloworld executable
add_executable(expertsdr ${expertsdr_SRC} ${expertsdr_RESOURCES_RCC})

# Use the Widgets module from Qt 5.
target_link_libraries(expertsdr Qt5::Widgets Qt5::OpenGL GLU GL udev portaudio)

# Install the executable
install(TARGETS expertsdr DESTINATION bin)
