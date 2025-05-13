#include "Allocator.h"

#include <stdlib.h>
#include <assert.h>

// MARK: - Memory Allocation

#define PureChunkSize (ChunkSize - sizeof(Chunk))

/// Mininum chunk space left to make a reservation
/// If chunks doesn't have this much space left, reserve in the next chunk
#define ReservationFreeSpace 128

typedef struct Chunk {
    byte* top;
    byte* end;
    bool isReserved;
    struct Chunk* previous;
    struct Chunk* next;
    byte storage[];
} Chunk;

// ???: Many threads ???
__thread struct {
    /// Linked list of memory chunks
    Chunk* head;
    /// Chunk from which we allocate space currently
    Chunk* active;
    /// Last chunk which was reserved for contiguous allocation
    Chunk* lastReserved;
} allocator;

static Chunk* newChunk(Chunk* fromChunk) {
    Chunk* result = malloc(sizeof(Chunk) + PureChunkSize);
    if (!result) return NULL;
    if (fromChunk) fromChunk->next = result;
    result->previous = fromChunk;
    result->top = result->storage;
    result->end = result->storage + PureChunkSize;
    result->next = NULL;
    result->isReserved = false;
    return result;
}

void initializeAllocator(void) {
    allocator.head = newChunk(NULL);
    allocator.active = allocator.head;
}

void deinitializeAllocator(void) {
    Chunk* current = allocator.head;
    while (current) {
        Chunk* next = current->next;
        free(current);
        current = next;
    }
    allocator.head = NULL;
    allocator.active = NULL;
}

/// For small sized allocations
void* allocate(uint size) {
    assert(size <= PureChunkSize); // won't allocate that much
    Chunk* current = allocator.active;
    Chunk* previous = current;
    assert(current);

    while (current) {
        if (current->top + size <= current->end && !current->isReserved)
            goto AddAndReturn;
        previous = current;
        current = current->next;
    }

    current = newChunk(previous);
    if (!current) return NULL;

AddAndReturn:
    allocator.active = current;
    byte* result = current->top;
    current->top += size;
    return result;
}

static Chunk* growChunk(Chunk* chunk) {
    // need to remember how much this chunk was filled to set
    // the top pointer to correct position after reallocation
    size_t filled = chunk->top - chunk->storage;

    size_t newStorageSize = (chunk->end - chunk->storage) * 2;
    chunk = realloc(chunk, sizeof(Chunk) + newStorageSize);

    if (!chunk) return NULL;

    chunk->top = chunk->storage + filled;
    chunk->end = chunk->storage + newStorageSize;
    chunk->previous->next = chunk;

    if (chunk->next)
        chunk->next->previous = chunk;

    return chunk;
}

/// Allocates in a reserved contiguous space
void* allocateFromReserved(uint size) {
    assert(size < PureChunkSize); // still against big size allocations
    Chunk* chunk = allocator.lastReserved;
    assert(chunk);
    assert(chunk->isReserved);

    if (chunk->top + size > chunk->end) {
        // this case is a real troubble, our reserved memory has run out
        // but we need more of it
        chunk = growChunk(chunk);
        if (!chunk) // allocation failed
            return NULL;
    }
    byte* result = chunk->top;
    chunk->top += size;
    return result;
}

/// Makes a contiguous reservation
///
/// - Returns: number of bytes reserved
size_t reserve(void) {
    Chunk* current = allocator.active;
    Chunk* previous = current;
    assert(current);
    assert(!current->isReserved);

    while (current) {
        // ???: Can current be reserved
        if (current->top + ReservationFreeSpace <= current->end)
            goto Found;
        // this chunk is not fit for reserving, select another
        previous = current;
        current = current->next;
    }
    current = newChunk(previous);
    if (!current) return 0;
Found:
    assert(!current->isReserved);
    current->isReserved = true;
    allocator.lastReserved = current;
    while (current && current->isReserved) {
        // I think it's not possible to have reserved chunks after an active chunk,
        // but, just in case, we loop on this condition
        previous = current;
        current = current->next;
    }
    if (!current) current = newChunk(previous);
    if (!current) {
        // interesting situation here: wee reserved the chunk, but failed to
        // allocate a new active chunk, in this case I guess we rollback the reservation
        unreserve();
        return 0;
    }
    // set allocator's active chunk
    allocator.active = current;
    return current->end - current->top;
}

void unreserve(void) {
    Chunk* chunk = allocator.lastReserved;
    assert(chunk);
    assert(chunk->isReserved);

    chunk->isReserved = false;
    allocator.active = chunk;

    // select previous reserved chunk if any
    while (chunk && !chunk->isReserved)
        chunk = chunk->previous;
    allocator.lastReserved = chunk;
}
