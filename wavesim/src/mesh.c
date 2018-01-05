#include "wavesim/face.h"
#include "wavesim/mesh.h"
#include "wavesim/memory.h"

/* ------------------------------------------------------------------------- */
mesh_t*
mesh_create()
{
    mesh_t* mesh = (mesh_t*)MALLOC(sizeof *mesh);
    if (mesh == NULL)
        OUT_OF_MEMORY(NULL);
    mesh_construct(mesh);
    return mesh;
}

/* ------------------------------------------------------------------------- */
void
mesh_construct(mesh_t* mesh)
{
    vector_construct(&mesh->faces, sizeof(face_t));
}

/* ------------------------------------------------------------------------- */
void
mesh_destruct(mesh_t* mesh)
{
    vector_clear_free(&mesh->faces);
}

/* ------------------------------------------------------------------------- */
void
mesh_destroy(mesh_t* mesh)
{
    mesh_destruct(mesh);
    FREE(mesh);
}
