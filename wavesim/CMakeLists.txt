###############################################################################
# Project name
###############################################################################

project ("wavesimlib"
    VERSION 0.1
    LANGUAGES C CXX)

include (CheckIncludeFiles)
include (CheckCSourceCompiles)

###############################################################################
# Library options and config
###############################################################################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set (MEM_DBG_DEFAULT ON)
else ()
    set (MEM_DBG_DEFAULT OFF)
endif ()

set (PROJECT_NAME "WAVESIM")  # Required for export.h.in
set (${PROJECT_NAME}_BUILD_TYPE "SHARED" CACHE STRING "Whether to build as shared or static")
option (${PROJECT_NAME}_PROFILING "Compiles with -pg on linux" OFF)
option (${PROJECT_NAME}_TESTS "Whether or not to build unit tests (note: requires C++)" OFF)
option (${PROJECT_NAME}_MEMORY_DEBUGGING "Enables malloc/free wrappers and memory reports" ${MEM_DBG_DEFAULT})
option (${PROJECT_NAME}_PYTHON "Build python bindings. The library must be shared." ON)
option (${PROJECT_NAME}_PIC "Position independent code when building as a static library" OFF)
option (${PROJECT_NAME}_MEMORY_BACKTRACE "Enables backtraces for malloc/free mismatches. Impacts performance on malloc, obviously." ${${PROJECT_NAME}_MEMORY_DEBUGGING})
set (${PROJECT_NAME}_PRECISION "double" CACHE STRING "The datatype to use for all calculations (float, double or long double)")
option (${PROJECT_NAME}_64BIT_INDEX_BUFFERS "64-bit index buffer support. If you know you won't be dealing with scenes that have more than 2^32 vertices in total, then disable this feature to save memory." OFF)

# Determine visibility macros if the library is a shared library.
if (${PROJECT_NAME}_BUILD_TYPE MATCHES "SHARED")
    if (${CMAKE_C_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_C_COMPILER_ID} MATCHES "GNU")
        if (CYGWIN)
            set (${PROJECT_NAME}_HELPER_API_EXPORT "__declspec(dllexport)")
            set (${PROJECT_NAME}_HELPER_API_IMPORT "__declspec(dllimport)")
            set (${PROJECT_NAME}_HELPER_API_LOCAL  "")
        else ()
            set (${PROJECT_NAME}_HELPER_API_EXPORT "__attribute__((visibility(\"default\")))")
            set (${PROJECT_NAME}_HELPER_API_IMPORT "__attribute__((visibility(\"default\")))")
            set (${PROJECT_NAME}_HELPER_API_LOCAL  "__attribute__((visibility(\"hidden\")))")
        endif ()
    elseif (${CMAKE_C_COMPILER_ID} MATCHES "MSVC")
        set (${PROJECT_NAME}_HELPER_API_EXPORT "__declspec(dllexport)")
        set (${PROJECT_NAME}_HELPER_API_IMPORT "__declspec(dllimport)")
        set (${PROJECT_NAME}_HELPER_API_LOCAL  "")
    else ()
        set (${PROJECT_NAME}_HELPER_API_EXPORT "")
        set (${PROJECT_NAME}_HELPER_API_IMPORT "")
        set (${PROJECT_NAME}_HELPER_API_LOCAL  "")
    endif ()
else ()
    set (${PROJECT_NAME}_HELPER_API_EXPORT "")
    set (${PROJECT_NAME}_HELPER_API_IMPORT "")
    set (${PROJECT_NAME}_HELPER_API_LOCAL  "")
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
check_include_files ("stdint.h" ${PROJECT_NAME}_HAVE_STDINT_H)

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

set (TEST_SOURCES "src/test_api_stub.c")
if (${${PROJECT_NAME}_TESTS})
    add_subdirectory ("../googletest" "${CMAKE_CURRENT_BINARY_DIR}/googletest")
    set (TEST_SOURCES
        "tests/environment_util_global.cpp"
        "tests/utils.cpp"
        "tests/test_api.cpp"
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
        "tests/test_vertex.cpp")
    set (TEST_INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googlemock/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googletest/include/")
endif ()

if (${${PROJECT_NAME}_PYTHON})
    find_package (PythonLibs 3 REQUIRED)
    add_library (wavesim_python OBJECT
        "python/src/wavesim_module.c"
        "python/src/wavesim_module_Attribute.c"
        "python/src/wavesim_module_Face.c"
        "python/src/wavesim_module_Mesh.c"
        "python/src/wavesim_module_MeshIterator.c"
        "python/src/wavesim_module_Vertex.c")
    set_property(TARGET wavesim_python PROPERTY POSITION_INDEPENDENT_CODE 1)
    target_include_directories (wavesim_python
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/extern>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
        "${CMAKE_CURRENT_SOURCE_DIR}/python/include"
        "${PYTHON_INCLUDE_DIRS}")
    target_compile_options (wavesim_python
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

add_library (wavesim ${WAVESIM_BUILD_TYPE}
    "src/aabb.c"
    "src/attribute.c"
    "src/btree.c"
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
    "src/platform/${PLATFORM_SOURCE_DIR}/backtrace.c"
    "${TEST_SOURCES}"
    $<TARGET_OBJECTS:wavesim_python>)

set_target_properties (wavesim PROPERTIES PREFIX "")
set_target_properties (wavesim PROPERTIES DEBUG_POSTFIX "")

target_include_directories (wavesim
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/extern>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/intern"
        "${TEST_INCLUDE_DIRS}")

if (${${PROJECT_NAME}_TESTS})
    target_link_libraries (wavesim PRIVATE gmock)
    add_executable (test_wavesim "tests/app.c")
    target_link_libraries (test_wavesim wavesim)
endif ()

if (${${PROJECT_NAME}_PYTHON})
    target_link_libraries (wavesim PRIVATE ${PYTHON_LIBRARIES})
endif()

install (TARGETS wavesim EXPORT WaveSimConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install (EXPORT WaveSimConfig DESTINATION "share/wavesim/cmake")
export (TARGETS wavesim FILE WaveSimConfig.cmake)

# The library is being built
add_definitions (-DWAVESIM_BUILDING)

###############################################################################
# compiler definitions and flags
###############################################################################

target_compile_options (wavesim
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
# Dependency settings
###############################################################################

message (STATUS "------------------------------------------------------------")
message (STATUS "Wavesim settings")
message (STATUS " + Build type: ${${PROJECT_NAME}_BUILD_TYPE}")
message (STATUS " + PIC (Position independent code): ${${PROJECT_NAME}_PIC}")
message (STATUS " + Precision: ${${PROJECT_NAME}_PRECISION}")
message (STATUS " + Memory debugging: ${${PROJECT_NAME}_MEMORY_DEBUGGING}")
message (STATUS " + Memory backtraces: ${${PROJECT_NAME}_MEMORY_BACKTRACE}")
message (STATUS " + Profiling: ${${PROJECT_NAME}_PROFILING}")
message (STATUS " + Unit Tests: ${${PROJECT_NAME}_TESTS}")
message (STATUS " + Python bindings: ${${PROJECT_NAME}_PYTHON}")
message (STATUS "------------------------------------------------------------")
