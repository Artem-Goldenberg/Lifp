#ifndef StringTable_h
#define StringTable_h

#include "Common/Common.h"

#include <stddef.h>

/** Hashtable for efficiently storing a lot of strings

 Removes duplicate string entries.
 Uses a simple pointer jumping algorithm instead of linked lists.

*/
typedef struct StringTable StringTable;


StringTable* newStringTable(uint initialNumberOfBuckets, size_t chunkSize);

/** Inserts given `string` to the table, discarding duplicates
 - Returns: Index of the inserted value (or old index if the `string` was already there)
*/
uint insertIntoStringTable(StringTable* self, const char* string);

const char* getFromStringTable(StringTable* self, uint index);

void freeStringTable(StringTable* self);


#endif /* StringTable_h */
