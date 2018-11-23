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

set (SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)

function (bgfx_shader SHADERS_OUT SHADERS_IN)
    foreach (IN_FILE ${SHADERS_IN})
        get_filename_component (FILE_NAME ${IN_FILE} NAME_WE)
        get_filename_component (IN_DIR ${IN_FILE} DIRECTORY)
        get_filename_component (FILE_EXTENSION ${IN_FILE} EXT)

        set (VARYING_FILE ${IN_DIR}/${FILE_NAME}.vdef)
        if (MSVC)
            set (PLATFORMS dx9 dx11 glsl asm.js)
        elseif (APPLE)
            set (PLATFORMS metal glsl gles asm.js)
        else ()
            set (PLATFORMS glsl gles asm.js)
        endif ()

        if (${FILE_EXTENSION} MATCHES ".vs")
            set (TYPE vertex)
        else ()
            set (TYPE fragment)
        endif ()

        foreach (PLATFORM ${PLATFORMS})
            set (OUT_DIR ${SHADER_OUTPUT_DIR}/${PLATFORM})
            set (OUT_FILE_NAME ${FILE_NAME}${FILE_EXTENSION}.bin)
            set (OUT_FILE ${OUT_DIR}/${OUT_FILE_NAME})
            add_custom_command (OUTPUT ${OUT_FILE}
                DEPENDS ${IN_FILE} ${VARYING_FILE}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_DIR}
                COMMAND "$<TARGET_FILE:shaderc>" --varyingdef ${VARYING_FILE} --type ${TYPE} --platform ${PLATFORM} -f ${IN_FILE} -o ${OUT_FILE}
                COMMENT "Compiling ${TYPE} shader ${OUT_FILE_NAME}")
            list (APPEND ${SHADERS_OUT} ${OUT_FILE})
        endforeach ()

    endforeach ()
    set (${SHADERS_OUT} ${${SHADERS_OUT}} PARENT_SCOPE)
endfunction ()

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
file (GLOB FRONTEND_FORMS
    "forms/*.ui")
file (GLOB FRONTEND_SHADERS
    "shaders/*.vs"
    "shaders/*.fs")
qt5_wrap_cpp (FRONTEND_SOURCES_MOC
    "include/frontend/models/Camera.hpp"
    "include/frontend/views/MainWindow.hpp"
    "include/frontend/views/SceneView.hpp")
qt5_wrap_ui (FRONTEND_SOURCES_UI
    ${FRONTEND_FORMS})
bgfx_shader (FRONTEND_SHADERS_GEN
    "${FRONTEND_SHADERS}")

add_executable (wavesim_frontend
    ${FRONTEND_HEADERS}
    ${FRONTEND_SOURCES}
    ${FRONTEND_SOURCES_MOC}
    ${FRONTEND_SOURCES_UI}
    ${FRONTEND_SHADERS_GEN})
target_include_directories (wavesim_frontend
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
set_property (TARGET wavesim_frontend
    PROPERTY CXX_STANDARD 14)

target_link_libraries (wavesim_frontend PUBLIC wavesim)
target_link_libraries (wavesim_frontend PUBLIC qwtinternal)
target_link_libraries (wavesim_frontend PUBLIC assimp::assimp)
target_link_libraries (wavesim_frontend PUBLIC bgfx)
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
