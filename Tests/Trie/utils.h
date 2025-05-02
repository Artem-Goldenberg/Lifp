#ifndef utils_h
#define utils_h

void append(const char* string) {
    strcpy(data.strings[data.top++], string);
}

uint max(uint a, uint b) {
    return b > a ? b : a;
}

char randomNodeChar(uint depth, uint maxDepth) {
    if (randomChance((float)depth / (maxDepth * 2)))
        return '\0';

    uint index = randomIndex(sizeof allowedSet);
    return allowedSet[index];
}

uint maxBranchesFor(uint depth, uint maxDepth) {
    float scale = 1.0 / (depth + 1);
    return max(scale * maxBranches, 1);
}

bool in(const char* key, uint size, const char* array[size]) {
    for (uint i = 0; i < size; ++i) {
        if (strcmp(array[i], key) == 0) {
            return true;
        }
    }
    return false;
}

#ifdef LifpDebug
void printKeys(uint nkeys, const char* keys[nkeys]) {
    for (int i = 0; i < nkeys; ++i)
        printf("%s\n", keys[i]);
}
void printTrie(const TrieNode* trie) {
    for (int i = 0; trie[i].letter; ++i)
        printf("%c:%d ", trie[i].letter, trie[i].childrenOffset);
    printf("\n");
}
#endif

#endif /* utils_h */
