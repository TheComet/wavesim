#include "Python.h"
#include "wavesim/attribute.h"

typedef struct wavesim_Attribute
{
    PyObject_HEAD
    double reflection;
    double transmission;
    double absorption;
} wavesim_Attribute;

PyTypeObject wavesim_AttributeType;

int
wavesim_init_Attribute(void);
