#include "StringTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdbool.h>

#define DefaultBucketsAmount 1001
#define DefaultChunkSize (4096 - 8) // 8 for the pointer

#define BorderLoadFactorPercent 24

 // 16 million strings should be enough for anyone
#define AvailableSlotsLimit (1 << 24)
#define ErrorIndex (AvailableSlotsLimit + 1)

#define EntrySize sizeof(const char*)

//typedef struct StringChunk StringChunk;
static void warn(const char* message, ...)
__attribute__ ((format (printf, 1, 2)));

static void warn(const char* message, ...) {
    va_list args;
    va_start(args, message);
    fprintf(stderr, "StringTable [Warning]: ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
}

typedef struct StringChunk {
    struct StringChunk* previous;
    char storage[];
} StringChunk;

static StringChunk* newChunk(size_t size) {
    StringChunk* result = malloc(sizeof(StringChunk) + size);
    if (!result) return NULL;
    result->previous = NULL;
    return result;
}

typedef struct EntrySet {
    struct EntrySet* next;
    /// Maximum amount of different hash values for the table
    uint availableSlots;
    uint filledSlots;
    /// An array of entries, each is a pointer to a unique string (sequence of characters ending with `\0`)
    const char* entries[];
} EntrySet;

static EntrySet* newSet(uint nslots) {
    if (nslots > AvailableSlotsLimit) {
        warn("Trying to allocate too big of a hash table with size = %u. "
             "Won't do it, quit",
             nslots);
        return NULL;
    }
    EntrySet* result = malloc(sizeof(EntrySet) + nslots * EntrySize);
    if (!result) return NULL;
    result->next = NULL;
    result->availableSlots = nslots;
    result->filledSlots = 0;
    memset(result->entries, 0, nslots * EntrySize);
    return result;
}

struct StringTable {
    /// Internal fields for working with chunks of strings
    StringChunk* currentChunk;
    size_t chunkSize;
    uint top;
    /// A linked list of hash tables with progressively more buckets
    EntrySet* tables;
};


StringTable* newStringTable(uint nbuckets, size_t chunkSize) {
    if (!nbuckets) nbuckets = DefaultBucketsAmount;
    if (!chunkSize) chunkSize = DefaultChunkSize;

    StringTable* result = malloc(sizeof(StringTable) + nbuckets * EntrySize);
    if (!result) return NULL;

    result->tables = newSet(nbuckets);
    if (!result->tables) {
        warn("Failed to allocate space for the first entry set\n");
        free(result);
        return NULL;
    }
    result->chunkSize = chunkSize;

    StringChunk* current = newChunk(chunkSize);
    if (!current) {
        warn("Failed to allocate space for the first chunk\n");
        free(result);
        return NULL;
    }
    result->currentChunk = current;
    result->top = 0;

    return result;
}

static uint hash(const char* str) {
    static const uint hash_mask = 0xF0000000;
    uint g; // for calculating hash
    uint hash_value = 0;
    for (; *str; str++) {
        // Calculating hash: see Dragon Book, Fig. 7.35
        hash_value = (hash_value << 4) + *str;
        if ((g = hash_value & hash_mask) != 0) {
            // hash_value ^= g >> 24 ^ g;
            hash_value = hash_value ^ (hash_value >> 24);
            hash_value ^= g;
        }
    }
    // final hash value for identifier
    return hash_value;
}

static bool isLoaded(const EntrySet* set) {
    // a little bit of math to avoid float arithmetic: n/m > p/100 -> n * 100 > m * p
    return set->filledSlots * 100 > set->availableSlots * BorderLoadFactorPercent;
}

/** Example probe function to use with the `StringTable` class

 There are a couple of requirements such function must meet:
    1. Values of `step` from `0` up to `n - 1` must each uniquely correspond to some index in this range.
        In other words, on the second argument, this function must be a bijection on the set `{0..<n}`,

    2. Negative values of the `step` parameter should correspond to the indices of previous probs

 > Note: something

 - Parameter hash: Hash of the element obtained by the hash function above
 - Parameter step: Probe number, **can be negative**, allowed values: `-n < i < n`
 - Parameter n: Amount of elements in the hash table
 */
static uint nextProbe(uint hash, uint step, uint n) {
    // just a simple linear probing for now
    return (hash + step) % n;
}

#define Probe(h, i, m) (((h) + (i)) % (m))

static uint moreSlots(uint currentSlots) {
    // again, very simple for now
    return currentSlots * 2;
}

static bool allocateNextChunk(StringTable* self) {
    StringChunk* chunk = newChunk(self->chunkSize);
    if (!chunk)
        return false;
    chunk->previous = self->currentChunk;
    self->currentChunk = chunk;
    // chunk was updated, so reset top to the start
    self->top = 0;
    return true;
}


static char* copyToChunk(StringTable* self, const char* str) {
    size_t size = strlen(str) + 1;
    if ((size_t)self->top + size > self->chunkSize) {
        bool ok = allocateNextChunk(self);
        if (!ok) {
            warn("Failed to allocate space for a new chunk");
            return NULL;
        }
    }
    char* destination = self->currentChunk->storage + self->top;
    strncpy(destination, str, size);
    self->top += size;
    return destination;
}

static uint insertIntoSet(StringTable* self, EntrySet* current, uint hash, const char* str);

static uint tryToInsertAt(StringTable* self, EntrySet* current,
                          uint index, uint hash, const char* str) {
    if (isLoaded(current)) {
        // check if the previous cell is clear
        // if that's the case, then we will never
        //            uint previousIndex = nextProbe(index, -1, current->availableSlots);
        //            if (current->entries[previousIndex] == NULL)
        //                return index;

        EntrySet* next = current->next;
        if (!next) {
            next = newSet(moreSlots(current->availableSlots));
            if (!next) {
                warn("Failed to allocate new hash table");
                return ErrorIndex;
             }
            current->next = next;
        }
        uint result = insertIntoSet(self, next, hash, str);
        if (result == ErrorIndex) return ErrorIndex;

        // offset the result from next hash table to separate indices
        return current->availableSlots + result;
    }
    // hash table is not full yet, can insert a new element
    const char* copy = copyToChunk(self, str);
    if (!copy) return ErrorIndex;

    current->entries[index] = copy;
    current->filledSlots++;

    return index;
}

static uint insertIntoSet(StringTable* self, EntrySet* current,
                          uint hash, const char* str) {
    uint startIndex = hash % current->availableSlots;
    for (uint i = 0; i < current->availableSlots; ++i) {
        uint index = nextProbe(startIndex, i, current->availableSlots);

        if (current->entries[index] == NULL) // found an empty place
            return tryToInsertAt(self, current, index, hash, str);

        if (strcmp(current->entries[index], str) == 0) // found a duplicate
            return index;

    }
    // this can never happend, when load factor is strictly less than 100%
    assert(false);
    return ErrorIndex;
}

uint insertIntoStringTable(StringTable* self, const char* str) {
    uint index = insertIntoSet(self, self->tables, hash(str), str);
    if (index == ErrorIndex)
        warn("Failed to insert: \"%s\"", str);
    return index;
}

const char* getFromStringTable(StringTable* self, uint index) {
    assert(index != ErrorIndex);
    EntrySet* set = self->tables;
    while (set) {
        if (index < set->availableSlots)
            return set->entries[index];

        index -= set->availableSlots;

        set = set->next;
    }
    return NULL;
}

static void freeEntrySet(EntrySet* table) {
    // we are not afraid of recursion here
    // the amount of tables in the linked list is expected to be small
    if (table->next != NULL)
        freeEntrySet(table->next);

    free(table);
}

void freeStringTable(StringTable* self) {
    StringChunk* chunk = self->currentChunk;
    // at least one chunk must be allocated
    assert(chunk);

    while (chunk->previous) {
        StringChunk* toFree = chunk;
        chunk = chunk->previous;
        free(toFree);
    }
    free(chunk);

    freeEntrySet(self->tables);

    memset(self, 0, sizeof(StringTable));
    free(self);
}
