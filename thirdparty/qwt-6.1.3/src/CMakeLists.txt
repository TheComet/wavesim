###############################################################################
# Library, source and header files
###############################################################################

include (TestVisibilityMacros)

test_visibility_macros (QWT_HELPER_API_EXPORT QWT_HELPER_API_IMPORT QWT_HELPER_API_LOCAL)

configure_file ("cmake/templates/qwt_global.h.in" "include/qwt_global.h")

file (GLOB QWT_HEADERS "core/include/*.h")
file (GLOB QWT_SOURCES "core/src/*.cpp")
set (QWT_MOC_HEADERS
    "core/include/qwt_dyngrid_layout.h"
    "core/include/qwt_magnifier.h"
    "core/include/qwt_panner.h"
    "core/include/qwt_picker.h"
    "core/include/qwt_text_label.h")
if (QWT_PLOT)
    file (GLOB QWT_PLOT_HEADERS "plot/include/*.h")
    file (GLOB QWT_PLOT_SOURCES "plot/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_PLOT_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_PLOT_SOURCES})
    list (APPEND QWT_MOC_HEADERS
        "plot/include/qwt_abstract_legend.h"
        "plot/include/qwt_legend.h"
        "plot/include/qwt_legend_label.h"
        "plot/include/qwt_plot.h"
        "plot/include/qwt_plot_renderer.h"
        "plot/include/qwt_plot_canvas.h"
        "plot/include/qwt_plot_panner.h"
        "plot/include/qwt_plot_picker.h"
        "plot/include/qwt_plot_zoomer.h"
        "plot/include/qwt_plot_magnifier.h"
        "plot/include/qwt_sampling_thread.h"
        "plot/include/qwt_scale_widget.h")
endif ()
if (QWT_SVG)
    file (GLOB QWT_SVG_HEADERS "svg/include/*.h")
    file (GLOB QWT_SVG_SOURCES "svg/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_SVG_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_SVG_SOURCES})
endif ()
if (QWT_OPENGL)
    file (GLOB QWT_OPENGL_HEADERS "opengl/include/*.h")
    file (GLOB QWT_OPENGL_SOURCES "opengl/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_OPENGL_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_OPENGL_SOURCES})
    list (APPEND QWT_MOC_HEADERS "opengl/include/qwt_plot_glcanvas.h")
endif ()
if (QWT_WIDGETS)
    file (GLOB QWT_WIDGETS_HEADERS "widgets/include/*.h")
    file (GLOB QWT_WIDGETS_SOURCES "widgets/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_WIDGETS_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_WIDGETS_SOURCES})
    list (APPEND QWT_MOC_HEADERS
        "widgets/include/qwt_abstract_slider.h"
        "widgets/include/qwt_abstract_scale.h"
        "widgets/include/qwt_analog_clock.h"
        "widgets/include/qwt_compass.h"
        "widgets/include/qwt_counter.h"
        "widgets/include/qwt_dial.h"
        "widgets/include/qwt_knob.h"
        "widgets/include/qwt_slider.h"
        "widgets/include/qwt_thermo.h"
        "widgets/include/qwt_wheel.h")
endif ()

find_package (Qt5Core REQUIRED)
find_package (Qt5Concurrent REQUIRED)
find_package (Qt5PrintSupport REQUIRED)
qt5_wrap_cpp (QWT_MOC_SOURCES ${QWT_MOC_HEADERS})

if (QWT_SHARED_LIB)
    set (BUILD_TYPE "SHARED")
else ()
    set (BUILD_TYPE "STATIC")
endif ()
add_library (qwtinternal ${BUILD_TYPE}
    ${QWT_SOURCES}
    ${QWT_HEADERS}
    ${QWT_MOC_SOURCES})
target_include_directories (qwtinternal
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_BINARY_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/core/include
            $<$<BOOL:${QWT_PLOT}>:${CMAKE_CURRENT_SOURCE_DIR}/plot/include>
            $<$<BOOL:${QWT_SVG}>:${CMAKE_CURRENT_SOURCE_DIR}/svg/include>
            $<$<BOOL:${QWT_OPENGL}>:${CMAKE_CURRENT_SOURCE_DIR}/opengl/include>
            $<$<BOOL:${QWT_WIDGETS}>:${CMAKE_CURRENT_SOURCE_DIR}/widgets/include>
        >)
target_compile_definitions (qwtinternal
    PRIVATE
        QWT_BUILDING
        $<$<BOOL:${QWT_SHARED_LIB}>:QT_DLL QWT_DLL QWT_MAKEDLL>)
target_compile_options (qwtinternal
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-W -Wall -Wextra -pedantic -Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:Clang>:-W -Wall -Wextra -pedantic -Wno-unused-parameter>)
target_link_libraries (qwtinternal
    Qt5::Core
    Qt5::Concurrent
    Qt5::PrintSupport)
if (QWT_WIDGETS)
    find_package (Qt5Widgets REQUIRED)
    target_link_libraries (qwtinternal Qt5::Widgets)
endif ()
if (QWT_SVG)
    find_package (Qt5Svg REQUIRED)
    target_link_libraries (qwtinternal Qt5::Svg)
endif ()
if (QWT_OPENGL)
    find_package (Qt5OpenGL REQUIRED)
    target_link_libraries (qwtinternal Qt5::OpenGL)
endif ()
