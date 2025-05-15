#ifndef Utils_h
#define Utils_h

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "Lifp.h"

static bool isLifpSource(const char* name) {
    long len = strlen(name);
    return strcmp(name + len - 5, ".lifp") == 0;
}

static const char* withPrefix(const char* prefix, const char* str) {
    long len1 = strlen(prefix);
    long len2 = strlen(str);

    char* result = calloc(len1 + len2 + 1, sizeof(char));

    strcat(result, prefix);
    strcat(result, str);

    return result;
}

static ProgramInfo* program;

int initialize(const char* filepath) {
    FILE* some = fopen(filepath, "r");

    if (!some) return 1;

    initializeAllocator();

    static bool lexerInit = false;
    if (!lexerInit) {
        initializeLexing();
        lexerInit = true;
    }

    program = newProgram(some, filepath);

    program->lexer = newLexer(program);

    return 0;
}

#endif /* Utils_h */
