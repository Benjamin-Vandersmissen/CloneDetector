cmake_minimum_required(VERSION 3.15)
project(CloneDetector)

set(CMAKE_CXX_STANDARD 14)

add_library(tinyxml2 tinyXML/tinyxml2.cpp tinyXML/tinyxml2.h)

add_executable(CloneDetector main.cpp XMLParser.cpp XMLParser.h Node.cpp Node.h Utility.cpp Utility.h Components.cpp Components.h)
target_link_libraries(CloneDetector tinyxml2)