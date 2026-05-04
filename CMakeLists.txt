cmake_minimum_required(VERSION 3.5)

project(projectServer VERSION 0.1 LANGUAGES CXX)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror") # -w: disable all warnings
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOR RCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bins)
set(SYSTEM_SERVER_EXECUTABLE_DIR "/opt/project_server")


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(yaml-cpp REQUIRED)
message("YAML_CPP_CMAKE_DIR=${YAML_CPP_CMAKE_DIR}")
message("YAML_CPP_INCLUDE_DIR=${YAML_CPP_INCLUDE_DIR}")
message("YAML_CPP_LIBRARIES=${YAML_CPP_LIBRARIES}")
include_directories(${YAMLCPP_INCLUDE_DIR})
#message(${YAMLCPP_INCLUDE_DIR})
#find_package(PkgConfig REQUIRED)
#find_library(YAML_CPP_LIBRARY
#    NAMES yaml-cpp libyaml-cpp0.6
#    PATHS /usr/lib /usr/lib/x86_64-linux-gnu
#    )
#find_path(YAML_CPP_INCLUDE_DIR
#    NAMES yaml-cpp/yaml.h
#    PATHS /usr/include
#    )

#add_library(yaml_cpp UNKNOWN IMPORTED)


#set(TS_FILES projectServer_ru_RU.ts)
set(RESOURCES resources.qrc)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        udpserver.h
        udpserver.cpp
        server.cpp
        server.h
        mainwindow.ui
        ${TS_FILES}
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
#target_link_libraries(${PROJECT_NAME} PRIVATE ${YAML_CPP_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${YAML_CPP_LIBRARIES})
#message(${YAMLCPP_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION "${SYSTEM_SERVER_EXECUTABLE_DIR}")
install(DIRECTORY DESTINATION "${SYSTEM_SERVER_EXECUTABLE_DIR}/data" DIRECTORY_PERMISSIONS
WORLD_READ WORLD_WRITE WORLD_EXECUTE
OWNER_READ OWNER_WRITE OWNER_EXECUTE
GROUP_READ GROUP_WRITE GROUP_EXECUTE
)

#install(FILES "${CMAKE_SOURCE_DIR}/bins/data/settings.yaml"
#    DESTINATION "${SYSTEM_SERVER_EXECUTABLE_DIR}/data"
#    PERMISSIONS
#    OWNER_WRITE OWNER_READ
#    GROUP_WRITE GROUP_READ
#    WORLD_WRITE WORLD_READ
#)



if(UNIX)
    set(CPACK_DEBIAN_COMPRESSION_TYPE xz)

#    set(CPACK_DEB_COMPONENT_INSTALL ON)
#    set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
    set(CPACK_GENERATOR "DEB")

    set(CPACK_STRIP_FILES YES)

    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "JSC Sozvezdie")
    set(CPACK_PACKAGE_VERSION_MAJOR "1")
    set(CPACK_PACKAGE_VERSION_MINOR "0")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
    set(CPACK_SYSTEM_NAME "ubuntu")
    set(CPACK_DEBIAN_PACKAGE_SECTION "My Fisrt Deb")

    set(CPACK_PACKAGE_NAME "project-server")
#    set(CPACK_DEBIAN_PACKAGE_NAME "project-server")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "About")


    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
        "${CMAKE_CURRENT_SOURCE_DIR}/distribution/preinst;${CMAKE_CURRENT_SOURCE_DIR}/distribution/prerm;${CMAKE_CURRENT_SOURCE_DIR}/distribution/postinst;${CMAKE_CURRENT_SOURCE_DIR}/distribution/postrm")

    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5, libqt5gui5, libqt5core5a, libqt5network5")
    # set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libyaml-cpp0.6")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libyaml-cpp0.8")


    include(CPack)
endif(UNIX)
