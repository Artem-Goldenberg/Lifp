#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "Random/Random.h"
#include "Trie.h"

const char allowedSet[] = "abcdefghijklmnopqrvwxyz";

const uint maxBranches = 16;
const uint maxLength = 64;
const uint maxStrings = 512;

struct {
    char strings[maxStrings][maxLength + 1];
    uint top;
} data;

#include "utils.h"

/**
 - Parameter maxLen: Maximum amount of characters in the generated string, **excluding** trailing `\0`,
        So for example if you got 256 bytes available, you should set maxLen to `255`
*/
void createStringsWith(char* prefix, uint top, uint maxLen) {
    if (data.top >= maxStrings) {
        printf("Maximum strings exceeded\n");
        return;
    }
    // inserted \0 on the last iteration
    if (top != 0 && prefix[top - 1] == '\0') {
        append(prefix);
        return;
    }
    // maximum length is reached
    if (top == maxLen) {
        prefix[top] = '\0';
        append(prefix);
        return;
    }

    // we want at least one branch to go on, until \0 is generated
    uint nbranches = randomUIntIn(1, maxBranchesFor(top, maxLen));
    assert(nbranches);

    for (uint i = 0; i < nbranches; ++i) {
        prefix[top] = randomNodeChar(top, maxLen);
        createStringsWith(prefix, top + 1, maxLen);
    }
}

#define Tests simple, byHand, keywords
#define TestSetup prologue

#include "Test/Test.h"

// common trie
static TrieNode trie[TrieMaxSize];

void prologue(void) {
    setGenerator(8876315, NULL, 0);
    test.config.useColor = false;

    char workspace[maxLength + 1];

    createStringsWith(workspace, 0, maxLength);

//    printf("Generated Strings:\n");
//    for (uint i = 0; i < data.top; ++i)
//        printf("%s\n", data.strings[i]);


    for (uint i = 0; i < maxLength; ++i) {
        printf("[%d] = %d\n", i, maxBranchesFor(i, maxLength - 1));
    }

    for (uint i = 0; i < maxLength; ++i) {
        static const float initialNullChance = 1.0 / sizeof(allowedSet);

        uint maxDepth = maxLength;
        uint depth = i;

        float scale = (float)maxDepth / (maxDepth - depth);
        float nullProb = scale * initialNullChance;

        printf("[%d] = %f\n", i, (float)depth / (2 * maxDepth));
    }
}

typedef const char* const Strings[];


void simple(void) {
    static Strings keys = {
        "evok",
        "endure",
        "evil"
    };
    static const uint nkeys = sizeof keys / sizeof keys[0];
    static const char* snapshot[nkeys];

    // just top array
    memcpy(snapshot, keys, sizeof keys);

    uint nodeCount = buildTrie(trie, nkeys, snapshot);
    /*
     e, n v, #d ur e, i o, #l \0, #k \0,
     */
    assertt(nodeCount, eq, 12)("All nodes (direct and indirect) must in total equal 10");

    const TrieNode* found;

    found = findInTrie(trie, "evok");
    assertt(found - trie, eq, 11)("Ends on an eleventh index");

    found = findInTrie(trie, "evak");
    assert(found == NULL);

    found = findInTrie(trie, "ev");
    assert(found == NULL, "Not a valid prefix!");

    found = findInTrie(trie, "evil");
    assertt(found - trie, eq, 9)("Not 8, because terminating zero counts!");

    found = findInTrie(trie, "endure");
    assertt(found - trie, eq, 5);

    found = findInTrie(trie, "enduri");
    assert(found == NULL, "bad letter in the end");

    found = findInTrie(trie, "evill");
    assert(found == NULL, "only prefix is matched");
}

void byHand(void) {
    static const char* const keys[] = {
        "broken"
        "brok"
        "grok"
        "talk"
        "talkative"
    };

//    buildTrie(trie, , <#const char **keys#>)

//    for
}

// This is most complicated test
// 1. have file with all keywords pick random ones of them
// 2. build trie
// 3. check all of them are in the trie, check they are all distinct
// 4. select random word, select random position in it
// 5. alter it to random letter, check if already got this word
// 6. if already have this word, then alter to another letter until it's a new word
// 7. if don't have this word, assert it's not in trie
// 8. do this for different amount of keywords, measure perfomance on big ones
void keywords(void) {
    static const uint nkeys = 2134; // number of lines in file
    static const char* keys[nkeys];
    static const char* snapshot[nkeys];
    // read file, get rid on newlines
    int top = 0; size_t cap = 0; ssize_t len;
    FILE* keysFile = fopen("keys.txt", "r");
    for (char* key = NULL; (len = getline(&key, &cap, keysFile)) >= 0; key = NULL) {
        key[len - 1] = '\0';
        keys[top++] = key;
    }
    assertt(top, eq, nkeys);

    // amount of keywords to test:
    static const uint sizes[] = {
        10, 32, 45, 65, 75
        // can't really go higher not enough space to fit in 256 or so,
        // random generation with this seed only! gives compact enough words to fit
        // into 256 their offsets
    };
    static const uint nsizes = sizeof sizes / sizeof sizes[0];

    for (int i = 0; i < nsizes; ++i) {
        uint size = sizes[i];
        // top of the current snapshot
        uint top = 0;

        test.out("Testing for size: %d\n", size);

        const char** picked = calloc(size, sizeof(const char*));
        uint pickedSize = 0;
        // pick random keys for trie, skip duplicates
        for (int j = 0; j < size; ++j) {
            const char* key;
            do {
                key = keys[randomIndex(nkeys)];
            } while (in(key, pickedSize, picked));
            picked[pickedSize++] = key;
            snapshot[top++] = key;
        }
        assert(top == size);

        uint nodesCount = buildTrie(trie, size, snapshot);
        assert(nodesCount, "Built trie is empty");

        const TrieNode** nodes = calloc(size, sizeof(const TrieNode*));

        // 3. check all of them are in the trie, check they are all distinct
        for (int j = 0; j < size; ++j) {
            const TrieNode* found = findInTrie(trie, picked[j]);
            assert(found, "Key '%s' was placed but was not found", picked[j]);

            for (int k = 0; k < j; ++k) {
                assert(nodes[k] != found,
                       "Two distinct keys ('%s' at %d and '%s' at %d) "
                       "have the same index in trie!",
                       picked[j], j, picked[k], k);
            }
            nodes[j] = found;
        }

        free(nodes);
        free(picked);
    }

    memcpy(snapshot, keys, sizeof keys);

//    uint nodes = buildTrie(trie, nkeys, snapshot);

}

