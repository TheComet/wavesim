#include "wavesim/btree.h"
#include "wavesim/hash.h"
#include "wavesim/mesh/octree.h"
#include "wavesim/mesh/obj.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
static wsret
write_vertices(obj_exporter_t* exporter, const octree_node_t* node)
{
    int i;
    wsret result;

    if ((result = obj_write_aabb_vertices(exporter, node->aabb.xyzxyz)) != WS_OK)
        return result;

    /* Recurse into children */
    if (node->children != NULL)
        for (i = 0; i != 8; ++i)
            if ((result = write_vertices(exporter, &node->children[i])) != WS_OK)
                return result;

    WSRET(WS_OK);
}

/* ------------------------------------------------------------------------- */
static wsret
write_indices(obj_exporter_t* exporter, const octree_node_t* node)
{
    int i;
    wsret result;

    if ((result = obj_write_aabb_indices(exporter, node->aabb.xyzxyz)) != WS_OK)
        return result;

    /* Recurse into children */
    if (node->children != NULL)
        for (i = 0; i != 8; ++i)
            if ((result = write_indices(exporter, &node->children[i])) != WS_OK)
                return result;

    WSRET(WS_OK);
}

/* ------------------------------------------------------------------------- */
wsret
obj_export_octree(const char* filename, const octree_t* octree)
{
    wsret result;
    obj_exporter_t exporter;

    if ((result = obj_exporter_open(&exporter, filename)) != WS_OK)
        return result;

    if ((result = write_vertices(&exporter, &octree->root)) != WS_OK)
        goto bail;
    if ((result = write_indices(&exporter, &octree->root)) != WS_OK)
        goto bail;

    obj_exporter_close(&exporter);
    WSRET(WS_OK);

    bail : obj_exporter_close(&exporter);
    WSRET(result);
}
