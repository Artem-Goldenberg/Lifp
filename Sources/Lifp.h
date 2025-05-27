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

// predefine interpretation context, as some syntax elements require it as a parameter
typedef struct Context Context;

#include "Syntax.h"
#include "Lifp.tab.h"

#include "Printer.h"
#include "Builtin.h"
#include "Interpreter.h"

// MARK: - General compiler context

/// General information about program we are compiling
struct ProgramInfo {
    /// Optional absolute path for file with the program text
    const char* filepath;
    /// Non-null access to the program text
    FILE* text;

    Lexer* lexer;

    Prog* syntaxRoot;

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

/// Used to create new program info to guide the compilation cycle
ProgramInfo* newProgram(FILE* text, const char* filename);

/// Before using string as identifier anywhere in the compilation, you must register it into the strings table,
/// and use an `ID` returned as a unique link for the string you provided
ID registerIdentifier(ProgramInfo* info, const char* name);

#ifdef ReportErrors
void addIssue(ProgramInfo* info, const Issue* newIssue);
bool reportErrors(const ProgramInfo* info, FILE* out);
#endif

#ifdef LifpDebug
void addToken(ProgramInfo* info, const TokenInfo* token);
#endif

#endif /* Lifp_h */
