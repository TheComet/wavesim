#include "wavesim/obj.h"
#include "wavesim/mesh.h"
#include "wavesim/mesh_builder.h"
#include <string.h>
#include <stdio.h>

#define BUF_LEN 256

/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
wsret
obj_import_mesh(mesh_t** mesh, const char* filename)
{
    FILE* fp;
    char buffer[BUF_LEN];
    wsret retval;
    mesh_builder_t* mb;

    fp = fopen(filename, "r");
    if (fp == NULL)
        WSRET(WS_ERR_FOPEN_FAILED);

    if ((retval = mesh_builder_create(&mb)) != WS_OK)
        goto mesh_builder_create_failed;

    while (fgets(buffer, BUF_LEN, fp) != NULL)
    {

    }

    if ((retval = mesh_builder_build(mesh, mb)) != WS_OK)
        goto mesh_build_failed;

    if (ferror(fp))
    {
        retval = WS_ERR_READ_ERROR;
        goto ferror_occurred;
    }

    return WS_OK;

    ferror_occurred            :
    mesh_build_failed          :
    mesh_builder_create_failed : fclose(fp);
    WSRET(retval);
}
