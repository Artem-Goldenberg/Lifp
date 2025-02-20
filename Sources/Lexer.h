//

#ifndef Lexer_h
#define Lexer_h

#include <stdio.h>
#include "Tokens.h"

typedef struct {
    FILE* source;

    /// number of the current line
    uint currentLine;
    /// pointer to the next character in the line
    char* next;

    /// pointer to the error message (`NULL` if has no errors)
    char* error;

    Token** base;
    Token** stream;
} Lexer;

Lexer doLex(FILE* source);

#endif /* Lexer_h */
