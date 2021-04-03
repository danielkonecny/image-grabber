cmake_minimum_required(VERSION 2.8)
project(ImageGrabber)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/opt/pylon/include)
link_directories(/opt/pylon/lib)
add_executable(
	ImageGrabber
	src/main.cpp
	src/ImageGrabber.cpp
	src/ImageEventHandler.cpp
	src/ConfigurationEventPrinter.cpp
	src/ArgumentsParser.cpp
)
target_link_libraries(ImageGrabber ${OpenCV_LIBS} pylonutility pylonbase GCBase_gcc_v3_1_Basler_pylon GenApi_gcc_v3_1_Basler_pylon)
