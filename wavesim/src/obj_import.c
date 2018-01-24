#include "wavesim/obj.h"
#include "wavesim/mesh.h"
#include "wavesim/mesh_builder.h"
#include "wavesim/string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 256

/* ------------------------------------------------------------------------- */
static wsret
process_vertices(vector_t* vertex_buffer, char* buffer, char** save_ptr)
{
    char* coord;
    int i;
    vec3_t vertex;

    /* Mandatory xyz components */
    for (i = 0; i != 3; ++i)
    {
        coord = ws_strtok(buffer, " ", save_ptr);
        if (coord == NULL)
            return -1;
        vertex.xyz[i] = atof(coord);

    }

    /* Optional "w" component */
    coord = ws_strtok(buffer, " ", save_ptr);
    if (coord)
        vec3_div_scalar(vertex.xyz, atof(coord));

    /* Push them into the vertex buffer */
    i = vector_count(vertex_buffer);
    if (vector_resize(vertex_buffer, i + 3) < 0)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    memcpy(vector_get_element(vertex_buffer, i), vertex.xyz, sizeof(vertex.xyz));

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
static wsret
process_indices(vector_t* index_buffer, char* buffer, char** save_ptr)
{
    char* index_str;

    while ((index_str = ws_strtok(buffer, " ", save_ptr)) != NULL)
    {
        /* Any strings containing "/" are not indices */
        if (strchr(index_str, '/') != NULL)
            continue;

        uint32_t* index = vector_emplace(index_buffer);
        if (index == NULL)
            WSRET(WS_ERR_OUT_OF_MEMORY);
        *index = atoi(index_str);
    }

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
wsret
obj_import_mesh(mesh_t** mesh, const char* filename)
{
    FILE* fp;
    char buffer[BUF_LEN];
    char* token;
    char* save_ptr;
    wsret retval;
    vector_t vertex_buffer;
    vector_t index_buffer;

    fp = fopen(filename, "r");
    if (fp == NULL)
        WSRET(WS_ERR_FOPEN_FAILED);

    vector_construct(&vertex_buffer, sizeof(double));
    vector_construct(&index_buffer, 4); /* We're using uint32_t for IBs */
    while (fgets(buffer, BUF_LEN, fp) != NULL)
    {
        token = ws_strtok(buffer, " ", &save_ptr);
        while (token != NULL)
        {
            /* Ditch entire line if we see a comment */
            if (token[0] == '#')
                break;

            /* Vertices begin with "v" */
            if (strcmp(token, "v") == 0)
            {
                if ((retval = process_vertices(&vertex_buffer, buffer, &save_ptr) < 0))
                    goto parse_failed;
                break;
            }

            /* Index buffer, begins with "f" */
            if (strcmp(token, "f") == 0)
            {
                if ((retval = process_indices(&index_buffer, buffer, &save_ptr) < 0))
                    goto parse_failed;
                break;
            }

            token = ws_strtok(NULL, " ", &save_ptr);
        };
    }

    if (ferror(fp))
    {
        retval = WS_ERR_READ_ERROR;
        goto ferror_occurred;
    }

    if ((retval = mesh_create(mesh)) != WS_OK)
        goto mesh_create_failed;

    if (mesh_copy_from_buffers(*mesh,
            vertex_buffer.data, index_buffer.data,
            (WS_IB)vector_count(&vertex_buffer) / 3, (WS_IB)vector_count(&index_buffer),
            MESH_VB_DOUBLE, MESH_IB_UINT32) != WS_OK)
    {
        goto mesh_copy_buffers_failed;
    }

    vector_clear_free(&vertex_buffer);
    vector_clear_free(&index_buffer);

    return WS_OK;

    mesh_copy_buffers_failed   : mesh_destroy(*mesh);
    mesh_create_failed         :
    ferror_occurred            :
    parse_failed               : fclose(fp);
                                 vector_clear_free(&vertex_buffer);
                                 vector_clear_free(&index_buffer);
    WSRET(retval);
}
