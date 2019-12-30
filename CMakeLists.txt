cmake_minimum_required(VERSION 3.15)
project(CloneDetector)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(dependencies/tinyXML)
add_subdirectory(dependencies/gtest)

include_directories(dependencies/tinyXML)
include_directories(dependencies/gtest/googletest/include)

add_executable(CloneDetector main/main.cpp
                             main/XMLParser.cpp
                             main/XMLParser.h
                             main/Node.cpp
                             main/Node.h
                             main/Utility.cpp
                             main/Utility.h
                             main/Components.cpp
                             main/Components.h)

target_link_libraries(CloneDetector tinyxml2)

add_executable(Test testing/test.cpp
                    main/XMLParser.cpp
                    main/XMLParser.h
                    main/Node.cpp
                    main/Node.h
                    main/Utility.cpp
                    main/Utility.h
                    main/Components.cpp
                    main/Components.h)
target_link_libraries(Test gtest tinyxml2)