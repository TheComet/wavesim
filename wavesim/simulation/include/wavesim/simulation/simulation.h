#ifndef SIMULATION_H
#define SIMULATION_H

#include "wavesim/config.h"
#include "wavesim/log.h"
#include "wavesim/simulation/medium.h"

C_BEGIN

typedef struct simulation_t simulation_t;
typedef wsret (*simulation_prepare_func)(simulation_t*);
typedef int (*simulation_advance_func)(simulation_t*,wsreal_t);
typedef void (*simulation_finalize_func)(simulation_t*);
typedef void (*simulation_interrupt_func)(simulation_t*);
typedef void (*simulation_advance_cb_func)(const simulation_t*,wsreal_t);

typedef struct audio_source_t audio_source_t;
typedef struct audio_listener_t audio_listener_t;

/*! Opaque pointer, is implemented by the specific simulation */
typedef struct simulation_state_t simulation_state_t;
/*! Opaque pointer, is implemented by clients of the wavesim library, if necessary */
typedef struct simulation_user_data_t simulation_user_data_t;

typedef enum simulation_type_e
{
    WAVESIM_ARD,
    WAVESIM_RAY
} simulation_type_e;

typedef struct simulation_t
{
    simulation_state_t*     state;
    simulation_user_data_t* user_data;

    vector_t meshes;          /* mesh_t* */
    vector_t audio_sources;   /* audio_source_t* */
    vector_t audio_listeners; /* audio_listener_t* */
    wsreal_t max_frequency;
    wsreal_t cell_tolerance;

    simulation_prepare_func   prepare;
    simulation_advance_func   advance;
    simulation_finalize_func  finalize;
    simulation_interrupt_func interrupt;
} simulation_t;

/*!
 * @brief Allocates and initializes a new simulation object on the heap.
 * @param[out] simulation The new object is written to this parameter.
 * @param[in] type The type of simulation (see simulation_type_e)
 * @return Returns WS_OK on success.
 */
WAVESIM_PUBLIC_API wsret WAVESIM_WARN_UNUSED
simulation_create(simulation_t** simulation, simulation_type_e type);

/*!
 * @brief Frees a simulation object.
 * @param[in] simulation Object to destroy.
 */
WAVESIM_PUBLIC_API void
simulation_destroy(simulation_t* simulation);

/*!
 * @brief Initializes an existing simulation object. Note that the object must
 * not have been previously initialized. Good for stack allocated simulation
 * objects.
 * @param[in] simulation The object to initialize.
 * @param[in] type The type of simulation (see simulation_type_e).
 */
WAVESIM_PUBLIC_API void
simulation_construct(simulation_t* simulation, simulation_type_e type);

/*!
 * @brief Frees all simulation related data, without freeing the simulation
 * object itself. Good for stack allocated simulation objects.
 * @param[in] simulation The simulation to destruct.
 */
WAVESIM_PUBLIC_API void
simulation_destruct(simulation_t* simulation);

/*!
 * @brief Sets callback functions to point to the a specific implementation.
 * @note The simulation must not be running.
 * @param[in] simulation The simulation object to modify.
 * @param[in] type The type of simulation (see simulation_type_e).
 */
WAVESIM_PUBLIC_API void
simulation_set_type(simulation_t* simulation, simulation_type_e type);

WAVESIM_PUBLIC_API void
simulation_set_resolution(simulation_t* simulation, wsreal_t max_frequency, wsreal_t cell_tolerance);

WAVESIM_PUBLIC_API wsret
simulation_add_mesh(simulation_t* simulation, mesh_t* mesh);

#define simulation_mesh_count(sim) \
        vector_count(&sim->meshes)

#define simulation_get_mesh(sim, idx) \
        *(mesh_t**)vector_get_element(&sim->meshes, idx)

WAVESIM_PUBLIC_API wsret
simulation_add_audio_source(simulation_t* simulation, audio_source_t* as);

#define simulation_audio_source_count(sim) \
        vector_count(&sim->audio_sources)

#define simulation_get_audio_source(sim, idx) \
        *(audio_source_t**)vector_get_element(&sim->audio_sources, idx)

WAVESIM_PUBLIC_API wsret
simulation_add_audio_listener(simulation_t* simulation, audio_listener_t* al);

#define simulation_audio_listener_count(sim) \
        vector_count(&sim->audio_listeners)

#define simulation_get_audio_listener(sim, idx) \
        *(audio_listener_t**)vector_get_element(&sim->audio_listeners, idx)

WAVESIM_PUBLIC_API wsret
simulation_execute(simulation_t* simulation);

C_END

#endif /* SIMULATION_H */
