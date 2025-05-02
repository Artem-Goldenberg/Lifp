#ifndef Common_h
#define Common_h

#include <stdint.h>
#include <stdbool.h>

typedef unsigned int uint;
typedef uint8_t byte;

typedef int32_t  int32;
typedef uint32_t uint32;

#define AlignedAt(align, ptr) (void*)(((uintptr_t)(ptr) + ((align) - 1)) & -(align));

typedef const char* const StaticString;

#endif /* Common_h */
