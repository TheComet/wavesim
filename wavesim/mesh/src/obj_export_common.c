#include "wavesim/hash.h"
#include "wavesim/vec3.h"
#include "wavesim/mesh/obj.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
wsret
obj_exporter_open(obj_exporter_t* exporter, const char* filename)
{
    exporter->fp = fopen(filename, "w");
    if (exporter->fp == NULL)
        return WS_ERR_FOPEN_FAILED;

    hashmap_construct(&exporter->vi_map, sizeof(wsreal_t)*3, sizeof(void*));
    exporter->index_counter = 1; /* Obj indices start at 1 */

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
void
obj_exporter_close(obj_exporter_t* exporter)
{
    hashmap_destruct(&exporter->vi_map);
    fclose(exporter->fp);
}

/* ------------------------------------------------------------------------- */
wsret
obj_write_vertex(obj_exporter_t* exporter, const wsreal_t vert[3])
{
    wsret result = hashmap_insert(&exporter->vi_map, vert, &exporter->index_counter);
    if (wsret_is_error(result))
        return result;

    if (result == WS_OK) /* vert doesn't exist yet */
    {
        fprintf(exporter->fp, "v %.6g %.6g %.6g\n", vert[0], vert[1], vert[2]);
        ++exporter->index_counter;
    }

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
wsret
obj_write_aabb_vertices(obj_exporter_t* exporter, const wsreal_t aabb[6])
{
    wsret result;

    /* These are the 8 vertices of the bounding box */
    if ((result = obj_write_vertex(exporter, vec3(aabb[0], aabb[1], aabb[2]).xyz)) != WS_OK) return result;
    if ((result = obj_write_vertex(exporter, vec3(aabb[0], aabb[1], aabb[5]).xyz)) != WS_OK) return result;
    if ((result = obj_write_vertex(exporter, vec3(aabb[0], aabb[4], aabb[2]).xyz)) != WS_OK) return result;
    if ((result = obj_write_vertex(exporter, vec3(aabb[0], aabb[4], aabb[5]).xyz)) != WS_OK) return result;
    if ((result = obj_write_vertex(exporter, vec3(aabb[3], aabb[1], aabb[2]).xyz)) != WS_OK) return result;
    if ((result = obj_write_vertex(exporter, vec3(aabb[3], aabb[1], aabb[5]).xyz)) != WS_OK) return result;
    if ((result = obj_write_vertex(exporter, vec3(aabb[3], aabb[4], aabb[2]).xyz)) != WS_OK) return result;
    if ((result = obj_write_vertex(exporter, vec3(aabb[3], aabb[4], aabb[5]).xyz)) != WS_OK) return result;

    return WS_OK;
}

/* ------------------------------------------------------------------------- */
wsret
obj_write_aabb_indices(obj_exporter_t* exporter, const wsreal_t aabb[6])
{
    int i;

    /* These are the 8 vertices of the bounding box */
    const vec3_t aaa = vec3(aabb[0], aabb[1], aabb[2]);
    const vec3_t aab = vec3(aabb[0], aabb[1], aabb[5]);
    const vec3_t aba = vec3(aabb[0], aabb[4], aabb[2]);
    const vec3_t abb = vec3(aabb[0], aabb[4], aabb[5]);
    const vec3_t baa = vec3(aabb[3], aabb[1], aabb[2]);
    const vec3_t bab = vec3(aabb[3], aabb[1], aabb[5]);
    const vec3_t bba = vec3(aabb[3], aabb[4], aabb[2]);
    const vec3_t bbb = vec3(aabb[3], aabb[4], aabb[5]);

    /* These are the 12 edges of the bounding box */
    struct {
        vec3_t a, b;
    } edges[12];
    edges[0].a  = aaa; edges[0].b  = aab;
    edges[1].a  = aaa; edges[1].b  = aba;
    edges[2].a  = aaa; edges[2].b  = baa;
    edges[3].a  = aab; edges[3].b  = abb;
    edges[4].a  = aab; edges[4].b  = bab;
    edges[5].a  = aba; edges[5].b  = abb;
    edges[6].a  = aba; edges[6].b  = bba;
    edges[7].a  = baa; edges[7].b  = bab;
    edges[8].a  = baa; edges[8].b  = bba;
    edges[9].a  = abb; edges[9].b  = bbb;
    edges[10].a = bab; edges[10].b = bbb;
    edges[11].a = bba; edges[11].b = bbb;

    for (i = 0; i != 12; ++i)
    {
        wsib_t* index1_ptr = hashmap_find(&exporter->vi_map, edges[i].a.xyz);
        wsib_t* index2_ptr = hashmap_find(&exporter->vi_map, edges[i].b.xyz);
        if (index1_ptr == NULL || index2_ptr == NULL)
            return WS_ERR_VERTEX_INDEX_NOT_FOUND;
        fprintf(exporter->fp, "f %d %d\n", *index1_ptr, *index2_ptr);
    }

    return WS_OK;
}
