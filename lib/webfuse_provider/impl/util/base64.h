#ifndef WFP_UTIL_BASE64_H
#define WFP_UTILBASE64_H

#ifndef __cplusplus
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#else
#include <cinttypes>
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern size_t wfp_impl_base64_encoded_size(size_t length);

extern size_t wfp_impl_base64_encode(
    uint8_t const * data,
    size_t length,
    char * buffer,
    size_t buffer_size);

extern size_t wfp_impl_base64_decoded_size(char const * data, size_t length);

extern size_t wfp_impl_base64_decode(
    char const * data,
    size_t length,
    uint8_t * buffer,
    size_t buffer_size);

extern bool wfp_impl_base64_isvalid(char const * data, size_t length);

#ifdef __cplusplus
}
#endif

#endif
