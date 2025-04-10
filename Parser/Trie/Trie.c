#include "Trie.h"

#include <stdlib.h>
#include <assert.h>


static TrieNode* directNodeAt(TrieNode* node, const char* key) {
    assert(*key); // key must be non-empty

    // move node one up, mark as direct + specifically say, there are no children
    // of this node, and it's letter is the first letter of the key
    *node++ = (TrieNode) {
        .letter = *key++,
        .isDirect = true,
        .childrenOffset = 0
    };

    // copy the rest of the key into plain memory of trie
    // end is a pointer to \0 char
    uintptr_t end = (uintptr_t)stpcpy((char*)node, key);

    // go to the next empty trie node
    end += sizeof(TrieNode) - end % (sizeof(TrieNode));

    return (TrieNode*)end;
}

static TrieNode* recursiveBuild(TrieNode* trie, uint nkeys, const char* keys[nkeys]) {
    for (;!**keys && nkeys; keys++, nkeys--); // drop all empty keys
    if (!nkeys) return trie; // if no keys left, have nothing to insert

    if (nkeys == 1) return directNodeAt(trie, *keys);

    TrieNode* last = trie;
    last->letter = **keys;

    uint* bucketSize = calloc(nkeys, sizeof(uint));
    uint* bucket = bucketSize;

    for (const char** key = keys; key < keys + nkeys; ++(*bucket), ++(*key), ++key) {
        if (last->letter != **key) {
            bucket++;
            (++last)->letter = **key;
        }
    }

    TrieNode* end = last + 1;
    for (uint i = 0; i <= last - trie; keys += bucketSize[i], ++i) {
        // set defaults
        trie[i].isDirect = false;
        trie[i].childrenOffset = 0;

        assert(end - trie - i < 256);

        trie[i].childrenOffset = end - trie - i;

        // first key string in this group (shortest one, because they are sorted)
        // is out of letters, it means, that current prefix is a valid entry
        if (!**keys) {
            // current child node is now a valid ending
            trie[i].isDirect = true;
            // skip this empty key in the recursive call
            keys++;
            // if no more keys except for this empty one, then skip this branch entirely
            if (--bucketSize[i] == 0)
                continue;
        }

        end = recursiveBuild(end, bucketSize[i], keys);
    }

    return end;
}

static int compareString(const char** str1, const char** str2) {
    return strcmp(*str1, *str2);
}

// Builds a trie structure
uint buildTrie(TrieNode trie[TrieMaxSize], uint nkeys, const char* keys[nkeys]) {
    typedef int Comparator(const void*, const void*);
    memset(trie, 0, TrieMaxSize * sizeof(TrieNode));

    // sort strings in lexicographical order
    qsort(keys, nkeys, sizeof(const char*), (Comparator*)compareString);

    TrieNode* end = recursiveBuild(trie, nkeys, keys);

    return (uint)(end - trie);
}

// Create a definition for the linker in this unit's object file in case inlining won't happen
extern inline const TrieNode* findInTrie(const TrieNode* trie, const char* key);
