#include "wavesim/wavesim_module_Face.h"
#include "wavesim/wavesim_module_Vertex.h"
#include "structmember.h"

/* ------------------------------------------------------------------------- */
static int
Face_init(wavesim_Face* self, PyObject* args, PyObject* kwds)
{
    (void)kwds;

    wavesim_Vertex *v1, *v2, *v3;
    PyObject* tmp;
    if (!PyArg_ParseTuple(args, "O!O!O!", &wavesim_VertexType, &v1,
                                          &wavesim_VertexType, &v2,
                                          &wavesim_VertexType, &v3))
    {
        return -1;
    }

    tmp = self->vertices;
    self->vertices = PyTuple_New(3);
    Py_INCREF(v1);   PyTuple_SetItem(self->vertices, 0, (PyObject*)v1);
    Py_INCREF(v2);   PyTuple_SetItem(self->vertices, 1, (PyObject*)v2);
    Py_INCREF(v3);   PyTuple_SetItem(self->vertices, 2, (PyObject*)v3);
    Py_XDECREF(tmp);

    return 0;
}

/* ------------------------------------------------------------------------- */
static PyMemberDef Face_members[] = {
    {"vertices", T_OBJECT_EX, offsetof(wavesim_Face, vertices), 0, ""},
    {NULL}
};

/* ------------------------------------------------------------------------- */
PyTypeObject wavesim_FaceType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wavesim.Face",                                /* tp_name */
    sizeof(wavesim_Face),                          /* tp_basicsize */
    0,                                             /* tp_itemsize */
    0,                                             /* tp_dealloc */
    0,                                             /* tp_print */
    0,                                             /* tp_getattr */
    0,                                             /* tp_setattr */
    0,                                             /* tp_reserved */
    0,                                             /* tp_repr */
    0,                                             /* tp_as_number */
    0,                                             /* tp_as_sequence */
    0,                                             /* tp_as_mapping */
    0,                                             /* tp_hash  */
    0,                                             /* tp_call */
    0,                                             /* tp_str */
    0,                                             /* tp_getattro */
    0,                                             /* tp_setattro */
    0,                                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                            /* tp_flags */
    "",                                            /* tp_doc */
    0,                                             /* tp_traverse */
    0,                                             /* tp_clear */
    0,                                             /* tp_richcompare */
    0,                                             /* tp_weaklistoffset */
    0,                                             /* tp_iter */
    0,                                             /* tp_iternext */
    0,                                             /* tp_methods */
    Face_members,                                  /* tp_members */
    0,                                             /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc)Face_init,                           /* tp_init */
    0,                                             /* tp_alloc */
    0                                              /* tp_new */
};

/* ------------------------------------------------------------------------- */
int
wavesim_init_Face(void)
{
    wavesim_FaceType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&wavesim_FaceType) < 0)
        return -1;
    return 0;
}
