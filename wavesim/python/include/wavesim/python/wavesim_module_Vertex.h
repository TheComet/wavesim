#include "Python.h"
#include "wavesim/mesh/vertex.h"

typedef struct wavesim_Attribute wavesim_Attribute;
typedef struct wavesim_Vertex
{
    PyObject_HEAD
    PyObject* position;
    wavesim_Attribute* attr;
} wavesim_Vertex;

PyTypeObject wavesim_VertexType;

int
wavesim_init_Vertex(void);
