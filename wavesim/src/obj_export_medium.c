#include "wavesim/btree.h"
#include "wavesim/hash.h"
#include "wavesim/obj.h"
#include "wavesim/medium.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
wsret
obj_export_medium(const char* filename, const medium_t* medium)
{
    wsret result;
    obj_exporter_t exporter;

    if ((result = obj_exporter_open(&exporter, filename)) != WS_OK)
        return result;

    VECTOR_FOR_EACH(&medium->partitions, medium_partition_t, partition)
        if ((result = obj_write_aabb_vertices(&exporter, partition->aabb.xyzxyz)) != WS_OK)
            goto bail;
    VECTOR_END_EACH
    VECTOR_FOR_EACH(&medium->partitions, medium_partition_t, partition)
        if ((result = obj_write_aabb_indices(&exporter, partition->aabb.xyzxyz)) != WS_OK)
            goto bail;
    VECTOR_END_EACH

    bail: obj_exporter_close(&exporter);

    return result;
}
