cmake_minimum_required(VERSION 3.10)

project(camera_calibraion_project)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(camera_calibration src/main.cpp)
target_link_libraries(camera_calibration ${OpenCV_LIBS})
