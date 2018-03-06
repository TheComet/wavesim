###############################################################################
# Project name
###############################################################################

project ("wavesimlib"
    VERSION 0.0.1
    LANGUAGES C CXX)

set (CMAKE_CXX_STANDARD 11)

include (CheckIncludeFiles)
include (CheckCSourceCompiles)
include (CMakeDependentOption)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

###############################################################################
# Library options and config
###############################################################################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set (DEBUG_FEATURES 1)
else ()
    set (DEBUG_FEATURES 0)
endif ()

set (PROJECT_NAME "WAVESIM")  # Required for export.h.in
option (WAVESIM_64BIT_INDEX_BUFFERS "64-bit index buffer support. If you know you won't be dealing with scenes that have more than 2^32 vertices in total, then disable this feature to save memory." off)
set (WAVESIM_BUILD_TYPE "SHARED" CACHE STRING "Whether to build as shared or static")
option (WAVESIM_MEMORY_BACKTRACE "Generates backtraces to each malloc call that doesn't get matched to a free. Impacts performance, but can help track down memory leaks easily." ${DEBUG_FEATURES})
option (WAVESIM_MEMORY_DEBUGGING "Enables malloc/free wrappers and memory usage reports" ${DEBUG_FEATURES})
option (WAVESIM_PIC "Position independent code when building as a static library" ON)
set (WAVESIM_PRECISION "double" CACHE STRING "The datatype to use for all calculations (float, double or long double)")
option (WAVESIM_PROFILING "Compiles with -pg on linux" OFF)
option (WAVESIM_PYTHON "Build python bindings. The library must be shared." OFF)
option (WAVESIM_TESTS "Whether or not to build unit tests (note: requires C++)" ${DEBUG_FEATURES})
option (WAVESIM_USE_SYSTEM_FFTW "Forces wavesim to use the system fftw library. If it is not present, an error is thrown." OFF)
option (WAVESIM_PREFER_SYSTEM_FFTW "If fftw is installed on the host system, wavesim will prefer using it over building fftw internally. If this option is disabled, then the internal fftw library is always built." OFF)

string (REPLACE " " "_" WS_PRECISION_CAPS_AND_NO_SPACES ${WAVESIM_PRECISION})
string (TOUPPER ${WS_PRECISION_CAPS_AND_NO_SPACES} WS_PRECISION_CAPS_AND_NO_SPACES)

# Determine visibility macros if the library is a shared library.
if (WAVESIM_BUILD_TYPE MATCHES "SHARED")
    if (CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
        if (CYGWIN)
            set (WAVESIM_HELPER_API_EXPORT "__declspec(dllexport)")
            set (WAVESIM_HELPER_API_IMPORT "__declspec(dllimport)")
            set (WAVESIM_HELPER_API_LOCAL  "")
        else ()
            set (WAVESIM_HELPER_API_EXPORT "__attribute__((visibility(\"default\")))")
            set (WAVESIM_HELPER_API_IMPORT "__attribute__((visibility(\"default\")))")
            set (WAVESIM_HELPER_API_LOCAL  "__attribute__((visibility(\"hidden\")))")
        endif ()
    elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
        set (WAVESIM_HELPER_API_EXPORT "__declspec(dllexport)")
        set (WAVESIM_HELPER_API_IMPORT "__declspec(dllimport)")
        set (WAVESIM_HELPER_API_LOCAL  "")
    else ()
        set (WAVESIM_HELPER_API_EXPORT "")
        set (WAVESIM_HELPER_API_IMPORT "")
        set (WAVESIM_HELPER_API_LOCAL  "")
    endif ()
else ()
    set (WAVESIM_HELPER_API_EXPORT "")
    set (WAVESIM_HELPER_API_IMPORT "")
    set (WAVESIM_HELPER_API_LOCAL  "")
endif ()

# Need the restrict keyword for some of the math stuff
foreach (RESTRICT_KEYWORD __restrict__ __restrict restrict)
    check_c_source_compiles ("int test (void *${RESTRICT_KEYWORD} x); int main (void) {return 0;}" WAVESIM_RESTRICT_${RESTRICT_KEYWORD})
    if (WAVESIM_RESTRICT_${RESTRICT_KEYWORD})
        set (WS_RESTRICT ${RESTRICT_KEYWORD})  # This replaces ${WS_RESTRICT} in config.h.in
        break ()
    endif ()
endforeach ()

# Use stdint.h if possible, otherwise use pstdint.h
check_include_files ("stdint.h" WAVESIM_HAVE_STDINT_H)

# Check if we can warn about unused function results
check_c_source_compiles ("int __attribute__((warn_unused_result)) f(int z) { return z*z + z*2 + z/3 + 23; } int main(void) { return f(4); }" HAVE_WARN_UNUSED)
if (HAVE_WARN_UNUSED)
    set (WS_WARN_UNUSED "__attribute__((warn_unused_result))")
endif ()

set (WAVESIM_HOST_COMPUTER ${CMAKE_HOST_SYSTEM})
set (WAVESIM_COMPILER ${CMAKE_C_COMPILER_ID})

find_program (UNAME_PROGRAM uname)
if (UNAME_PROGRAM)
    execute_process (
        COMMAND ${UNAME_PROGRAM} -o -r -m -p -i
        OUTPUT_VARIABLE WAVESIM_HOST_COMPUTER)
    string (REPLACE "\n" "" WAVESIM_HOST_COMPUTER ${WAVESIM_HOST_COMPUTER})
else ()
    set (WAVESIM_HOST_COMPUTER ${CMAKE_HOST_SYSTEM_NAME})
endif ()

# Required for build info
find_package (Git)
if (GIT_FOUND)
    execute_process (COMMAND ${GIT_EXECUTABLE} describe --tags RESULT_VARIABLE RESULT OUTPUT_VARIABLE COMMIT_TAG)
    if (RESULT)
        set (COMMIT_TAG "(unknown tag)")
    else ()
        string (REPLACE "\n" "" COMMIT_TAG ${COMMIT_TAG})
    endif ()
    execute_process (COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD RESULT_VARIABLE RESULT OUTPUT_VARIABLE COMMIT_HASH)
    if (RESULT)
        set (COMMIT_HASH "(commit hash unknown)")
    else ()
        string (REPLACE "\n" "" COMMIT_HASH ${COMMIT_HASH})
    endif ()
    set (WAVESIM_COMMIT_INFO "${COMMIT_TAG} (${COMMIT_HASH})")
else ()
    set (WAVESIM_COMMIT_INFO "unknown (git not found!)")
    message (WARNING "Git not found. Build will not contain git revision info.")
endif ()

# We use FFTW for DCTs
set (FFTW_FOUND "")
if (WAVESIM_PREFER_SYSTEM_FFTW OR WAVESIM_USE_SYSTEM_FFTW)
    set (WAVESIM_FFTW_REQUIRED "")
    if (WAVESIM_USE_SYSTEM_FFTW)
        set (WAVESIM_FFTW_REQUIRED "REQUIRED")
    endif ()
    find_package (FFTW ${WAVESIM_FFTW_REQUIRED})
endif ()
if (NOT FFTW_FOUND)
    add_subdirectory ("../fftw-3.3.7" "${CMAKE_CURRENT_BINARY_DIR}/fftw-3.3.7")
    set (FFTW_LIBRARIES fftw3)
    set (FFTW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../fftw-3.3.7/api")
    set_property (TARGET fftw3 PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})
endif ()

# Need pthread for multithreaded simulations
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set (WAVESIM_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

configure_file ("src/build_info.c.in" "src/build_info.c")
configure_file ("include/intern/wavesim/config.h.in"
                "include/wavesim/config.h")

###############################################################################
# source files and library definitions
###############################################################################

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set (PLATFORM_SOURCE_DIR "osx")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set (PLATFORM_SOURCE_DIR "win32")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set (PLATFORM_SOURCE_DIR "linux")
else ()
    message (FATAL_ERROR "Platform is not supported")
endif ()

###############################################################################
# Wavesim core
###############################################################################

# Headers are required for Visual Studio to list them in the project
file (GLOB WAVESIM_HEADERS "include/extern/wavesim/*.h" "include/intern/wavesim/*.h")
add_library (wavesim_obj OBJECT
    "src/aabb.c"
    "src/attribute.c"
    "src/audio_source.c"
    "src/audio_listener.c"
    "src/btree.c"
    "${CMAKE_CURRENT_BINARY_DIR}/src/build_info.c"
    "src/face.c"
    "src/hash.c"
    "src/intersections.c"
    "src/log.c"
    "src/memory.c"
    "src/mesh.c"
    "src/mesh_builder.c"
    "src/obj_export_common.c"
    "src/obj_export_medium.c"
    "src/obj_export_octree.c"
    "src/obj_import.c"
    "src/octree.c"
    "src/medium.c"
    "src/return_codes.c"
    "src/simulation.c"
    "src/simulation_ard.c"
    "src/simulation_ray.c"
    "src/string.c"
    "src/vec3.c"
    "src/vector.c"
    "src/vertex.c"
    "src/wavesim.c"
    "src/platform/${PLATFORM_SOURCE_DIR}/backtrace.c"
    ${WAVESIM_HEADERS})

set_property (TARGET wavesim_obj PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})

target_include_directories (wavesim_obj
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/extern"
        "${FFTW_INCLUDE_DIRS}")

target_compile_definitions (wavesim_obj
    PRIVATE WAVESIM_BUILDING  # The library is being built
    PRIVATE $<$<CONFIG:Debug>:
        DEBUG
    >
)

target_compile_options (wavesim_obj
    PUBLIC $<$<C_COMPILER_ID:MSVC>:
        /EHa /MTd /W4 /WX /wd4706 /D_CRT_SECURE_NO_DEPRECATE
    >
    PUBLIC $<$<C_COMPILER_ID:GNU>:
        -Wall -Wextra -Werror -pedantic -pedantic-errors -Wshadow -Wconversion
    >
    PUBLIC $<$<C_COMPILER_ID:Clang>:
        -Wall -Wextra -Werror -pedantic -pedantic-errors -Wshadow -Wconversion
    >
)

# Update build info every time the project is compiled
add_custom_target (wavesim_generate_build_info ALL
    COMMAND ${CMAKE_COMMAND} -DWAVESIM_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_timestamp.cmake"
    DEPENDS "cmake/generate_timestamp.cmake")
add_dependencies (wavesim_obj wavesim_generate_build_info)

###############################################################################
# Python bindings
###############################################################################

if (WAVESIM_PYTHON)
    find_package (PythonLibs 3 REQUIRED)
    # Headers are required for Visual Studio to list them in the project
    file (GLOB WAVESIM_PYTHON_HEADERS "include/python/*.h")
    add_library (wavesim_python_obj OBJECT
        "src/python/wavesim_module.c"
        "src/python/wavesim_module_Attribute.c"
        "src/python/wavesim_module_Face.c"
        "src/python/wavesim_module_Mesh.c"
        "src/python/wavesim_module_MeshIterator.c"
        "src/python/wavesim_module_Vertex.c"
        ${WAVESIM_PYTHON_HEADERS})
    set_property (TARGET wavesim_python_obj PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})
    target_include_directories (wavesim_python_obj
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/extern"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/python"
            "${PYTHON_INCLUDE_DIRS}")
    target_compile_options (wavesim_python_obj
        PUBLIC $<$<C_COMPILER_ID:MSVC>:
            /EHa /MTd /W4 /WX /wd4115 /D_CRT_SECURE_NO_DEPRECATE
        >
        PUBLIC $<$<C_COMPILER_ID:GNU>:
            -Wall -Wextra -Werror -pedantic -pedantic-errors -Wno-missing-field-initializers -Wshadow
        >
        PUBLIC $<$<C_COMPILER_ID:Clang>:
            -Wall -Wextra -Werror -pedantic -pedantic-errors -Wno-missing-field-initializers -Wshadow
        >
    )
    target_compile_definitions (wavesim_python_obj
        PRIVATE WAVESIM_BUILDING  # The library is being built
        PRIVATE $<$<CONFIG:Debug>:
            DEBUG
        >
    )
endif ()

###############################################################################
# Unit tests
###############################################################################

if (WAVESIM_TESTS)
    add_library (wavesim_tests_obj OBJECT
        "googletest/src/gtest-all.cc"
        "googlemock/src/gmock-all.cc"
        "tests/environment_util_global.cpp"
        "tests/utils.cpp"
        "tests/api.cpp"
        "tests/test_aabb.cpp"
        "tests/test_attribute.cpp"
        "tests/test_btree.cpp"
        "tests/test_face.cpp"
        "tests/test_intersections.cpp"
        "tests/test_mesh.cpp"
        "tests/test_mesh_builder.cpp"
        "tests/test_obj_import.cpp"
        "tests/test_octree.cpp"
        "tests/test_medium.cpp"
        "tests/test_simulation.cpp"
        "tests/test_string.cpp"
        "tests/test_vec3.cpp"
        "tests/test_vector.cpp"
        "tests/test_vertex.cpp"
        $<$<BOOL:${WAVESIM_PYTHON}>:
            ${CMAKE_CURRENT_BINARY_DIR}/tests/python/test_python_bindings.cpp>)
    if (WAVESIM_PYTHON)
        file (GLOB PYTHON_TEST_SOURCE_FILES "tests/python/*.py")
        set (GENERATE_PYTHON_TESTS_CMD ${CMAKE_COMMAND}
            -DWAVESIM_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DWAVESIM_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_python_unit_tests.cmake")
        add_custom_command (OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/tests/python/test_python_bindings.cpp"
            COMMAND ${GENERATE_PYTHON_TESTS_CMD}
            DEPENDS
                "cmake/generate_python_unit_tests.cmake"
                "tests/python/test_python_bindings.cpp.in"
                ${PYTHON_TEST_SOURCE_FILES})
        add_custom_target (wavesim_generate_unit_tests ALL
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/tests/python/test_python_bindings.cpp")
        add_dependencies (wavesim_tests_obj wavesim_generate_unit_tests)
    endif ()
    target_include_directories (wavesim_tests_obj
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/googlemock"
            "${CMAKE_CURRENT_SOURCE_DIR}/googlemock/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/googletest"
            "${CMAKE_CURRENT_SOURCE_DIR}/googletest/include"
            $<$<BOOL:${WAVESIM_PYTHON}>:${PYTHON_INCLUDE_DIRS}>)
else ()
    add_library (wavesim_tests_obj OBJECT "tests/api_stub.c")
endif ()

set_property (TARGET wavesim_tests_obj PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})
target_compile_options (wavesim_tests_obj
    PUBLIC $<$<C_COMPILER_ID:MSVC>:
        /MTd /D_CRT_SECURE_NO_DEPRECATE
    >
    PUBLIC $<$<C_COMPILER_ID:GNU>:
        -Wno-unused-result
    >
    PUBLIC $<$<C_COMPILER_ID:Clang>:
        -Wno-unused-result
    >
)
target_include_directories (wavesim_tests_obj
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/extern")
target_compile_definitions (wavesim_tests_obj
    PRIVATE WAVESIM_BUILDING  # The library is being built
    PRIVATE $<$<CONFIG:Debug>:
        DEBUG
    >
)

###############################################################################
# The actual wavesim library
###############################################################################

add_library (wavesim ${WAVESIM_BUILD_TYPE}
    $<TARGET_OBJECTS:wavesim_obj>
    $<$<BOOL:${WAVESIM_TESTS}>:$<TARGET_OBJECTS:wavesim_tests_obj>>
    $<$<BOOL:${WAVESIM_PYTHON}>:$<TARGET_OBJECTS:wavesim_python_obj>>)

set_target_properties (wavesim PROPERTIES
    PREFIX ""
    DEBUG_POSTFIX ""
    SOVERSION ${PROJECT_VERSION})
target_include_directories (wavesim
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/extern>
        $<INSTALL_INTERFACE:include>)
target_link_libraries (wavesim PRIVATE ${FFTW_LIBRARIES})
target_link_libraries(wavesim PRIVATE Threads::Threads)

if (WAVESIM_TESTS)
    add_executable (wavesim_tests "tests/app.c")
    target_link_libraries (wavesim_tests wavesim)
    set_target_properties (wavesim_tests PROPERTIES
        INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    install (TARGETS wavesim_tests
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

if (WAVESIM_PYTHON)
    target_link_libraries (wavesim PRIVATE ${PYTHON_LIBRARIES})
endif()

install (TARGETS wavesim
    EXPORT WaveSimConfig
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install (DIRECTORY "include/extern/wavesim" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/wavesim" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install (EXPORT WaveSimConfig DESTINATION "share/wavesim/cmake")
export (TARGETS wavesim FILE WaveSimConfig.cmake)

###############################################################################
# Dependency settings
###############################################################################

message (STATUS "------------------------------------------------------------")
message (STATUS "Wavesim settings")
message (STATUS " + Configuration: ${CMAKE_BUILD_TYPE}")
message (STATUS " + Build type: ${WAVESIM_BUILD_TYPE}")
message (STATUS " + PIC (Position independent code): ${WAVESIM_PIC}")
message (STATUS " + Precision: ${WAVESIM_PRECISION}")
message (STATUS " + Memory debugging: ${WAVESIM_MEMORY_DEBUGGING}")
message (STATUS " + Memory backtraces: ${WAVESIM_MEMORY_BACKTRACE}")
message (STATUS " + Profiling: ${WAVESIM_PROFILING}")
message (STATUS " + Unit Tests: ${WAVESIM_TESTS}")
message (STATUS " + Python bindings: ${WAVESIM_PYTHON}")
message (STATUS "------------------------------------------------------------")
