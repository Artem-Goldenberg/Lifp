#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Random/Random.h"
#include "Trie.h"

const char allowedSet[] = "abcdefghijklmnopqrvwxyz";
//const char allowedSet[] = {'a', '\0'};
//const char allowedSet[1] = "a\0";

const uint maxBranches = 16;
const uint maxLength = 32;
const uint maxStrings = 256;

struct {
    char strings[maxStrings][maxLength + 1];
    uint top;
} data;

void append(const char* string) {
    strcpy(data.strings[data.top++], string);
}

uint max(uint a, uint b) {
    return b > a ? b : a;
}

char randomNodeChar(uint depth, uint maxDepth) {
//    static const float initialNullChance = 1.0 / sizeof(allowedSet);
//
//    float scale = (float)maxDepth / (maxDepth - depth);
//    float nullProb = scale * initialNullChance;

    if (randomChance((float)depth / (maxDepth * 2)))
        return '\0';

    uint index = randomIndex(sizeof allowedSet);
    return allowedSet[index];
}

uint maxBranchesFor(uint depth, uint maxDepth) {
//    float scale = (float)(maxDepth - depth) / maxDepth;
//    return max(scale * maxBranches, 1);

    float scale = 1.0 / (depth + 1);
    return max(scale * maxBranches, 1);
}

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
//    uint nbranches = maxBranchesFor(top, maxLen);
    assert(nbranches);

    for (uint i = 0; i < nbranches; ++i) {
        prefix[top] = randomNodeChar(top, maxLen);
        createStringsWith(prefix, top + 1, maxLen);
    }
}


int main1(int argc, const char * argv[]) {
    setGenerator(8876315, NULL, 0);

//    printf("%d\n", sizeof allowedSet);
//    printf("%d\n", allowedSet[sizeof allowedSet - 1]);

    char workspace[maxLength + 1];

    createStringsWith(workspace, 0, maxLength);

    printf("Generated Strings:\n");
    for (uint i = 0; i < data.top; ++i)
        printf("%s\n", data.strings[i]);


//    for (uint i = 0; i < maxLength; ++i) {
//        printf("[%d] = %d\n", i, maxBranchesFor(i, maxLength - 1));
//    }

    for (uint i = 0; i < maxLength; ++i) {
        static const float initialNullChance = 1.0 / sizeof(allowedSet);

        uint maxDepth = maxLength;
        uint depth = i;

        float scale = (float)maxDepth / (maxDepth - depth);
        float nullProb = scale * initialNullChance;

        printf("[%d] = %f\n", i, (float)depth / (2 * maxDepth));
    }

    return 0;
}

#define Tests simple, byHand, keywords
#define TestSetup prologue

#include "Test.h"

#include <stdlib.h>

// common trie
static TrieNode trie[TrieMaxSize];

static const TrieNode* const none = NULL; // not found constant

void prologue(void) {
    test.config.useColor = false;
}

#include <stdio.h>
void printKeys(uint nkeys, const char* keys[nkeys]) {
    for (int i = 0; i < nkeys; ++i)
        printf("%s\n", keys[i]);
}
void printTrie(const TrieNode* trie) {
    for (int i = 0; trie[i].letter; ++i)
        printf("%c:%d ", trie[i].letter, trie[i].childrenOffset);
    printf("\n");
}

void simple(void) {
    static const char* const keys[] = {
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
    assert(found == none);

    found = findInTrie(trie, "ev");
    assert(found == none, "Not a valid prefix!");

    found = findInTrie(trie, "evil");
    assertt(found - trie, eq, 9)("Not 8, because terminating zero counts!");

    found = findInTrie(trie, "endure");
    assertt(found - trie, eq, 5);

    found = findInTrie(trie, "enduri");
    assert(found == none, "bad letter in the end");

    found = findInTrie(trie, "evill");
    assert(found == none, "only prefix is matched");
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

void keywords(void) {
    static const char* const keys[] = {
        "any",
        "annonymous",
        "atom",
        "attach",
        "break",
        "breach",
        "close",
        "closure",
    };
    static const uint nkeys = sizeof keys / sizeof keys[0];

    const char* snapshot[nkeys];
    // just need to copy the top array
    memcpy(snapshot, keys, sizeof keys);

    uint nodes = buildTrie(trie, nkeys, snapshot);

}

