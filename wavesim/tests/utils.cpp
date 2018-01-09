#include "utils.hpp"
#include "wavesim/vec3.h"
#include <string.h>

static vec3_t cube_mb[36] = {
    // front face
    {{ 0,  0,  0}},
    {{ 1,  0,  0}},
    {{ 1,  1,  0}},
    {{ 1,  1,  0}},
    {{ 0,  1,  0}},
    {{ 0,  0,  0}},
    // back face
    {{ 0,  0,  1}},
    {{ 1,  0,  1}},
    {{ 1,  1,  1}},
    {{ 1,  1,  1}},
    {{ 0,  1,  1}},
    {{ 0,  0,  1}},
    // left face
    {{ 0,  0,  0}},
    {{ 0,  1,  0}},
    {{ 0,  1,  1}},
    {{ 0,  1,  1}},
    {{ 0,  0,  1}},
    {{ 0,  0,  0}},
    // right face
    {{ 1,  0,  0}},
    {{ 1,  1,  0}},
    {{ 1,  1,  1}},
    {{ 1,  1,  1}},
    {{ 1,  0,  1}},
    {{ 1,  0,  0}},
    // top face
    {{ 0,  1,  0}},
    {{ 1,  1,  0}},
    {{ 1,  1,  1}},
    {{ 1,  1,  1}},
    {{ 0,  1,  1}},
    {{ 0,  1,  0}},
    // bottom face
    {{ 0,  0,  0}},
    {{ 1,  0,  0}},
    {{ 1,  0,  1}},
    {{ 1,  0,  1}},
    {{ 0,  0,  1}},
    {{ 0,  0,  0}}
};

static float cube_vb[24] = {
    0,  0,  0,  /* 6 o-------o 7    */
    0,  0,  1,  /*   |\      |\     */
    0,  1,  0,  /*   | \ 2   | \ 3  */
    0,  1,  1,  /*   |4 o-------o   */
    1,  0,  0,  /*   o--|----o  |   */
    1,  0,  1,  /*    \ |    5\ |   */
    1,  1,  0,  /*     \|0     \|1  */
    1,  1,  1   /*      o-------o   */
};

static uint16_t cube_ib[36] = {
    0, 1, 2, 2, 1, 3, /* front */
    4, 0, 6, 6, 0, 2, /* left */
    2, 3, 6, 6, 3, 7, /* top */
    5, 4, 6, 6, 7, 5, /* back */
    1, 5, 7, 7, 3, 1, /* right */
    0, 4, 1, 1, 4, 5  /* bottom */
};

void mesh_builder_cube(mesh_builder_t* mb, aabb_t bb)
{
    real width  = bb.b.v.x - bb.a.v.x;
    real height = bb.b.v.y - bb.a.v.y;
    real depth  = bb.b.v.z - bb.a.v.z;
    for (int i = 0; i != 12; ++i)
    {
        mesh_builder_add_face(mb, face(
            vertex(vec3( cube_mb[i*3+0].v.x * width  + bb.a.v.x,
                                             cube_mb[i*3+0].v.y * height + bb.a.v.y,
                                             cube_mb[i*3+0].v.z * depth  + bb.a.v.z), 0, 0, 0),
            vertex(vec3( cube_mb[i*3+1].v.x * width  + bb.a.v.x,
                                             cube_mb[i*3+1].v.y * height + bb.a.v.y,
                                             cube_mb[i*3+1].v.z * depth  + bb.a.v.z), 0, 0, 0),
            vertex(vec3( cube_mb[i*3+2].v.x * width  + bb.a.v.x,
                                             cube_mb[i*3+2].v.y * height + bb.a.v.y,
                                             cube_mb[i*3+2].v.z * depth  + bb.a.v.z), 0, 0, 0)
        ));
    }
}

void mesh_cube(mesh_t* mesh, aabb_t bb)
{

    real width  = bb.b.v.x - bb.a.v.x;
    real height = bb.b.v.y - bb.a.v.y;
    real depth  = bb.b.v.z - bb.a.v.z;

    float buffer[24];
    memcpy(buffer, cube_vb, sizeof(float) * 24);
    for (int i = 0; i != 24; i += 3)
    {
        buffer[i+0] = buffer[i+0] * width  + bb.a.v.x;
        buffer[i+1] = buffer[i+1] * height + bb.a.v.y;
        buffer[i+2] = buffer[i+2] * depth  + bb.a.v.z;
    }

    mesh_copy_from_buffers(mesh, buffer, cube_ib, 24, 36, MESH_VB_FLOAT, MESH_IB_UINT16);
}
