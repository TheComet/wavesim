#include "Python.h"
#include "wavesim/python/wavesim_module_Attribute.h"
#include "wavesim/python/wavesim_module_Face.h"
#include "wavesim/python/wavesim_module_Mesh.h"
#include "wavesim/python/wavesim_module_MeshIterator.h"
#include "wavesim/python/wavesim_module_Vertex.h"
#include "wavesim/wavesim.h"
#include "wavesim/build_info.h"

/* ------------------------------------------------------------------------- */
void
wavesim_module_free(void* x)
{
    (void)x;
    wavesim_deinit();
}

/* ------------------------------------------------------------------------- */
static PyModuleDef wavesim_module = {
    PyModuleDef_HEAD_INIT,
    "wavesim",               /* Module name */
    NULL,                    /* docstring, may be NULL */
    -1,                      /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables */
    NULL,                    /* module methods */
    NULL,                    /* m_reload */
    NULL,                    /* m_traverse */
    NULL,                    /* m_clear */
    wavesim_module_free      /* m_free */
};

/* ------------------------------------------------------------------------- */
static int
add_module_constants(PyObject* m)
{
    if (PyModule_AddStringConstant(m, "build_info", wavesim_build_info()) == -1) return -1;
    if (PyModule_AddIntConstant(m, "build_number", wavesim_build_number()) == -1) return -1;
    if (PyModule_AddStringConstant(m, "build_host", wavesim_build_host()) == -1) return -1;
    if (PyModule_AddStringConstant(m, "build_time", wavesim_build_time()) == -1) return -1;
    if (PyModule_AddStringConstant(m, "commit_info", wavesim_commit_info()) == -1) return -1;
    if (PyModule_AddStringConstant(m, "compiler_info", wavesim_compiler_info()) == -1) return -1;
    if (PyModule_AddStringConstant(m, "cmake_configuration", wavesim_cmake_configuration()) == -1) return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */
static int
init_builtin_types(void)
{
    if (wavesim_init_Attribute() != 0)     return -1;
    if (wavesim_init_Face() != 0)          return -1;
    if (wavesim_init_Mesh() != 0)          return -1;
    if (wavesim_init_MeshIterator() != 0)  return -1;
    if (wavesim_init_Vertex() != 0)        return -1;
    return 0;
}

/* ------------------------------------------------------------------------- */
PyTypeObject wavesim_MeshType;
static int
add_builtin_types_to_module(PyObject* m)
{
    Py_INCREF(&wavesim_AttributeType);
    Py_INCREF(&wavesim_FaceType);
    Py_INCREF(&wavesim_MeshType);
    Py_INCREF(&wavesim_MeshIteratorType);
    Py_INCREF(&wavesim_VertexType);

    if (PyModule_AddObject(m, "Attribute",    (PyObject*)&wavesim_AttributeType) < 0)    return -1;
    if (PyModule_AddObject(m, "Face",         (PyObject*)&wavesim_FaceType) < 0)         return -1;
    if (PyModule_AddObject(m, "Mesh",         (PyObject*)&wavesim_MeshType) < 0)         return -1;
    if (PyModule_AddObject(m, "MeshIterator", (PyObject*)&wavesim_MeshIteratorType) < 0) return -1;
    if (PyModule_AddObject(m, "Vertex",       (PyObject*)&wavesim_VertexType) < 0)       return -1;
    return 0;
}

/* ------------------------------------------------------------------------- */
PyMODINIT_FUNC
PyInit_wavesim(void)
{
    PyObject* m;

    m = PyModule_Create(&wavesim_module);
    if (m == NULL)
        goto module_alloc_failed;

    if (wavesim_init() != 0)
        goto wavesim_init_failed;

    if (init_builtin_types() != 0)            goto init_module_failed;
    if (add_builtin_types_to_module(m) != 0)  goto init_module_failed;
    if (add_module_constants(m) != 0)         goto init_module_failed;

    return m;

    init_module_failed        :
    wavesim_init_failed       : Py_DECREF(m);
    module_alloc_failed       : return NULL;
}
