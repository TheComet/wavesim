#ifndef BUILD_INFO_H
#define BUILD_INFO_H

#include "wavesim/config.h"

C_BEGIN

WAVESIM_PUBLIC_API const char*
wavesim_version(void);

WAVESIM_PUBLIC_API int
wavesim_build_number(void);

WAVESIM_PUBLIC_API const char*
wavesim_build_host(void);

WAVESIM_PUBLIC_API const char*
wavesim_build_time(void);

WAVESIM_PUBLIC_API const char*
wavesim_commit_info(void);

WAVESIM_PUBLIC_API const char*
wavesim_compiler_info(void);

WAVESIM_PUBLIC_API const char*
wavesim_cmake_configuration(void);

WAVESIM_PUBLIC_API const char*
wavesim_build_info(void);

C_END

#endif /* BUILD_INFO_H */
