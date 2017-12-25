#ifndef _COMMON_H
#define _COMMON_H
#endif

// error codes.
const enum class Result : int32_t
{
    ErrorUnknown      = -2,
    ErrorNotSupported = -1,
    Success           = 0,
};

// common types
typedef int32_t  int32;
typedef uint32_t uint32;
typedef int64_t  int64;
typedef uint64_t uint64;