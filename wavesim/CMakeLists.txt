###############################################################################
# Wavesim cmake script
###############################################################################

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

include (CheckIncludeFiles)
include (CheckCSourceCompiles)
include (CMakeDependentOption)
include (TestVisibilityMacros)

###############################################################################
# Library options and config
###############################################################################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set (DEBUG_FEATURE 1)
    set (RELEASE_FEATURE 0)
else ()
    set (DEBUG_FEATURE 0)
    set (RELEASE_FEATURE 1)
endif ()

option (WAVESIM_64BIT_INDEX_BUFFERS "64-bit index buffer support. If you know you won't be dealing with scenes that have more than 2^32 vertices in total, then disable this feature to save memory." off)
set (WAVESIM_LIB_TYPE "SHARED" CACHE STRING "Whether to build as shared or static")
cmake_dependent_option (WAVESIM_BENCHMARKS "Compile benchmarks (requires C++)" OFF "CMAKE_BUILD_TYPE MATCHES Release" OFF)
option (WAVESIM_UPDATE_BUILD_INFO_ON_REBUILD "Whether to generate buildinfo.c every time you hit compile" ${RELEASE_FEATURE})
option (WAVESIM_MEMORY_BACKTRACE "Generates backtraces to each malloc call that doesn't get matched to a free. Impacts performance, but can help track down memory leaks easily." ${DEBUG_FEATURE})
option (WAVESIM_MEMORY_DEBUGGING "Enables malloc/free wrappers and memory usage reports" ${DEBUG_FEATURE})
option (WAVESIM_PIC "Position independent code when building as a static library" ON)
set (WAVESIM_PRECISION "double" CACHE STRING "The datatype to use for all calculations (float, double or long double)")
option (WAVESIM_PROFILING "Compiles with -pg on linux" OFF)
option (WAVESIM_PYTHON "Build python bindings. The library must be shared." OFF)
option (WAVESIM_TESTS "Whether or not to build unit tests (note: requires C++)" ${DEBUG_FEATURE})
option (WAVESIM_UPDATE_BUILD_INFO "Enabling this causes build_info_dynamic.h to be generated every compile, meaning build_info.c has to be compiles every time." ${RELEASE_FEATURE})
option (WAVESIM_USE_SYSTEM_FFTW "Forces wavesim to use the system fftw library. If it is not present, an error is thrown." OFF)
option (WAVESIM_PREFER_SYSTEM_FFTW "If fftw is installed on the host system, wavesim will prefer using it over building fftw internally. If this option is disabled, then the internal fftw library is always built." OFF)

if (WAVESIM_BENCHMARKS OR WAVESIM_TESTS)
    set (NEED_CXX_LANGUAGE "CXX")
endif ()

project ("wavesimlib"
    VERSION 0.0.1
    LANGUAGES C ${NEED_CXX_LANGUAGE})

string (REPLACE " " "_" WAVESIM_PRECISION_CAPS_AND_NO_SPACES ${WAVESIM_PRECISION})
string (TOUPPER ${WAVESIM_PRECISION_CAPS_AND_NO_SPACES} WAVESIM_PRECISION_CAPS_AND_NO_SPACES)

test_visibility_macros (WAVESIM_HELPER_API_EXPORT WAVESIM_HELPER_API_IMPORT WAVESIM_HELPER_API_LOCAL)

# Need the restrict keyword for some of the math stuff
foreach (RESTRICT_KEYWORD __restrict__ __restrict restrict)
    check_c_source_compiles ("int test (void *${RESTRICT_KEYWORD} x); int main (void) {return 0;}" WAVESIM_RESTRICT_${RESTRICT_KEYWORD})
    if (WAVESIM_RESTRICT_${RESTRICT_KEYWORD})
        set (WAVESIM_RESTRICT ${RESTRICT_KEYWORD})  # This replaces ${WAVESIM_RESTRICT} in config.h.in
        break ()
    endif ()
endforeach ()

# Use stdint.h if possible, otherwise use pstdint.h
check_include_files ("stdint.h" WAVESIM_HAVE_STDINT_H)

# Check if we can warn about unused function results
check_c_source_compiles ("int __attribute__((warn_unused_result)) f(int z) { return z*z + z*2 + z/3 + 23; } int main(void) { return f(4); }" HAVE_WARN_UNUSED)
if (HAVE_WARN_UNUSED)
    set (WAVESIM_WARN_UNUSED "__attribute__((warn_unused_result))")
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
if (WAVESIM_PREFER_SYSTEM_FFTW OR WAVESIM_USE_SYSTEM_FFTW)
    set (WAVESIM_FFTW_REQUIRED "")
    if (WAVESIM_USE_SYSTEM_FFTW)
        set (WAVESIM_FFTW_REQUIRED "REQUIRED")
    endif ()
    find_package (FFTW ${WAVESIM_FFTW_REQUIRED})
endif ()
if (NOT FFTW_FOUND AND NOT WAVESIM_USE_SYSTEM_FFTW)
    add_subdirectory ("../thirdparty/fftw-3.3.7" "${CMAKE_CURRENT_BINARY_DIR}/fftw-3.3.7")
    set (FFTW_LIBRARIES fftw3)
    set (FFTW_INCLUDE_DIRS "../thirdparty/fftw-3.3.7/api")
    set_property (TARGET fftw3 PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})
endif ()

# Need pthread for multithreaded simulations
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

set (WAVESIM_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

message (STATUS "------------------------------------------------------------")
message (STATUS "Wavesim settings")
message (STATUS " + Configuration: ${CMAKE_BUILD_TYPE}")
message (STATUS " + Library type: ${WAVESIM_LIB_TYPE}")
message (STATUS " + 64-bit index buffers: ${WAVESIM_64BIT_INDEX_BUFFERS}")
message (STATUS " + Build benchmark tests: ${WAVESIM_BENCHMARKS}")
message (STATUS " + WAVESIM_UPDATE_BUILD_INFO_ON_REBUILD: ${WAVESIM_UPDATE_BUILD_INFO_ON_REBUILD}")
message (STATUS " + Memory debugging: ${WAVESIM_MEMORY_DEBUGGING}")
message (STATUS " + Memory backtraces: ${WAVESIM_MEMORY_BACKTRACE}")
message (STATUS " + PIC (Position independent code): ${WAVESIM_PIC}")
message (STATUS " + Precision: ${WAVESIM_PRECISION}")
message (STATUS " + Profiling: ${WAVESIM_PROFILING}")
message (STATUS " + Python bindings: ${WAVESIM_PYTHON}")
message (STATUS " + Unit Tests: ${WAVESIM_TESTS}")
message (STATUS "------------------------------------------------------------")

configure_file ("cmake/templates/build_info.c.in" "src/build_info.c")
configure_file ("cmake/templates/config.h.in"
                "include/wavesim/config.h")

###############################################################################
# Wavesim core
###############################################################################

file (GLOB WAVESIM_HEADERS "common/include/wavesim/*.h")
file (GLOB WAVESIM_SOURCES "common/src/*.c")
file (GLOB WAVESIM_HEADERS_MESH "mesh/include/wavesim/mesh/*.h")
file (GLOB WAVESIM_SOURCES_MESH "mesh/src/*.c")
file (GLOB WAVESIM_HEADERS_SIM "simulation/include/wavesim/simulation/*.h")
file (GLOB WAVESIM_SOURCES_SIM "simulation/src/*.c")
if (WIN32)
    file (GLOB WAVESIM_PLATFORM_SOURCES "common/src/platform/win32/*.c")
elseif (UNIX)
    file (GLOB WAVESIM_PLATFORM_SOURCES "common/src/platform/unix/*.c")
else ()
    message (FATAL_ERROR "No sources for this platform")
endif ()
add_library (wavesim_obj OBJECT
    ${WAVESIM_HEADERS}
    ${WAVESIM_SOURCES}
    ${WAVESIM_HEADERS_MESH}
    ${WAVESIM_SOURCES_MESH}
    ${WAVESIM_HEADERS_SIM}
    ${WAVESIM_SOURCES_SIM}
    ${WAVESIM_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/src/build_info.c)
target_include_directories (wavesim_obj
    PRIVATE
        "common/include"
        "mesh/include"
        "simulation/include"
        ${FFTW_INCLUDE_DIRS}
        "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_definitions (wavesim_obj
    PRIVATE
        WAVESIM_BUILDING)
target_compile_options (wavesim_obj
    PUBLIC $<$<C_COMPILER_ID:MSVC>:
        /EHa /MTd /W4 /WX /wd4706 /D_CRT_SECURE_NO_DEPRECATE
    >
    PUBLIC $<$<C_COMPILER_ID:GNU>:
        -W -Wall -Wextra -Werror -pedantic -pedantic-errors -Wshadow -Wconversion
    >
    PUBLIC $<$<C_COMPILER_ID:Clang>:
        -W -Wall -Wextra -Werror -pedantic -pedantic-errors -Wshadow -Wconversion
    >
)
set_property (
    TARGET wavesim_obj
    PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})

# Update build info every time the project is compiled
add_custom_target (wavesim_generate_build_info ALL
    COMMAND ${CMAKE_COMMAND}
		-DWAVESIM_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
		-DWAVESIM_UPDATE_BUILD_INFO=${WAVESIM_UPDATE_BUILD_INFO}
		-P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/generate_timestamp.cmake"
    DEPENDS "cmake/modules/generate_timestamp.cmake")
add_dependencies (wavesim_obj wavesim_generate_build_info)
# Have to generate the build info header at least once
if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/include/wavesim/build_info_dynamic.h)
	execute_process (COMMAND ${CMAKE_COMMAND}
		-DWAVESIM_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
		-DWAVESIM_UPDATE_BUILD_INFO=ON
		-P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/generate_timestamp.cmake")
endif ()

###############################################################################
# Python bindings
###############################################################################

if (WAVESIM_PYTHON)
    find_package (PythonLibs 3 REQUIRED)
    file (GLOB WAVESIM_HEADERS_PYTHON "python/include/wavesim/python/*.h")
    file (GLOB WAVESIM_SOURCES_PYTHON "python/src/*.c")
    add_library (wavesim_python_obj OBJECT
        ${WAVESIM_HEADERS_PYTHON}
        ${WAVESIM_SOURCES_PYTHON})
    set_property (
        TARGET wavesim_python_obj
        PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})
    target_include_directories (wavesim_python_obj
        PRIVATE
            "common/include"
            "mesh/include"
            "simulation/include"
            "python/include"
            "${CMAKE_CURRENT_BINARY_DIR}/include"
            ${PYTHON_INCLUDE_DIRS})
    target_compile_definitions (wavesim_python_obj
        PRIVATE WAVESIM_BUILDING)
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
endif ()

###############################################################################
# Unit tests
###############################################################################

if (WAVESIM_TESTS)
    file (GLOB WAVESIM_SOURCES_TESTS "tests/src/*.cpp")
    add_library (wavesim_tests_obj OBJECT
        ${WAVESIM_SOURCES_TESTS}
        "tests/run/wavesim_tests_api.cpp"
        $<$<BOOL:${WAVESIM_PYTHON}>:${CMAKE_CURRENT_BINARY_DIR}/tests/src/test_python_bindings.cpp>)
    target_include_directories (wavesim_tests_obj
        PRIVATE
            tests/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/googletest/googlemock/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/googletest/googletest/include
            $<$<BOOL:${WAVESIM_PYTHON}>:${CMAKE_CURRENT_SOURCE_DIR}/python/include>
            $<$<BOOL:${WAVESIM_PYTHON}>:${PYTHON_INCLUDE_DIRS}>)

    if (WAVESIM_PYTHON)
        file (GLOB PYTHON_TEST_SOURCE_FILES "tests/python/*.py")
        set (GENERATE_PYTHON_TESTS_CMD ${CMAKE_COMMAND}
            -DWAVESIM_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DWAVESIM_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_python_unit_tests.cmake")
        add_custom_command (OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/tests/src/test_python_bindings.cpp"
            COMMAND ${GENERATE_PYTHON_TESTS_CMD}
            DEPENDS
                "cmake/generate_python_unit_tests.cmake"
                "cmake/templates/test_python_bindings.cpp.in"
                ${PYTHON_TEST_SOURCE_FILES})
        add_custom_target (wavesim_generate_unit_tests ALL
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/tests/src/test_python_bindings.cpp")
        add_dependencies (wavesim_tests_obj wavesim_generate_unit_tests)
    endif ()
else ()
    add_library (wavesim_tests_obj OBJECT "tests/run/wavesim_tests_api_stub.c")
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
        "common/include"
        "mesh/include"
        "simulation/include"
        "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_definitions (wavesim_tests_obj
    PRIVATE WAVESIM_BUILDING)

###############################################################################
# The actual wavesim library
###############################################################################

add_library (wavesim ${WAVESIM_LIB_TYPE}
    $<TARGET_OBJECTS:wavesim_obj>
    $<TARGET_OBJECTS:wavesim_tests_obj>
    $<$<BOOL:${WAVESIM_PYTHON}>:$<TARGET_OBJECTS:wavesim_python_obj>>)

set_target_properties (wavesim PROPERTIES
    PREFIX ""
    DEBUG_POSTFIX ""
    SOVERSION ${PROJECT_VERSION})
target_include_directories (wavesim
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mesh/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/simulation/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries (wavesim PRIVATE ${FFTW_LIBRARIES})
target_link_libraries (wavesim PRIVATE Threads::Threads)

if (WAVESIM_TESTS)
    add_executable (wavesim_tests "tests/run/wavesim_tests_main.c")
    set_property (TARGET gtest PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})
    set_property (TARGET gmock PROPERTY POSITION_INDEPENDENT_CODE ${WAVESIM_PIC})
    target_link_libraries (wavesim PRIVATE gmock)
    target_link_libraries (wavesim_tests PRIVATE wavesim)
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
install (DIRECTORY "common/include" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/wavesim" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install (EXPORT WaveSimConfig DESTINATION "share/wavesim/cmake")
export (TARGETS wavesim FILE WaveSimConfig.cmake)
