#ifndef Trie_h
#define Trie_h

#include <string.h>
#include "Common/Common.h"

#include "JustText/JustText.h"

typedef union TrieNode TrieNode;

union TrieNode {
    struct {
        byte letter: 7, isDirect: 1;
        /// number of nodes from the current node to it's first child node
        byte childrenOffset;
    };
    char chars[2];
};

/// This is a maximum safe size for a trie, because we are storing index offsets in one byte
#define TrieMaxSize 256

/**
 Builds a trie structure

 - Returns: Number of nodes in the `trie` array, that are built and ready to use as a trie.

 - Parameter trie:
    Pointer to the start of the trie nodes, which represent a trie, must be allocated to store
    at least `TrieMaxSize` trie nodes.
 - Parameter nkeys: Number of keys to be passed
 - Parameter keys:
    An array of strings to insert into trie, all empty strings and duplicates will be dropped
    While strings will not be changed, the array itself will be modified in the process of building
 */
uint buildTrie(TrieNode trie[TrieMaxSize], uint nkeys, const char* keys[nkeys]);

inline int incrementaConvert(int old, char digit) {
    return old * 10 + digit - '0';
}

static const void* incrementalLookup(const void* triePointer, char next) {
    static bool isDirectCompare = false;

    if (isDirectCompare) {
        const char* direct = (const char*)triePointer;

        if (next != *direct) return NULL;

        return *direct ? direct + 1 : direct;
    }

//    if (!next && node->isDirect)

    const TrieNode* node = triePointer;
    const TrieNode* end = node + node->childrenOffset;

    if (node < end) {
        if (node->letter != next)
            return node + 1;

        node += node->childrenOffset;
        return node + node->childrenOffset;
    }

    return NULL;
}

inline const TrieNode* findInTrie(const TrieNode* trie, const char* key) {
    const TrieNode* node = trie;
    const TrieNode* end = node + node->childrenOffset;

    while (node < end) {
        if (node->letter != *key) {
            node++;
            continue;
        }

        key++;

        if (!*key) return node->isDirect ? node : NULL;

        node += node->childrenOffset;
        end = node + node->childrenOffset;
    }

    if (node->isDirect && !node->childrenOffset) {
        DirectCompare:
        if (node++->letter != *key++) return NULL;

        const char* raw = (const char*)node;

        for (; *key && *key == *raw; key++, raw++);

        if (*key != *raw) return NULL;

        // raw should point to the last node of the raw string
        // just need to align it, so it is a valid TrieNode pointer
        return (const TrieNode*)((uintptr_t)raw & ~(sizeof(TrieNode) - 1));
    }

    return NULL;
}

/**
 Quickly searches for a `key` in a `trie`

 Assumes tries was build in a dfs order

 - Warning: `trie` parameter **must not be empty**, it must at leat have one direct node
*/
static inline int findInTrie2(const TrieNode* trie, const char* key) {
//    uint start = 0; //, end = trie[start].childrenOffset;
//    TrieNode node = trie[start];

    uint start = 0, end;
//    uint end = start + trie[start].childrenOffset;

//    while (*key) {
//        end = start + trie[start].childrenOffset;
//
//        for (; start < end; ++start)
//            if (trie[start].letter == *key)
//                break;
//
//        if (start == end) { // key not found or...
//            if (trie[start].isDirect)
//                strncmp((const char*)(trie + start), <#const char *s2#>, )
//            return -1;
//        }
//
//        key++;
//        // jump to children
//        start = trie[start].childrenOffset;
//    }

    end = start + trie[start].childrenOffset;
    TrieNode node = trie[start];

    while (start < end) {
        node = trie[start];
        // otherwise, if letters don't match, continue searching
        if (node.letter != *key) {
            start++;
            continue;
        }
        // letters matched!
        // eat the matched letter from the key
        key++;
        // if key is no more, we found a correct prefix, but is it in trie? node will tell
        if (!*key) return node.isDirect ? start : -1;
        // if still got some key left, jump to the children of a matched node
        uint parent = start;
        start += node.childrenOffset;
        node = trie[start];
        // this condition means there is only one word to compare with left
        if (node.isDirect && !node.childrenOffset) {
            // direct string of characters ahead
            // end of that string is where next node's children start
            uint uncle = parent + 1;
            if (uncle < end)
                end = uncle + trie[uncle].childrenOffset;
            else // if there is not next node, we will compare until \0
                end = start + 1 + (uint)strlen((const char*)(trie + start + 1)) / 2;
            goto DirectCompare;
        }
        // otherwise, we know how many children via first child's children offset
        end = start + node.childrenOffset;
    }

    return -1;

DirectCompare: // can't just strcmp, because first node is hacked to store only one letter
    if (trie[start++].letter != *key++) return -1;

    const char* raw = (const char*)(trie + start);
    char ch;

    while ((ch = *raw++ == *key++));

    return raw;
    // rest of the nodes store two chars each
//    int cmp = strncmp((const char*)(trie + start), key, 2 * (end - start));
//    return cmp == 0 ? end : -1;
}

#define PackedNode(char, skip) (TrieNode) { \
.letter = #char[0], \
.withPackedChildren = true, \
.childrenOffset = skip \
}
#define BranchNode(letter, skip)

#define Fork(char, jump, ...) \
(TrieNode) { \
.letter = #char[0], \
.isDirect = false, \
.childrenOffset = jump, \
}

#define Direct(char, jump) \
(TrieNode) { \
.letter = #char[0], \
.isDirect = true, \
.childrenOffset = jump, \
}

#define Pack(twoChars) (TrieNode) { .chars = #twoChars }

/**
 The first argument is ignored, it's used for readability on the call-site
 */
#define Packed(jump, doc, first, ...) \
(TrieNode) { \
.letter = #first[0], \
.childrenOffset = jump }__VA_OPT__(,) \
JTMapComma(Pack, __VA_ARGS__)

static const TrieNode keywordsTrie[42] = {
    Fork(f, 11),
    Direct(n, 13),
    Direct(t, 15),
    Direct(q, 17),
    Direct(s, 20),
    Direct(l, 22),
    Direct(p, 25),
    Direct(c, 27),
    Direct(w, 29),
    Direct(r, 32),
    Direct(b, 35),

    Direct(a, 38),
    Direct(u, 40),

    Packed(42, n, u, ll),
    Packed(42, t, r, ue),
    Packed(42, q, o, ut, e),
    Packed(42, s, e, tq),
    Packed(42, l, a, mb, da),
    Packed(42, p, r, og),
    Packed(42, c, o, nd),
    Packed(42, w, h, il, e),
    Packed(42, r, e, tu, rn),
    Packed(42, b, r, ea, k),

    Packed(42, fa, l, ee),
    Packed(42, fu, n, c)
};

#endif /* Trie_h */
