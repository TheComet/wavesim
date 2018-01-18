###############################################################################
# Project name
###############################################################################

project ("wavesimlib"
    VERSION 0.0.1
    LANGUAGES C CXX)

include (CheckIncludeFiles)
include (CheckCSourceCompiles)

###############################################################################
# Library options and config
###############################################################################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set (MEM_DBG_DEFAULT 1)
else ()
    set (MEM_DBG_DEFAULT 0)
endif ()

set (PROJECT_NAME "WAVESIM")  # Required for export.h.in
set (WAVESIM_64BIT_INDEX_BUFFERS 0 CACHE STRING "64-bit index buffer support. If you know you won't be dealing with scenes that have more than 2^32 vertices in total, then disable this feature to save memory.")
set (WAVESIM_BUILD_TYPE "SHARED" CACHE STRING "Whether to build as shared or static")
set (WAVESIM_MEMORY_DEBUGGING ${MEM_DBG_DEFAULT} CACHE STRING "Enables malloc/free wrappers and memory reports")
set (WAVESIM_MEMORY_BACKTRACE ${WAVESIM_MEMORY_DEBUGGING} CACHE STRING "Enables backtraces for malloc/free mismatches. Impacts performance on malloc, obviously.")
set (WAVESIM_PYTHON 1 CACHE STRING "Build python bindings. The library must be shared.")
set (WAVESIM_PIC 0 CACHE STRING "Position independent code when building as a static library")
set (WAVESIM_PRECISION "double" CACHE STRING "The datatype to use for all calculations (float, double or long double)")
set (WAVESIM_PROFILING 0 CACHE STRING "Compiles with -pg on linux")
set (WAVESIM_TESTS 0 CACHE STRING "Whether or not to build unit tests (note: requires C++)")

# Determine visibility macros if the library is a shared library.
if (WAVESIM_BUILD_TYPE MATCHES "SHARED")
    if (${CMAKE_C_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_C_COMPILER_ID} MATCHES "GNU")
        if (CYGWIN)
            set (WAVESIM_HELPER_API_EXPORT "__declspec(dllexport)")
            set (WAVESIM_HELPER_API_IMPORT "__declspec(dllimport)")
            set (WAVESIM_HELPER_API_LOCAL  "")
        else ()
            set (WAVESIM_HELPER_API_EXPORT "__attribute__((visibility(\"default\")))")
            set (WAVESIM_HELPER_API_IMPORT "__attribute__((visibility(\"default\")))")
            set (WAVESIM_HELPER_API_LOCAL  "__attribute__((visibility(\"hidden\")))")
        endif ()
    elseif (${CMAKE_C_COMPILER_ID} MATCHES "MSVC")
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
        set (RESTRICT ${RESTRICT_KEYWORD})  # This replaces ${RESTRICT} in config.h.in
        break ()
    endif ()
endforeach ()

# Use stdint.h if possible, otherwise use pstdint.h
check_include_files ("stdint.h" WAVESIM_HAVE_STDINT_H)

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

find_package (Git)
if (GIT_FOUND)
    execute_process (COMMAND ${GIT_EXECUTABLE} describe --tags RESULT_VARIABLE RESULT OUTPUT_VARIABLE RESULT2)
    if (NOT ${RESULT} EQUAL 0)
        set (RESULT2 "(git commit id unknown)")
    else ()
        string (REPLACE "\n" "" RESULT2 ${RESULT2})
    endif ()
    execute_process (COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD OUTPUT_VARIABLE RESULT3)
    string (REPLACE "\n" "" RESULT3 ${RESULT3})
    set (WAVESIM_COMMIT_INFO "${RESULT2} (${RESULT3})")
else ()
    set (WAVESIM_COMMIT_INFO "unknown (git not found!)")
    message (WARNING "Git not found. Build will not contain git revision info.")
endif ()

set (WAVESIM_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

configure_file ("src/build_info.c.in" "src/build_info.c")
configure_file ("include/intern/wavesim/export.h.in"
                "include/wavesim/export.h")
configure_file ("include/intern/wavesim/config.h.in"
                "include/wavesim/config.h")

###############################################################################
# source files and library definitionvector_count(&octree->faces)
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
# Unit test library
###############################################################################

add_library (wavesim_obj OBJECT
    "src/aabb.c"
    "src/attribute.c"
    "src/btree.c"
    "${CMAKE_CURRENT_BINARY_DIR}/src/build_info.c"
    "src/face.c"
    "src/intersections.c"
    "src/memory.c"
    "src/mesh.c"
    "src/mesh_builder.c"
    "src/octree.c"
    "src/partition.c"
    "src/simulation.c"
    "src/vec3.c"
    "src/vector.c"
    "src/vertex.c"
    "src/wavesim.c"
    "src/platform/${PLATFORM_SOURCE_DIR}/backtrace.c")

set_property(TARGET wavesim_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories (wavesim_obj
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/extern")

# The library is being built
target_compile_definitions (wavesim_obj PRIVATE WAVESIM_BUILDING)

target_compile_options (wavesim_obj
    PUBLIC $<$<C_COMPILER_ID:MSVC>:
        /EHa /MTd /W4 /WX /D_CRT_SECURE_NO_DEPRECATE
    >
    PUBLIC $<$<C_COMPILER_ID:GNU>:
        -Wall -Wextra -Werror -pedantic -pedantic-errors
    >
    PUBLIC $<$<C_COMPILER_ID:Clang>:
        -Wall -Wextra -Werror -pedantic -pedantic-errors
    >
)

###############################################################################
# Python bindings
###############################################################################

if (${WAVESIM_PYTHON})
    find_package (PythonLibs 3 REQUIRED)
    add_library (wavesim_python_obj OBJECT
        "python/src/wavesim_module.c"
        "python/src/wavesim_module_Attribute.c"
        "python/src/wavesim_module_Face.c"
        "python/src/wavesim_module_Mesh.c"
        "python/src/wavesim_module_MeshIterator.c"
        "python/src/wavesim_module_Vertex.c")
    set_property(TARGET wavesim_python_obj PROPERTY POSITION_INDEPENDENT_CODE 1)
    target_include_directories (wavesim_python_obj
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/extern"
            "${CMAKE_CURRENT_SOURCE_DIR}/python/include"
            "${PYTHON_INCLUDE_DIRS}")
    target_compile_options (wavesim_python_obj
        PUBLIC $<$<C_COMPILER_ID:MSVC>:
            /EHa /MTd /W4 /WX /D_CRT_SECURE_NO_DEPRECATE
        >
        PUBLIC $<$<C_COMPILER_ID:GNU>:
            -Wall -Wextra -Werror -pedantic -pedantic-errors -Wno-missing-field-initializers
        >
        PUBLIC $<$<C_COMPILER_ID:Clang>:
            -Wall -Wextra -Werror -pedantic -pedantic-errors -Wno-missing-field-initializers
        >
    )
endif ()

###############################################################################
# Unit tests
###############################################################################

set (TEST_SOURCES "tests/api_stub.c")
if (${WAVESIM_TESTS})
    add_subdirectory ("../googletest" "${CMAKE_CURRENT_BINARY_DIR}/googletest")
    add_library (wavesim_tests_obj OBJECT
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
        "tests/test_octree.cpp"
        "tests/test_partition.cpp"
        "tests/test_vec3.cpp"
        "tests/test_vector.cpp"
        "tests/test_vertex.cpp"
        $<${WAVESIM_PYTHON}:${CMAKE_CURRENT_BINARY_DIR}/python/tests/test_python_bindings.cpp>)
    if (${WAVESIM_PYTHON})
        file (GLOB PYTHON_TEST_SOURCE_FILES "python/tests/*.py")
        set (GENERATE_PYTHON_TESTS_CMD ${CMAKE_COMMAND}
            -DWAVESIM_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DWAVESIM_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_python_unit_tests.cmake")
        add_custom_command (OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/python/tests/test_python_bindings.cpp"
            COMMAND ${GENERATE_PYTHON_TESTS_CMD}
            DEPENDS
                "generate_python_unit_tests.cmake"
                "python/tests/test_python_bindings.cpp.in"
                ${PYTHON_TEST_SOURCE_FILES})
        add_custom_target (wavesim_generate_unit_tests ALL
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/python/tests/test_python_bindings.cpp")
        add_dependencies (wavesim_tests_obj wavesim_generate_unit_tests)
    endif ()
    set_property(TARGET wavesim_tests_obj PROPERTY POSITION_INDEPENDENT_CODE 1)
    target_include_directories (wavesim_tests_obj
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/extern"
            "${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googlemock/include/"
            "${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googletest/include/"
            $<${WAVESIM_PYTHON}:${PYTHON_INCLUDE_DIRS}>)
endif ()

###############################################################################
# The actual wavesim library
###############################################################################

add_library (wavesim ${WAVESIM_BUILD_TYPE}
    $<TARGET_OBJECTS:wavesim_obj>
    $<${WAVESIM_TESTS}:$<TARGET_OBJECTS:wavesim_tests_obj>>
    $<${WAVESIM_PYTHON}:$<TARGET_OBJECTS:wavesim_python_obj>>)

set_target_properties (wavesim PROPERTIES
    PREFIX ""
    DEBUG_POSTFIX ""
    SOVERSION ${PROJECT_VERSION})
target_include_directories (wavesim
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/extern>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_custom_target (wavesim_timestamp
    COMMAND ${CMAKE_COMMAND} -DWAVESIM_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_timestamp.cmake"
    ADD_DEPENDENCIES "generate_timestamp.cmake")
add_dependencies (wavesim wavesim_timestamp)

if (${WAVESIM_TESTS})
    target_link_libraries (wavesim PRIVATE gmock)
    add_executable (wavesim_tests "tests/app.c")
    target_link_libraries (wavesim_tests wavesim)
    set_target_properties (wavesim_tests PROPERTIES
        INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    install (TARGETS wavesim_tests
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

if (${WAVESIM_PYTHON})
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
message (STATUS " + Build type: ${WAVESIM_BUILD_TYPE}")
message (STATUS " + PIC (Position independent code): ${WAVESIM_PIC}")
message (STATUS " + Precision: ${WAVESIM_PRECISION}")
message (STATUS " + Memory debugging: ${WAVESIM_MEMORY_DEBUGGING}")
message (STATUS " + Memory backtraces: ${WAVESIM_MEMORY_BACKTRACE}")
message (STATUS " + Profiling: ${WAVESIM_PROFILING}")
message (STATUS " + Unit Tests: ${WAVESIM_TESTS}")
message (STATUS " + Python bindings: ${WAVESIM_PYTHON}")
message (STATUS "------------------------------------------------------------")
