#include "wavesim/string.h"
#include "wavesim/mesh/obj.h"
#include "wavesim/mesh/mesh.h"
#include "wavesim/mesh/mesh_builder.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 256

/* ------------------------------------------------------------------------- */
static wsret
process_vertices(vector_t* vertex_buffer, char** save_ptr)
{
    char* coord;
    size_t i;
    vec3_t vertex;

    /* Mandatory xyz components */
    for (i = 0; i != 3; ++i)
    {
        coord = ws_strtok(NULL, " ", save_ptr);
        if (coord == NULL)
            return -1;
        vertex.xyz[i] = atof(coord);

    }

    /* Optional "w" component */
    coord = ws_strtok(NULL, " ", save_ptr);
    if (coord)
        vec3_div_scalar(vertex.xyz, atof(coord));

    /* Push them into the vertex buffer */
    i = vector_count(vertex_buffer);
    if (vector_resize(vertex_buffer, i + 3) == VECTOR_ERROR)
        WSRET(WS_ERR_OUT_OF_MEMORY);
    memcpy(vector_get(vertex_buffer, i), vertex.xyz, sizeof(vertex.xyz));

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
static wsret
process_face_elements(vector_t* index_buffer, char** line_save_ptr)
{
    char* index_str;
    char* token_save_ptr;
    int i;

    for (i = 0; i != 3; ++i)
    {
        /* Get next face element in current line */
        index_str = ws_strtok(NULL, " ", line_save_ptr);
        if (index_str == NULL)
        {
            WSRET(WS_ERR_TOO_FEW_INDICES);
        }

        /* split index/texcoord/normal -- we're only interested in the index */
        index_str = ws_strtok(index_str, "/", &token_save_ptr);
        if (index_str == NULL)
        {
            WSRET(WS_ERR_TOO_FEW_INDICES);
        }

        int32_t* index = vector_emplace(index_buffer);
        if (index == NULL)
            WSRET(WS_ERR_OUT_OF_MEMORY);
        *index = atoi(index_str);
    }

    /* Make sure what we parsed was actually a tri and not a quad/ngon */
    if (ws_strtok(NULL, " ", line_save_ptr) != NULL)
    {
        WSRET(WS_ERR_INDICES_ARENT_A_TRI);
    }

    WSRET(WS_OK);
}

/* ------------------------------------------------------------------------- */
void
remap_indices(vector_t* index_buffer, int vertex_count)
{
    VECTOR_FOR_EACH(index_buffer, int32_t, index)
        if (*index < 0)
            *index = *index + vertex_count; /* -1 refers to the last vertex */
        else
            --(*index); /* obj uses offsets starting at 1, we use 0 */
    VECTOR_END_EACH
}

/* ------------------------------------------------------------------------- */
wsret
obj_import_mesh(const char* filename, mesh_t* mesh)
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
    vector_construct(&index_buffer, 4); /* We're using int32_t for IBs */
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
                if ((retval = process_vertices(&vertex_buffer, &save_ptr) < 0))
                    goto parse_failed;
                break;
            }

            /* Index buffer, begins with "f" */
            if (strcmp(token, "f") == 0)
            {
                if ((retval = process_face_elements(&index_buffer, &save_ptr) < 0))
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

    remap_indices(&index_buffer, (int)vector_count(&vertex_buffer) / 3);

    /* Note: This call clears the mesh's existing buffers for us */
    if ((retval = mesh_copy_from_buffers(mesh,
            vertex_buffer.data, index_buffer.data,
            vector_count(&vertex_buffer) / 3, vector_count(&index_buffer),
            MESH_VB_DOUBLE, MESH_IB_INT32)) != WS_OK)
    {
        goto mesh_copy_buffers_failed;
    }

    vector_clear_free(&vertex_buffer);
    vector_clear_free(&index_buffer);

    return WS_OK;

    mesh_copy_buffers_failed   :
    ferror_occurred            :
    parse_failed               : fclose(fp);
                                 vector_clear_free(&vertex_buffer);
                                 vector_clear_free(&index_buffer);
    WSRET(retval);
}
