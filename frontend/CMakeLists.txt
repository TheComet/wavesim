option (WAVESIM_FRONTEND "Whether to build the GUI front-end or not" ON)

if (NOT WAVESIM_FRONTEND)
    return ()
endif ()

project ("wavesim GUI"
    VERSION 0.0.1
    LANGUAGES CXX)

add_subdirectory ("../thirdparty/qwt-6.1.3" "${CMAKE_CURRENT_BINARY_DIR}/qwt-6.1.3")
add_subdirectory ("../thirdparty/assimp" "${CMAKE_CURRENT_BINARY_DIR}/assimp")
add_subdirectory ("../thirdparty/bgfx.cmake" "${CMAKE_CURRENT_BINARY_DIR}/bgfx.cmake")

find_package (Qt53DCore REQUIRED)
find_package (Qt53DExtras REQUIRED)
find_package (Qt53DRender REQUIRED)
find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)

file (GLOB FRONTEND_SOURCES
    "src/*.cpp"
    "src/models/*.cpp"
    "src/views/*.cpp")
file (GLOB FRONTEND_HEADERS
    "include/frontend/models/*.hpp"
    "include/frontend/views/*.hpp")
set (FRONTEND_HEADERS_MOC)

qt5_wrap_cpp (FRONTEND_SOURCES_MOC
    "include/frontend/views/MainWindow.hpp"
    "include/frontend/views/SceneView.hpp")
qt5_wrap_ui (FRONTEND_SOURCES_UI
    "forms/MainWindow.ui")

add_executable (wavesim_frontend
    ${FRONTEND_HEADERS}
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
target_link_libraries (wavesim_frontend PUBLIC assimp::assimp)
target_link_libraries (wavesim_frontend PUBLIC bgfx)
target_link_libraries (wavesim_frontend PUBLIC Qt5::3DCore)
target_link_libraries (wavesim_frontend PUBLIC Qt5::3DExtras)
target_link_libraries (wavesim_frontend PUBLIC Qt5::3DRender)
target_link_libraries (wavesim_frontend PUBLIC Qt5::Core)
target_link_libraries (wavesim_frontend PUBLIC Qt5::Gui)
target_link_libraries (wavesim_frontend PUBLIC Qt5::Widgets)

###############################################################################
# DLLs need to be copied on Windows
###############################################################################

if (WIN32)
    macro (copy_dlls CONFIG)
        if (${CONFIG} MATCHES Debug)
            set (POSTFIX d)
        else ()
            set (POSTFIX "")
        endif ()
        set (BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG})
        add_custom_target (Qt5CopyBinaries${CONFIG}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../plugins/platforms/qwindowsd.dll   ${BIN_DIR}/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt53DCore${POSTFIX}.dll       ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt53DExtras${POSTFIX}.dll     ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt53DInput${POSTFIX}.dll      ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt53DLogic${POSTFIX}.dll      ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt53DRender${POSTFIX}.dll     ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Concurrent${POSTFIX}.dll   ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Core${POSTFIX}.dll         ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Gamepad${POSTFIX}.dll      ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Gui${POSTFIX}.dll          ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Network${POSTFIX}.dll      ${BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgets${POSTFIX}.dll      ${BIN_DIR}

            COMMENT "Copying Qt binaries from '${Qt5Core_DIR}/../../bin/' to '${BIN_DIR}'"
            VERBATIM
        )
        add_dependencies (wavesim_frontend Qt5CopyBinaries${CONFIG})
    endmacro ()

    copy_dlls(Debug)
    copy_dlls(Release)
endif ()

