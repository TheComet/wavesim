###############################################################################
# Setup, project name, etc.
###############################################################################

cmake_minimum_required (VERSION 3.2 FATAL_ERROR)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif ()

project ("Wave Simulator"
    VERSION 0.1
    LANGUAGES C)

# Must use GNUInstallDirs to install libraries into correct locations on all
# platforms.
include(GNUInstallDirs)

add_subdirectory ("thirdparty/googletest")
add_subdirectory ("wavesim")
add_subdirectory ("frontend")
