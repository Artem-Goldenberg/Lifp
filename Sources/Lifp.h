#ifndef Lifp_h
#define Lifp_h

// MARK: - Dependencies:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#include "Common/Common.h"
#include "JustText/JustText.h"

// MARK: - Our Library Setup:

#include "Trie.h"
#include "Allocator.h"
#include "StringTable.h"

#include "DebugUtils.h"

// when debug is on, we are reporting errors as well
#ifdef LifpDebug
#define ReportErrors
#endif

typedef struct ProgramInfo ProgramInfo;

#include "CodeGeneration.h"
#include "EnumGeneration.h"
#include "VisitorGeneration.h"

#include "Token.h"
#include "Issue.h"
#include "Lexer.h"

#include "Syntax.h"
#include "Lifp.tab.h"

#include "Printer.h"
#include "Interpreter.h"

// MARK: - General compiler context

/// General information about program we are compiling
struct ProgramInfo {
    /// Optional absolute path for file with the program text
    const char* filepath;
    /// Non-null access to the program text
    FILE* text;

    Lexer* lexer;

    List* syntaxRoot;

    StringTable* strings;

#ifdef ReportErrors
#define MaxErrors 128
    uint issuesCount;
    Issue issues[MaxErrors];
#endif

#ifdef LifpDebug
#define MaxProgramTokens (1 << 14)
    uint tokensCount;
    /// Parsed tokens of the current program
    TokenInfo tokens[MaxProgramTokens];
#endif

};

ProgramInfo* newProgram(FILE* text, const char* filename);

#ifdef ReportErrors
void addIssue(ProgramInfo* info, const Issue* newIssue);
bool reportErrors(const ProgramInfo* info, FILE* out);
#endif

#ifdef LifpDebug
void addToken(ProgramInfo* info, const TokenInfo* token);
#endif

#endif /* Lifp_h */
