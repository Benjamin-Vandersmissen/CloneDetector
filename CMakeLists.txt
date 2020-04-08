cmake_minimum_required(VERSION 3.14)
project(CloneDetector)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(dependencies/tinyXML)
add_subdirectory(dependencies/gtest)

include_directories(dependencies/tinyXML)
include_directories(dependencies/gtest/googletest/include)

add_executable(CloneDetector src/main.cpp
        src/main/XMLParser.cpp
        src/main/XMLParser.h
        src/main/Node.cpp
        src/main/Node.h
        src/main/Utility.cpp
        src/main/Utility.h
        src/main/Components.cpp
        src/main/Components.h
        src/main/Functions.cpp
        src/main/Functions.h
        src/main/Graph.h
        src/main/Graph.cpp
        src/main/Clone.h
        src/main/Clone.cpp
        src/main/Circuit.h
        src/main/Circuit.cpp)

target_link_libraries(CloneDetector tinyxml2)

add_executable(Test src/testing/test.cpp
        src/main/XMLParser.cpp
        src/main/XMLParser.h
        src/main/Node.cpp
        src/main/Node.h
        src/main/Utility.cpp
        src/main/Utility.h
        src/main/Components.cpp
        src/main/Components.h)
target_link_libraries(Test gtest tinyxml2)