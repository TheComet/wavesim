#include "wavesim/python/wavesim_module_Mesh.h"
#include "wavesim/python/wavesim_module_MeshIterator.h"
#include "wavesim/mesh/mesh.h"

/* ------------------------------------------------------------------------- */
static Py_ssize_t
MeshIterator_len(wavesim_MeshIterator* self)
{
    switch (self->kind)
    {
        case ITERATE_INDICES  : return mesh_index_count(self->pyMesh->mesh);
        case ITERATE_VERTICES : return mesh_vertex_count(self->pyMesh->mesh);
        case ITERATE_FACES    : return mesh_face_count(self->pyMesh->mesh);
        default: return -1;
    }
}

/* ------------------------------------------------------------------------- */
static PySequenceMethods MeshIterator_sequence_methods = {
    (lenfunc)MeshIterator_len     /* sq_length */
};

/* ------------------------------------------------------------------------- */
PyTypeObject wavesim_MeshIteratorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wavesim.MeshIterator",                        /* tp_name */
    sizeof(wavesim_MeshIterator),                  /* tp_basicsize */
    0,                                             /* tp_itemsize */
    0,                                             /* tp_dealloc */
    0,                                             /* tp_print */
    0,                                             /* tp_getattr */
    0,                                             /* tp_setattr */
    0,                                             /* tp_reserved */
    0,                                             /* tp_repr */
    0,                                             /* tp_as_number */
    &MeshIterator_sequence_methods,                /* tp_as_sequence */
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
    0,                                             /* tp_members */
    0,                                             /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    0,                                             /* tp_init */
    0,                                             /* tp_alloc */
    0                                              /* tp_new */
};

/* ------------------------------------------------------------------------- */
int
wavesim_init_MeshIterator(void)
{
    wavesim_MeshIteratorType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&wavesim_MeshIteratorType) < 0)
        return -1;
    return 0;
}
