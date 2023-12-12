cmake_minimum_required(VERSION 3.15)
project(Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(LIBS ${PROJECT_SOURCE_DIR}/../../libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin/server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)

file(GLOB_RECURSE sql_SRC sql/*.sql)
file(GLOB server_SRC CONFIGURE_DEPENDS "*.h" "*.cpp")

add_executable(Server ${server_SRC} ${sql_SRC})
include_directories("../../src")

target_include_directories(Server PRIVATE ${LIBS})
target_link_libraries(Server Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Server Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(Server Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(Server common)

target_include_directories(Server PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32 AND MSVC)
        target_include_directories(Server PRIVATE ${LIBS}/protobuf/win/msvc-debug/include)
    elseif (UNIX AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_include_directories(Server PRIVATE ${LIBS}/protobuf/linux/gcc-debug/include)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if (WIN32 AND MSVC)
        target_include_directories(Server PRIVATE ${LIBS}/protobuf/win/msvc-release/include)
    elseif (UNIX AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_include_directories(Server PRIVATE ${LIBS}/protobuf/linux/gcc-release/include)
    endif()
endif()
