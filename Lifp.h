#ifndef Lifp_h
#define Lifp_h

#include <stdio.h>
#include "Common/Common.h"
#include "Debug.h"
#include "Error.h"
#include "StringTable.h"

/// General information about program we are compiling
typedef struct ProgramInfo {
    /// Optional absolute path for file with the program text
    const char* filepath;
    /// Non-null access to the program text
    FILE* text;

    StringTable* strings;

#ifdef ReportErrors
// for now coded inside macros the maximum amount of memory allocated
// for storing errors, so we can later show them all to the user
#define MaxBytesForErrors (4096 * 4)
    ErrorContainer errors;
    Error errors[MaxErrorss]
#endif

#ifdef LifpDebug
#define MaxProgramTokens (1 << 14)
    uint tokensCount;
    /// Parsed tokens of the current program
    TokenInfo tokens[MaxProgramTokens];
#endif

} ProgramInfo;

ProgramInfo* newProgram(FILE* text, const char* filename);

ErrorsOnly(
void reportErrors(ProgramInfo* info, FILE* out);
)

DebugOnly(
void addToken(ProgramInfo* info, const TokenInfo* token);
)


#endif /* Lifp_h */
