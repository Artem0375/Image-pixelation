cmake_minimum_required(VERSION 3.10)
project(pixelate CXX)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(MOC_HEADERS
    MainWindow.h.
)
add_executable(pixelate pixelate.cpp ${MOC_SOURCES})
target_link_libraries(pixelate PRIVATE ${OpenCV_LIBS} Qt6::Widgets)

