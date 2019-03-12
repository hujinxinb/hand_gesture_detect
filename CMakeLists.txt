cmake_minimum_required(VERSION 3.6)
project(hand_gesture_detect)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF) #禁用CUDA


set(SOURCE_FILES hand_detect.cpp)


find_package(OpenCV REQUIRED)




add_executable(hand_gesture_detect ${SOURCE_FILES})

target_link_libraries(hand_gesture_detect ${OpenCV_LIBS})