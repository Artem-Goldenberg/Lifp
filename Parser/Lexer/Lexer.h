#ifndef Lexer_h
#define Lexer_h

#include <stdio.h>
#include "Lifp.h"
#include "Token.h"

#define MaxIdentifierLength 4095

/// Main object for the lexing interface
///
/// ``Lexer`` is a partial object, user can access it only through passing pointers to it to the
/// functions defined in this header
typedef struct Lexer Lexer;

/// Creates and initializes all necessary data structures needed for lexing
///
/// - Warning: You have to call this method before calling anything else!
void initializeLexing(void);

/// Use to create a new lexer object on the given file
///
/// If debug mode is on, optionally provide a pointer to the `ProgramInfo` structure
/// which contains the `tokens` array, lexer will save the processed tokens in this array.
/// Pass `NULL` if debug mode is on but you don't want that.
Lexer* newLexer(FILE* programInput DebugArg(ProgramInfo* info));

/// After creating a new lexer, use this function to retrieve tokens on by one from the file you gave to lexer
Token getToken(Lexer* lexer);

#endif /* Lexer_h */
