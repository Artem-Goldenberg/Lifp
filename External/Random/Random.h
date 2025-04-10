#ifndef Random_h
#define Random_h

#include "Common/Common.h"

#define RandomDefaultBufferSize 1024

void setGenerator(uint seed, uint basicRandomnessFunction(void), uint maxRandomNumber);

bool setResultsMemory(void* buffer, size_t bufferCapacity);

void freeResultsMemory(void);

// MARK: - Integers

int randomInt(void);
int randomIntIn(int start, int end);

uint randomUInt(void);
uint randomIndex(uint bound);
uint randomUIntIn(uint from, uint to);

bool randomBool(void);
size_t randomSize(size_t boundedBy);

bool randomOneIn(uint totalEvents);
bool randomChanceExact(uint wantedEvents, uint totalEvents);

// MARK: - Characters

char randomCharIn(char from, char to);
char randomCharFrom(const char* set);
char randomLowercaseLetter(void);
char randomUppercaseLetter(void);
char randomLetter(void);

char randomDigit(void);
char randomAlphaNumChar(void);
char randomPunctuation(void);

// MARK: - Floating Points

float randomUniformFloat(void);
float randomFloatIn(float min, float max);

bool randomChance(float probability);

// MARK: - Strings

char* randomFixedStringFrom(const char* set, size_t len);
char* randomStringFrom(const char* set, size_t maxlen);

char* randomUIntString(void);
char* randomFloatString(float min, float max, uint precision);


#endif /* Random_h */
