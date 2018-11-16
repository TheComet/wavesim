cmake_minimum_required (VERSION 3.3 FATAL_ERROR)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif ()

project ("qwt"
    VERSION 6.1.3
    LANGUAGES CXX)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include (GNUInstallDirs)

###############################################################################
# config
###############################################################################

set (QWT_VER_MAJ ${PROJECT_VERSION_MAJOR})
set (QWT_VER_MIN ${PROJECT_VERSION_MINOR})
set (QWT_VER_PAT ${PROJECT_VERSION_PATCH})
set (QWT_VERSION ${PROJECT_VERSION})

option (QWT_SHARED_LIB "Enable this to build Qwt as a shared library. Disable
to build as a static library" OFF)

option (QWT_PLOT "Enables all classes that are needed to use the QwtPlot
widget" ON)

option (QWT_WIDGETS "Enables all classes that are needed to use all other
widgets (sliders, dials, etc.), besides QwtPlot" ON)

option (QWT_SVG "If you want to display svg images on the plot canvas or
export a plot to a svg document" ON)

option (QWT_OPENGL "If you want to use an OpenGL plot canvas" ON)

option (QWT_MATHML "You can use the MathML renderer of the Qt solutions
package to enable MathML support in Qwt. Because of license implications the (
modified ) code of the MML Widget solution is included and linked together
with the QwtMathMLTextEngine into its own library." OFF)

option (QWT_DESIGNER "If you want to build the Qwt designer plugins." ON)

if (WIN32)
    option (QWT_DESIGNER_SELF_CONTAINED "Compile all Qwt classes into the
    designer plugin instead of linking it against the shared Qwt library. Has
    no effect when QWT_DESIGNER or QWT_SHARED_LIB are not both enabled." OFF)
endif ()

if (APPLE)
    option (QWT_FRAMEWORK "Build as a framework" ON)
endif ()

if (UNIX)
    option (QWT_PKG_CONFIG "Create and install pc files for pkg-config" OFF)
endif ()

###############################################################################
# sub directories
###############################################################################

add_subdirectory ("src")
add_subdirectory ("textengines")
if (QWT_DESIGNER)
    add_subdirectory ("designer")
endif ()
