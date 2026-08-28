cmake_minimum_required(VERSION 3.16)

project(libfreeverb
    VERSION 1.0.0
    DESCRIPTION "A portable, sample-rate independent implementation of the Freeverb reverberator"
    LANGUAGES CXX)

add_library(freeverb STATIC
    src/Freeverb.cpp
    src/freeverb/allpass.cpp
    src/freeverb/comb.cpp
    src/freeverb/revmodel.cpp)

add_library(freeverb::freeverb ALIAS freeverb)

target_compile_features(freeverb PUBLIC cxx_std_17)

target_include_directories(freeverb
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)

set_target_properties(freeverb PROPERTIES
    POSITION_INDEPENDENT_CODE ON)

if(PROJECT_IS_TOP_LEVEL)
    include(GNUInstallDirs)
    install(TARGETS freeverb ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
