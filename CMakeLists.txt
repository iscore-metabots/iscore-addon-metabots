cmake_minimum_required(VERSION 3.1)
project(iscore_addon_metabots LANGUAGES CXX)

if(NOT TARGET iscore_plugin_engine)
    return()
endif()

find_package(Qt5 COMPONENTS SerialPort)
iscore_common_setup()

set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Metabots/Protocol/Protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Metabots/Protocol/Protocol.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Metabots/Device/Device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Metabots/Device/Device.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_addon_metabots.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_addon_metabots.cpp"
)


add_library(${PROJECT_NAME} ${SRCS})

iscore_generate_command_list_file(${PROJECT_NAME} "${SRCS}")
target_link_libraries(${PROJECT_NAME} PUBLIC iscore_plugin_engine Qt5::SerialPort)

setup_iscore_plugin(${PROJECT_NAME})
