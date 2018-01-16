#include "wavesim/wavesim_module_Vertex.h"
#include "wavesim/wavesim_module_Attribute.h"
#include "structmember.h"

/* ------------------------------------------------------------------------- */
static int
Vertex_init(wavesim_Vertex* self, PyObject* args, PyObject* kwds)
{
    (void)kwds;
    double x, y, z;
    PyObject* tmp;
    wavesim_Attribute* attr = NULL;

    if (!PyArg_ParseTuple(args, "ddd|O!",
                          &x, &y, &z,
                          &wavesim_AttributeType, &attr))
    {
        return -1;
    }

    tmp = self->position;
    self->position = PyTuple_New(3);
    PyTuple_SetItem(self->position, 0, PyFloat_FromDouble(x));
    PyTuple_SetItem(self->position, 1, PyFloat_FromDouble(y));
    PyTuple_SetItem(self->position, 2, PyFloat_FromDouble(z));
    Py_XDECREF(tmp);

    if (attr != NULL)
    {
        tmp = (PyObject*)self->attr;
        Py_INCREF(attr);
        self->attr = attr;
        Py_XDECREF(tmp);
    }
    else
    {
        tmp = (PyObject*)self->attr;
        self->attr = (wavesim_Attribute*)PyObject_CallObject((PyObject*)&wavesim_AttributeType, NULL);
        Py_XDECREF(tmp);
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
static PyMemberDef Vertex_members[] = {
    {"position", T_OBJECT_EX, offsetof(wavesim_Vertex, position), 0, ""},
    {"attr",     T_OBJECT_EX, offsetof(wavesim_Vertex, attr),     0, ""},
    {NULL}
};

/* ------------------------------------------------------------------------- */
PyTypeObject wavesim_VertexType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wavesim.Vertex",                              /* tp_name */
    sizeof(wavesim_Vertex),                        /* tp_basicsize */
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
    Vertex_members,                                /* tp_members */
    0,                                             /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc)Vertex_init,                         /* tp_init */
    0,                                             /* tp_alloc */
    0                                              /* tp_new */
};

/* ------------------------------------------------------------------------- */
int
wavesim_init_Vertex(void)
{
    wavesim_VertexType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&wavesim_VertexType) < 0)
        return -1;
    return 0;
}
