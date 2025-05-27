#ifndef Allocator_h
#define Allocator_h

#include "Common/Common.h"

#include <stddef.h>

/// Allocator will allocate memory in chunks, this is the size of them, but they can grow sometimes
#define ChunkSize 4096

void initializeAllocator(void);
void deinitializeAllocator(void);

size_t reserve(void);
void* allocate(uint size);
void* allocateFromReserved(uint size);
void* getReservedPointer(void);
void setReservedPointer(void* pointer);
void unreserve(void);

#endif /* Allocator_h */
