#ifdef ReportErrors

#define IssueTypes LexerError, LexerWarning, SyntaxError, GeneralError

UtilDefinitionsFor(IssueType, issueType)

//typedef enum: byte {
//    LexerErrorType,
//    SyntaxErrorType,
//    GeneralErrorType
//} ErrorType;

typedef enum: byte {
    Digits,
    DigitsOrDot
} CharGroup;

/// Error which occured during separating program text into lexemes
typedef struct LexerError {
    /// During crafting what token
    Token token;
    /// Character lexer did not expect to find next
    char unexpected;
    /// Characters lexer was expecting to find next
    CharGroup expecting;
} LexerError;

typedef enum LexerWaring {
    TooLongIdentifierLexerWarning
} LexerWarning;

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
typedef struct Issue {
    /// nature
    IssueType type;
    /// where
    Location start;
    Location end;
    union {
        LexerWarning lexerWarning;
        LexerError lexerError;
        SyntaxError syntaxError;
        GeneralError generalError;
    } data;
} Issue;

void printIssues(uint count, const Issue errors[count], FILE* text, FILE* out);

#define ErrorsOnly(...) __VA_ARGS__

#else

#define ErrorsOnly(...)

#endif // ReportErrors
