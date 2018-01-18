#include "Python.h"

typedef enum wavesim_MeshIteratorKind
{
    ITERATE_INDICES,
    ITERATE_VERTICES,
    ITERATE_FACES
} wavesim_MeshIteratorKind;

typedef struct wavesim_Mesh wavesim_Mesh;
typedef struct wavesim_MeshIterator
{
    PyObject_HEAD
    wavesim_MeshIteratorKind kind;
    wavesim_Mesh* pyMesh;
} wavesim_MeshIterator;

PyTypeObject wavesim_MeshIteratorType;

int
wavesim_init_MeshIterator(void);
