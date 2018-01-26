#ifndef WAVESIM_STRING_H
#define WAVESIM_STRING_H

#include <stdio.h>
#include "wavesim/config.h"

C_BEGIN

/*!
 * @brief Parses a string into a sequence of tokens.
 *
 * @param[in,out] str On the first call to ws_strtok() the string to be parsed
 * should be specified. In each subsequent call that should parse the same
 * string, str should be NULL.
 *
 * @param[in] delim pecifies a set of bytes that delimit the tokens in the
 * parsed string. The caller may specify different strings in delim in
 * successive calls that parse the same string.
 *
 * @param[in,out] save_ptr A pointer to a char* variable that is used
 * internally by ws_strtok() in order to maintain context between successive
 * calls that parse the same string. In subsequent calls to ws_strtok, saveptr
 * should be unchanged since the previous call.
 *
 * Different strings may be parsed concurrently using sequences of calls to
 * strtok_r() that specify different saveptr arguments.
 *
 * @return Each call to ws_strtok() returns a pointer to a null-terminated
 * string containing the next token. This string does not include the
 * delimiting byte. If no more tokens are found, strtok() returns NULL.
 *
 * A sequence of two or more contiguous delimiter bytes in the parsed string is
 * considered to be a single delimiter. Delimiter bytes at the start or end of
 * the string are ignored. Put another way: the tokens returned by ws_strtok()
 * are always nonempty strings.
 */
WAVESIM_PRIVATE_API char*
ws_strtok(char* str, const char* delim, char** save_ptr);

C_END

#endif /* WAVESIM_STRING_H */
