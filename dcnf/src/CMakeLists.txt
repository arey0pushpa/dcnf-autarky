cmake_minimum_required(VERSION 3.5.1)

project(DCNFAUTARKY)

SET (LLVM_DEBUG FALSE CACHE BOOL "Use Locally compiled and debug enabled LLVM")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message("debug mode")
    SET (LLVM_DEBUG TRUE)
    #SET (Z3_DEBUG TRUE)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

message(STATUS "DEBUG : LLVM - ${LLVM_DEBUG} ")

IF(WIN32)
    SET(Boost_USE_STATIC_LIBS    ON)
    SET(Boost_USE_STATIC_RUNTIME ON)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
       string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
ELSE(WIN32)
    SET(Boost_USE_STATIC_LIBS    OFF)
    SET(Boost_USE_STATIC_RUNTIME OFF)
ENDIF(WIN32)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.52.0 COMPONENTS regex program_options system filesystem REQUIRED) 

find_package (Threads)

SET (HB_UNICODE TRUE CACHE BOOL "Enable unicode output")

IF(NOT WIN32)
    ADD_DEFINITIONS(-std=c++11 -Wall)
ENDIF(NOT WIN32)

if (HB_UNICODE AND NOT WIN32)
  ADD_DEFINITIONS(-DHB_UNICODE)
ENDIF(HB_UNICODE AND NOT WIN32)

include_directories (${CMAKE_CURRENT_BINARY_DIR} "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/library")
include_directories(${Boost_INCLUDE_DIRS}) 
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

add_executable( dcnf-autarky main.cpp defs.h parse-qdimacs.cpp util.h )

target_link_libraries (dcnf-autarky ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
