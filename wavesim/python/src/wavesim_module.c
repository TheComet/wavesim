#include "Python.h"
#include "wavesim/wavesim.h"
#include "wavesim/wavesim_module_Attribute.h"
#include "wavesim/wavesim_module_Face.h"
#include "wavesim/wavesim_module_Mesh.h"
#include "wavesim/wavesim_module_MeshIterator.h"
#include "wavesim/wavesim_module_Vertex.h"

static PyObject*
wavesim_hello(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;

    puts("hello world");

    Py_INCREF(Py_None);
    return Py_None;
}

/* ------------------------------------------------------------------------- */
void
wavesim_module_free(void* x)
{
    (void)x;
    wavesim_deinit();
}

/* ------------------------------------------------------------------------- */
static PyMethodDef wavesim_methods[] = {
    {"hello", wavesim_hello, METH_VARARGS, "Hello World"},
    {NULL, NULL, 0, NULL}
};

/* ------------------------------------------------------------------------- */
static PyModuleDef wavesim_module = {
    PyModuleDef_HEAD_INIT,
    "wavesim",               /* Module name */
    NULL,                    /* docstring, may be NULL */
    -1,                      /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables */
    wavesim_methods,         /* module methods */
    NULL,                    /* m_reload */
    NULL,                    /* m_traverse */
    NULL,                    /* m_clear */
    wavesim_module_free      /* m_free */
};

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

    if (wavesim_init() != 0)
        goto library_init_failed;

    m = PyModule_Create(&wavesim_module);
    if (m == NULL)
        goto create_module_failed;

    if (init_builtin_types() != 0)
        goto init_builtin_types_failed;
    if (add_builtin_types_to_module(m) != 0)
        goto add_builtin_types_failed;

    return m;

    add_builtin_types_failed  :
    init_builtin_types_failed : Py_DECREF(m);
    create_module_failed      : wavesim_deinit();
    library_init_failed       : return NULL;
}
