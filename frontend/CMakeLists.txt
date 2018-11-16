option (WAVESIM_FRONTEND "Whether to build the GUI front-end or not" ON)

if (NOT WAVESIM_FRONTEND)
    return ()
endif ()

project ("wavesim GUI"
    VERSION 0.0.1
    LANGUAGES CXX)

add_subdirectory ("../thirdparty/qwt-6.1.3/" "${CMAKE_CURRENT_BINARY_DIR}/qwt-6.1.3")

find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)

file (GLOB FRONTEND_SOURCES "src/*.cpp")
file (GLOB FRONTEND_HEADERS "include/frontend/*.hpp")
set (FRONTEND_HEADERS_MOC)

qt5_wrap_cpp (FRONTEND_SOURCES_MOC
    "include/frontend/MainWindow.hpp")
qt5_wrap_ui (FRONTEND_SOURCES_UI
    "forms/MainWindow.ui")

add_executable (wavesim_frontend
    ${FRONTEND_SOURCES}
    ${FRONTEND_SOURCES_MOC}
    ${FRONTEND_SOURCES_UI})
target_include_directories (wavesim_frontend
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries (wavesim_frontend PUBLIC wavesim)
target_link_libraries (wavesim_frontend PUBLIC qwtinternal)
target_link_libraries (wavesim_frontend PUBLIC Qt5::Core)
target_link_libraries (wavesim_frontend PUBLIC Qt5::Gui)
target_link_libraries (wavesim_frontend PUBLIC Qt5::Widgets)
