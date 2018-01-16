#include "Python.h"
#include "wavesim/face.h"

typedef struct wavesim_Vertex wavesim_Vertex;
typedef struct wavesim_Face
{
    PyObject_HEAD
    PyObject* vertices;
} wavesim_Face;

PyTypeObject wavesim_FaceType;

int
wavesim_init_Face(void);
