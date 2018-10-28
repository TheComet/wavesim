function (set_python_tests)
    foreach (f ${ARGN})
        file (READ ${f} TEST_SRC)
        string (REPLACE "\"" "\\\"" TEST_SRC ${TEST_SRC})
        string (REPLACE "\n" "\\n\"\n\"" TEST_SRC ${TEST_SRC})
        get_filename_component (TEST_NAME ${f} NAME_WE)
        string (CONCAT TEST_SRC
            "TEST_F(NAME, ${TEST_NAME})\n{\n"
            "    EXPECT_THAT(PyRun_SimpleString(\""
            ${TEST_SRC}
            "    \"), Eq(0))\;\n}\n")
        file (APPEND "${WAVESIM_BINARY_DIR}/tests/src/test_python_bindings.cpp" ${TEST_SRC})
    endforeach ()
endfunction ()

execute_process (COMMAND ${CMAKE_COMMAND} -E copy "${WAVESIM_SOURCE_DIR}/cmake/templates/test_python_bindings.cpp.in" "${WAVESIM_BINARY_DIR}/tests/src/test_python_bindings.cpp")

file (GLOB PYTHON_TEST_SOURCE_FILES "${WAVESIM_SOURCE_DIR}/tests/python/*.py")

set_python_tests (${PYTHON_TEST_SOURCE_FILES})
