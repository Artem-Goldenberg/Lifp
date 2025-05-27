#ifndef Common_h
#define Common_h

#include <stdint.h>
#include <stdbool.h>

typedef unsigned int uint;
typedef uint8_t byte;

typedef int32_t  int32;
typedef uint32_t uint32;

/// Expands to a pointer which is at `align` alignment and is greater than or equals to `ptr`
/// **`align` must be a power of two!**
#define AlignedAt(align, ptr) (void*)(((uintptr_t)(ptr) + ((int)(align) - 1)) & -(int)(align))

typedef const char* const StaticString;

#endif /* Common_h */
