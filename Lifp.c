#include "Lifp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

ProgramInfo* newProgram(FILE* text, const char* filename) {
    ProgramInfo* program = malloc(sizeof(ProgramInfo));
    if (!program) return NULL;

    program->filepath = filename;
    program->text = text;

    // initialized with default values
    program->strings = newStringTable(0, 0);

#ifdef ReportErrors
    program->errors.storage = malloc(MaxBytesForErrors);
    if (!program->errors.storage) return NULL;
    program->errors.top = program->errors.storage;
    program->errors.end = program->errors.storage + MaxBytesForErrors;
#endif

    return program;
}

#ifdef ReportErrors
void reportErrors(ProgramInfo* info, FILE* out) {
    if (isNoErrors(&info->errors)) {
        fprintf(out, "No errors encountered");
        if (info->filepath)
            fprintf(out, " in %s", info->filepath);
        fprintf(out, "\n");
        return;
    }

    fprintf(out, "Errors found");
    if (info->filepath)
        fprintf(out, " in %s", info->filepath);
    fprintf(out, ":\n");

    printErrorsIn(&info->errors, info->text, out);
}
#endif

#ifdef LifpDebug
void addToken(ProgramInfo* info, const TokenInfo* token) {
    assert(info->tokensCount < MaxProgramTokens);
    info->tokens[info->tokensCount++] = *token;
}
#endif

