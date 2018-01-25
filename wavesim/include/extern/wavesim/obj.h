#ifndef WAVESIM_OBJ_H
#define WAVESIM_OBJ_H

#include "wavesim/config.h"

C_BEGIN

typedef struct mesh_t mesh_t;
typedef struct medium_t medium_t;

WAVESIM_PUBLIC_API wsret WS_WARN_UNUSED
obj_import_mesh(mesh_t** mesh, const char* filename);

WAVESIM_PUBLIC_API wsret WS_WARN_UNUSED
obj_export_partition(const medium_t* mesh);

C_END

#endif /* WAVESIM_OBJ_H */
