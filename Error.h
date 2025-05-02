#ifndef Error_h
#define Error_h

#include <stdio.h>

// when debug is on, we are reporting errors as well
#ifdef LifpDebug
#define ReportErrors
#endif

#include "Token.h"

#ifdef ReportErrors

#define Errors Lexer, Syntax, General
#define ErrorIndex(error) error##ErrorType

typedef enum: byte {
    LexerErrorType,
    SyntaxErrorType,
    GeneralErrorType
} ErrorType;

typedef enum: byte {
    ErrorSeverity,
    WarningSeverity,
    NoteSeverity
} Severity;

typedef enum: byte {
    Digits,
    DigitsOrDot
} CharGroup;

typedef enum: byte {
    Keywords
} TokenGroup;

/// Error which occured during separating program text into lexemes
typedef struct LexerError {
    /// During crafting what token
    Token token;
    /// Character lexer did not expect to find next
    char unexpected;
    /// Characters lexer was expecting to find next
    CharGroup expecting;
} LexerError;

/// Error which occured during program parsing
typedef struct SyntaxError {
    // TODO: this
    /// on what
    Token token;
    Token unexpected;
    TokenGroup expecting;
} SyntaxError;

/// Error which does not fall into any of the predefined categories
typedef struct GeneralError {
    /// description
    const char* message;
} GeneralError;

/// Base structure for all the errors
typedef struct Error {
    /// nature
    ErrorType type;
    /// how bad?
    Severity severity;
    /// where
    Location start;
    Location end;
    union {
        LexerError lexer;
        SyntaxError syntax;
        GeneralError general;
    } data;
} Error;

/// Used to manage the contiguous storage of errors of different types
typedef struct ErrorContainer {
    byte* storage;
    byte* top;
    byte* end;
} ErrorContainer;

void addError(ErrorContainer* container, const Error* errorToAdd);
void printErrorsIn(ErrorContainer* container, FILE* programText, FILE* out);

bool isNoErrors(const ErrorContainer* errors);

#define ErrorsOnly(...) __VA_ARGS__

#else

#define ErrorsOnly(...)

#endif

#endif /* Error_h */
