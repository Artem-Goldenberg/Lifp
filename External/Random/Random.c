#include "Random.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define GrowSize(old) ((old) * 2)

static uint defaultSource(void) {
    // for now just use rand, maybe replace by something stronger in the future;
    return (uint)rand();
}

static struct {
    uint seed;
    uint (*source)(void);
    uint bound;
    /// Fields needed for memory management:
    bool isSelfManaged;
    void* buffer;
    size_t filled;
    size_t capacity;
} generator;

void setGenerator(uint seed, uint randomSource(void), uint max) {
    if (!randomSource) {
        // default source requires srand, other sources user'll have to handle himself
        srand(seed);
        randomSource = defaultSource;
    }
    if (!max) max = RAND_MAX;
    generator.seed = seed;
    generator.bound = max;
    generator.source = randomSource;
}

bool setResultsMemory(void* buffer, size_t size) {
    assert(!generator.isSelfManaged);
    if (buffer) { // user provides buffer and size for it
        assert(size);
        generator.filled = 0;
        generator.buffer = buffer;
        generator.capacity = size;
        return true;
    }
    // self-managed memory mode
    if (!size) size = RandomDefaultBufferSize;

    buffer = malloc(size);
    if (!buffer) return false;

    generator.filled = 0;
    generator.buffer = buffer;
    generator.capacity = size;
    generator.isSelfManaged = true;

    return true;
}

void freeResultsMemory(void) {
    // can only free self-managed buffers
    assert(generator.isSelfManaged);
    free(generator.buffer);
    // clear all fields
    generator.filled = 0;
    generator.capacity = 0;
    generator.buffer = NULL;
    generator.isSelfManaged = false;
}

static void* request(size_t size) {
    if (generator.filled + size > generator.capacity) {
        if (!generator.isSelfManaged)
            return NULL;

        size_t newSize = GrowSize(generator.capacity);
        void* newBuffer = realloc(generator.buffer, newSize);

        if (!newBuffer) return NULL;

        generator.buffer = newBuffer;
        generator.capacity = newSize;
    }
    void* result = generator.buffer + generator.filled;
    generator.filled += size;
    return result;
}

// MARK: - Integers

int randomInt(void) {
    return generator.source() - generator.bound / 2;
}

int randomIntIn(int start, int end) {
    assert(start <= end);
    uint rand = generator.source();
    return rand % (end - start + 1) + start;
}

uint randomUInt(void) {
    return generator.source();
}

uint randomIndex(uint bound) {
    assert(bound);
    return randomUInt() % bound;
}

uint randomUIntIn(uint start, uint end) {
    assert(start <= end);
    uint rand = generator.source();
    return rand % (end - start + 1) + start;
}

size_t randomSize(size_t bound) {
    return generator.source() % bound;
}

bool randomBool(void) {
    return generator.source() % 2;
}

bool randomOneIn(uint amount) {
    assert(amount);
    return randomUIntIn(1, amount) == amount;
}

bool randomChanceExact(uint num, uint denum) {
    assert(denum);
    return randomUIntIn(1, denum) <= num;
}

// MARK: - Characters

char randomCharIn(char start, char end) {
    return randomIntIn(start, end);
}

char randomCharFrom(const char* set) {
    size_t len = strlen(set);
    return set[generator.source() % len];
}

char randomLowercaseLetter(void) {
    return randomCharIn('a', 'z');
}

char randomUppercaseLetter(void) {
    return randomCharIn('A', 'Z');
}

char randomLetter(void) {
    if (randomBool())
        return randomLowercaseLetter();
    return randomUppercaseLetter();
}

char randomDigit(void) {
    return randomCharIn('0', '9');
}

char randomAlphaNumChar(void) {
    if (randomBool())
        return randomLetter();
    return randomDigit();
}

char randomPunctuation(void) {
    return randomCharFrom("!\"#$%&'()*+,-./:;?@[\\]^_`{|}~");
}

// MARK: - Floating Points

float randomUniformFloat(void) {
    float zeroToOne = (float)generator.source() / (generator.bound + 1);
    return zeroToOne;
}

float randomFloatIn(float min, float max) {
    assert(min <= max);
    return min + randomUniformFloat() * (max - min);
}

bool randomChance(float p) {
    return randomUniformFloat() < p;
}

// MARK: - Strings

char* randomFixedStringFrom(const char* set, size_t len) {
    char* result = request(len + 1); // \0
    if (!result) return NULL;

    for (uint i = 0; i < len; ++i) result[i] = randomCharFrom(set);
    result[len] = '\0';

    return result;
}

char* randomStringFrom(const char* set, size_t maxlen) {
    size_t len = randomSize(maxlen);
    return randomFixedStringFrom(set, len);
}

static uint ndigits(int num) {
    uint result = 0;
    for (; num; ++result) num /= 10;
    return result;
}

char* randomUIntString(void) {
    uint rand = randomUInt();
    size_t len = ndigits(rand);
    char* result = request(len + 1);
    if (!result) return NULL;
    sprintf(result, "%u", rand);
    return result;
}

char* randomFloatString(float min, float max, uint precision) {
    float rand = randomFloatIn(min, max);
    int intPart = (int)rand;

    // calculate required string length:
    uint len = ndigits(intPart);
    if (!len) len = 1; // will still print zero
    if (intPart < 0) len++; // print - sign
    len += precision + 1; // . + digits after it

    char* result = request(len + 1); // for \0
    if (!result) return NULL;

    sprintf(result, "%.*f", precision, rand);

    return result;
}

