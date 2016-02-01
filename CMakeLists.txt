CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FILE(GLOB file_cpp_src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB file_h_src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

ADD_EXECUTABLE(text_box 
    ${file_cpp_src}
    ${file_h_src})
