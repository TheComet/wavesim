#ifndef WAVESIM_H
#define WAVESIM_H

#include "wavesim/config.h"

C_BEGIN

/*!
 * @brief Initializes global state in the wavesim library. Must be called once
 * before using the library or after having called wavesim_deinit(). Calling
 * this more than once consecutively is a bad idea.
 */
WAVESIM_PUBLIC_API void
wavesim_init(void);

/*!
 * @brief Cleans up any global state in the wavesim library. Must be called
 * once after having initialized the library. Calling this more than once
 * consecutively is a bad idea.
 */
WAVESIM_PUBLIC_API void
wavesim_deinit(void);

/*!
 * @brief Runs all unit tests in the library.
 * @param[in] argv Command line arguments are passed on to googletest/mock
 * @return If all tests pass, then zero is returned. Otherwise
 */
WAVESIM_PUBLIC_API int
wavesim_run_tests(int* argc, char** argv);

C_END

#endif /* WAVESIM_H */
