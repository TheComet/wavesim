#ifndef WAVESIM_H
#define WAVESIM_H

#include "wavesim/config.h"

C_BEGIN

/*!
 * @brief Initializes global state in the wavesim library. Must be called once
 * before using the library or after having called wavesim_deinit(). The call
 * is guarded and can be called multiple times consecutively.
 */
WAVESIM_PUBLIC_API wsret WAVESIM_WARN_UNUSED
wavesim_init(void);

/*!
 * @brief Cleans up any global state in the wavesim library. Must be called
 * once after having initialized the library. The call is guarded and can be
 * called multiple times consecutively.
 * @return Returns the difference in malloc() and free() calls, if memory
 * debugging is enabled. Returns 0 if there were no memory leaks, or 0 if
 * memory debugging is disabled.
 */
WAVESIM_PUBLIC_API int
wavesim_deinit(void);

/*!
 * @brief Runs all unit tests in the library.
 * @param[in] argv Command line arguments are passed on to googletest/mock
 */
WAVESIM_PUBLIC_API wsret WAVESIM_WARN_UNUSED
wavesim_run_tests(int* argc, char** argv);

C_END

#endif /* WAVESIM_H */
