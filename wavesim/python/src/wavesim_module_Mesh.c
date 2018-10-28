#include "wavesim/python/wavesim_module_Attribute.h"
#include "wavesim/python/wavesim_module_Face.h"
#include "wavesim/python/wavesim_module_Mesh.h"
#include "wavesim/python/wavesim_module_MeshIterator.h"
#include "wavesim/python/wavesim_module_Vertex.h"
#include "wavesim/mesh/mesh.h"
#include "wavesim/mesh/mesh_builder.h"

/* ------------------------------------------------------------------------- */
static void
Mesh_dealloc(wavesim_Mesh* self)
{
    if (self->mesh_builder != NULL)
        mesh_builder_destroy(self->mesh_builder);
    if (self->mesh != NULL)
        mesh_destroy(self->mesh);
    Py_XDECREF(self->faceIterator);
    Py_XDECREF(self->vertexIterator);
    Py_XDECREF(self->indexIterator);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    (void)args;
    (void)kwds;

    wavesim_Mesh* self = (wavesim_Mesh*)type->tp_alloc(type, 0);
    if (self == NULL)
        goto Mesh_alloc_failed;

    self->indexIterator = (wavesim_MeshIterator*)
            PyObject_CallObject((PyObject*)&wavesim_MeshIteratorType, NULL);
    self->vertexIterator = (wavesim_MeshIterator*)
            PyObject_CallObject((PyObject*)&wavesim_MeshIteratorType, NULL);
    self->faceIterator = (wavesim_MeshIterator*)
            PyObject_CallObject((PyObject*)&wavesim_MeshIteratorType, NULL);
    if (self->indexIterator == NULL || self->vertexIterator == NULL || self->faceIterator == NULL)
        goto alloc_iterators_failed;
    self->indexIterator->kind = ITERATE_INDICES;
    self->vertexIterator->kind = ITERATE_VERTICES;
    self->faceIterator->kind = ITERATE_FACES;
    self->indexIterator->pyMesh = self;
    self->vertexIterator->pyMesh = self;
    self->faceIterator->pyMesh = self;

    /* Always have a mesh instance. Builders are only created when begin() is called */
    if (mesh_create(&self->mesh) != WS_OK)
        goto mesh_create_failed;
    self->mesh_builder = NULL;

    return (PyObject*)self;

    mesh_create_failed      :
    alloc_iterators_failed  : Py_DECREF(self);
    Mesh_alloc_failed       : return NULL;
}

/* ------------------------------------------------------------------------- */
static int
Mesh_init(wavesim_Mesh* self, PyObject* args, PyObject* kwds)
{
    (void)self;
    (void)args;
    (void)kwds;
    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_begin(wavesim_Mesh* self)
{
    if (self->mesh_builder != NULL)
        mesh_builder_destroy(self->mesh_builder);

    if (mesh_builder_create(&self->mesh_builder) != WS_OK)
        return NULL;

    Py_RETURN_NONE;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_add_face(wavesim_Mesh* self, wavesim_Face* pyFace)
{
    if (PyObject_TypeCheck(pyFace, &wavesim_FaceType) == 0)
    {
        PyErr_SetString(PyExc_TypeError, "Argument must be of type wavesim.Face");
        return NULL;
    }

    if (self->mesh_builder == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "No build session is active. You must call begin() first before adding faces.");
        return NULL;
    }

    face_t f = face(
        vertex(vec3(
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 0))->position, 0)),
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 0))->position, 1)),
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 0))->position, 2))
        ), attribute(
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 0))->attr->reflection,
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 0))->attr->transmission,
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 0))->attr->absorption
        )),
        vertex(vec3(
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 1))->position, 0)),
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 1))->position, 1)),
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 1))->position, 2))
        ), attribute(
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 1))->attr->reflection,
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 1))->attr->transmission,
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 1))->attr->absorption
        )),
        vertex(vec3(
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 2))->position, 0)),
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 2))->position, 1)),
            PyFloat_AsDouble(PyTuple_GetItem(((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 2))->position, 2))
        ), attribute(
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 2))->attr->reflection,
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 2))->attr->transmission,
            ((wavesim_Vertex*)PyTuple_GetItem(pyFace->vertices, 2))->attr->absorption
        ))
    );
    if (mesh_builder_add_face(self->mesh_builder, f) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Internal error: Couldn't add face");
        return NULL;
    }

    Py_RETURN_NONE;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_build(wavesim_Mesh* self)
{
    mesh_t* new_mesh;

    if (self->mesh_builder == NULL)
        return NULL;

    if (mesh_builder_build(&new_mesh, self->mesh_builder) != WS_OK)
        return NULL;

    mesh_destroy(self->mesh);
    self->mesh = new_mesh;

    Py_RETURN_NONE;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_clear(wavesim_Mesh* self)
{
    mesh_clear_buffers(self->mesh);
    Py_RETURN_NONE;
}

/* TODO
 *   attributes: faces[], vertices[], indices[] */

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_getindices(wavesim_Mesh* self, void* closure)
{
    (void)closure;
    Py_INCREF(self->indexIterator);
    return (PyObject*)self->indexIterator;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_getvertices(wavesim_Mesh* self, void* closure)
{
    (void)closure;
    Py_INCREF(self->vertexIterator);
    return (PyObject*)self->vertexIterator;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Mesh_getfaces(wavesim_Mesh* self, void* closure)
{
    (void)closure;
    Py_INCREF(self->faceIterator);
    return (PyObject*)self->faceIterator;
}

/* ------------------------------------------------------------------------- */
static PyGetSetDef Mesh_getsetters[] = {
    {"indices",  (getter)Mesh_getindices,  NULL, ""},
    {"vertices", (getter)Mesh_getvertices, NULL, ""},
    {"faces",    (getter)Mesh_getfaces,    NULL, ""},
    {NULL}
};

/* ------------------------------------------------------------------------- */
static PyMethodDef Mesh_methods[] = {
    {"begin",    (PyCFunction)Mesh_begin,    METH_NOARGS, "Begin a mesh builder session"},
    {"add_face", (PyCFunction)Mesh_add_face, METH_O,      "Add a face to the mesh"},
    {"build",    (PyCFunction)Mesh_build,    METH_NOARGS, "End a building session, making the mesh usable."},
    {"clear",    (PyCFunction)Mesh_clear,    METH_NOARGS, "Clears all vertices/faces from the mesh."},
    {NULL}
};

/* ------------------------------------------------------------------------- */
PyTypeObject wavesim_MeshType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wavesim.Mesh",                                /* tp_name */
    sizeof(wavesim_Mesh),                          /* tp_basicsize */
    0,                                             /* tp_itemsize */
    (destructor)Mesh_dealloc,                      /* tp_dealloc */
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
    Mesh_methods,                                  /* tp_methods */
    0,                                             /* tp_members */
    Mesh_getsetters,                               /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc)Mesh_init,                           /* tp_init */
    0,                                             /* tp_alloc */
    Mesh_new                                       /* tp_new */
};

/* ------------------------------------------------------------------------- */
int
wavesim_init_Mesh(void)
{
    if (PyType_Ready(&wavesim_MeshType) < 0)
        return -1;
    return 0;
}
