cmake_minimum_required(VERSION 3.16)
project(ImageGrabber)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/opt/pylon/include)
link_directories(/opt/pylon/lib)

add_executable(
        ImageGrabber
        src/main.cpp src/main.h
        src/ImageGrabber.cpp src/ImageGrabber.h
        src/ImageEventHandler.cpp src/ImageEventHandler.h
        src/ConfigurationEventPrinter.cpp src/ConfigurationEventPrinter.h
        src/ArgumentsParser.cpp src/ArgumentsParser.h
        src/SignalHandler.cpp src/SignalHandler.h
)

target_link_libraries(
        ImageGrabber
        stdc++fs
        ${OpenCV_LIBS}
        pylonutility
        pylonbase
        GCBase_gcc_v3_1_Basler_pylon
        GenApi_gcc_v3_1_Basler_pylon
)
