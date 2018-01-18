#include "Python.h"

typedef struct wavesim_MeshIterator wavesim_MeshIterator;
typedef struct mesh_t mesh_t;
typedef struct mesh_builder_t mesh_builder_t;

typedef struct wavesim_Mesh
{
    PyObject_HEAD
    wavesim_MeshIterator* indexIterator;
    wavesim_MeshIterator* vertexIterator;
    wavesim_MeshIterator* faceIterator;
    mesh_t* mesh;
    mesh_builder_t* mesh_builder;
} wavesim_Mesh;

PyTypeObject wavesim_MeshType;

int
wavesim_init_Mesh(void);
