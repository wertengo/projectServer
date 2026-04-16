cmake_minimum_required(VERSION 3.5)

project(projectServer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOR RCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network REQUIRED)

#set(TS_FILES projectServer_ru_RU.ts)
set(RESOURCES resources.qrc)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        server.cpp
        server.h
        mainwindow.ui
        ${TS_FILES}
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

