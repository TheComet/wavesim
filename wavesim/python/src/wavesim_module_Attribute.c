#include "wavesim/python/wavesim_module_Attribute.h"
#include "structmember.h"

static char* kwlist[] = {"reflection", "transmission", "absorption", NULL};

/* ------------------------------------------------------------------------- */
static int
Attribute_init(wavesim_Attribute* self, PyObject* args, PyObject* kwds)
{
    self->reflection = 0;
    self->transmission = 0;
    self->absorption = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ddd", kwlist,
                                     &self->reflection,
                                     &self->transmission,
                                     &self->absorption))
    {
        return -1;
    }

    if (self->reflection == 0.0 || self->transmission == 0.0 || self->absorption == 0.0)
    {
        self->reflection = 0;
        self->transmission = 0;
        self->absorption = 1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Attribute_repr(wavesim_Attribute* self)
{
    PyObject* fmt = PyUnicode_FromString("wavesim.Attribute(reflection=%f, transmission=%f, absorption=%f)");
    PyObject* args = PyTuple_New(3);
    PyTuple_SetItem(args, 0, PyFloat_FromDouble(self->reflection));
    PyTuple_SetItem(args, 1, PyFloat_FromDouble(self->transmission));
    PyTuple_SetItem(args, 2, PyFloat_FromDouble(self->absorption));
    PyObject* str = PyUnicode_Format(fmt, args);

    Py_DECREF(fmt);
    Py_DECREF(args);
    return str;
}

/* ------------------------------------------------------------------------- */
static PyMemberDef Attribute_members[] = {
    {"reflection",   T_DOUBLE, offsetof(wavesim_Attribute, reflection),   0, ""},
    {"transmission", T_DOUBLE, offsetof(wavesim_Attribute, transmission), 0, ""},
    {"absorption",   T_DOUBLE, offsetof(wavesim_Attribute, absorption),   0, ""},
    {NULL}
};

/* ------------------------------------------------------------------------- */
PyTypeObject wavesim_AttributeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wavesim.Attribute",                           /* tp_name */
    sizeof(wavesim_Attribute),                     /* tp_basicsize */
    0,                                             /* tp_itemsize */
    0,                                             /* tp_dealloc */
    0,                                             /* tp_print */
    0,                                             /* tp_getattr */
    0,                                             /* tp_setattr */
    0,                                             /* tp_reserved */
    (reprfunc)Attribute_repr,                      /* tp_repr */
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
    Attribute_members,                             /* tp_members */
    0,                                             /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc)Attribute_init,                      /* tp_init */
    0,                                             /* tp_alloc */
    0                                              /* tp_new */
};

/* ------------------------------------------------------------------------- */
int
wavesim_init_Attribute(void)
{
    wavesim_AttributeType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&wavesim_AttributeType) < 0)
        return -1;
    return 0;
}
