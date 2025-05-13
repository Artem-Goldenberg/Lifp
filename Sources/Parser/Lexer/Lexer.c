#include "Lifp.h"

// TODO: read errors

struct Lexer {
    /// File or whatever from where we read the program text
    FILE* source;
    /// Current byte the lexer has officially stopped on
    char current;
    /// Peek byte, ie the next byte lexer will officially read
    char peek;

    /// Marks the location from where we've started reading current token
    Location tokenStart;
    /// Current official location of the lexer
    Location location;

    /// Buffer for reading in the identifier
    char buffer[MaxIdentifierLength + 1];
    uint top;

    union {
        int integer;
        float real;
        /// Identifier string index in the string table
        uint identifierIndex;
    } value;

    ProgramInfo* programInfo;
};


// Trie structure for quick search of keywords
static TrieNode keywordTrie[TrieMaxSize];
// Translated trie answers to the real token codes
static Token keywordTable[TrieMaxSize];

// Declare arrays of the `Token` enum members for each token group,
// so that we know exactly what tokens are in each group at runtime
static const Token SpecialTokensIndices[] = {
    JTMapComma(TokenIndex, SpecialTokens)
};

void initializeLexing(void) {
#define GetSize(key) sizeof #key
    static const uint keywordsCount = JTCount(SpecialTokens);
    // size of all keyword strings concatenated (including \0)
    static const uint totalSize = JTMap(GetSize, +, SpecialTokens);
    // actual array with all keywords strings concatenated (including \0)
    static char keywordsStorage[totalSize] = JTMap(JTString, "\0", SpecialTokens);

//#define OffsetsAccumulator(old, key) (JTHead old + sizeof #key, JTId old)
#define OffsetsAccumulator(old, key) (JTHead old + sizeof #key, JTId old)
    // array of pointers to strings in the keywordsStorage array,
    // offsets are calculated via a reduce macro (see assignment expressions below)
    static char* keywords[keywordsCount];

// produces a list of
// keywordsStorage + sizeof "Quote",
// keywordsStorage + sizeof "Quote" + sizeof "Setq",
// ... (but in reversed order)
#define KeywordOffsets \
    JTUnwrap(JTReduce(OffsetsAccumulator, (keywordsStorage), JTDropLast(SpecialTokens)))

    int i = 0;

#define AssignToNextKeyword(offset) keywords[i++] = offset;

    JTForEach(AssignToNextKeyword, KeywordOffsets)

    assert(i == keywordsCount);

    // keywords are all capitalized for now, need to lower case them
    for (uint i = 0; i < keywordsCount; ++i)
        *keywords[i] = tolower(*keywords[i]);

    // now we can build trie out of them (after that they all will point to \0)
    // char** is const char** too, but compiler cannot understand
    buildTrie(keywordTrie, keywordsCount, (const char**)keywords);

    // now fill out the keyword table, remember not to use keywords array
    // as it has been fried by the buildTrie call
    for (uint i = 0; i < keywordsCount; ++i) {
        Token keyword = SpecialTokensIndices[i];
        char* key = strdup(stringForToken(keyword));
        key[0] = tolower(key[0]);
        const TrieNode* node = findInTrie(keywordTrie, key);
        keywordTable[node - keywordTrie] = keyword;
        free(key);
    }
}


Lexer* newLexer(ProgramInfo* info) {
    Lexer* result = malloc(sizeof(Lexer));
    if (!result) return NULL;

    *result = (Lexer) {
        .source = info->text,
        .peek = getc(info->text),
        .location = (Location) {
            .line = 1, .column = 0
        }, // column is explicitly zeroed out, becuase we haven't read anything yet
        .programInfo = info,
        // other fields are zero
    };

    return result;
}


//#define Digits 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
#define Case(digit) case #digit[0]
#define CaseDigit JTMap(Case, :, Digits)

#define CaseSpacing case '\n': case ' ': case '\t'
#define CaseParen case '(': case ')'
#define CaseCommentStart case '#':

#define CaseAnySeparator CaseSpacing: CaseParen: \
    case '/': case '-': case '\'': case EOF

//#define Digits 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
#define DigitToChar(digit) #digit[0]

#define DigitCharacters JTMap(DigitToChar, Digits)
#define SpaceCharacters ' ', '\t', '\n'
#define PuctuationCharacters '(', ')', '\''
#define DelimitCharacters '(', ')'

#define Characters

#define CharacterGroups \
    Digit, Space, Punctuation, Delimit, EOF

#define SignCharacters '+' '-'

static bool isDelimeter(char next) {
    return
    isspace(next)  || next == '#'
    || next == '(' || next == ')'
    || next == EOF || next == '\'';
}

static bool isSign(char ch) {
    return ch == '+' || ch == '-';
}

static char readChar(Lexer* lexer) {
    if (lexer->current == EOF) // we done with the file
        return EOF; // will return EOF all the time and do nothing

#ifdef ReportErrors
    if (isValidLocation(&lexer->location))
        lexer->location.offset++;
#endif
    lexer->location.column++;

    if (lexer->current == '\n') {
        lexer->location.line++;
        lexer->location.column = 1;
    }

    lexer->current = lexer->peek;
    lexer->peek = getc(lexer->source);

    return lexer->current;
}

/// Just moves lexer to the position before the end of the line
static void skipToLineEnd(Lexer* lexer) {
    while (lexer->peek != '\n' && lexer->peek != EOF)
        readChar(lexer);
}

/// Similar function, just skips to delimeter, stopping lexer at last non-delimeter, different way of implementation
static void skipToDelimeter(Lexer* lexer) {
    while (!isDelimeter(lexer->peek)) readChar(lexer);
}

/// Continue filling the buffer (skipping if overflowed) until some separator character is reached
static uint fillUntilSeparator(Lexer* lexer, uint end) {
    while (true) {
        switch (lexer->peek) {
            CaseAnySeparator:
                lexer->buffer[end++] = '\0';
                return end;
            default:
                if (end < MaxIdentifierLength)
                    lexer->buffer[end++] = readChar(lexer);
        }
    }
}

static bool appendToBuffer(Lexer* lexer, char ch) {
    if (lexer->top >= MaxIdentifierLength)
        return false;
    lexer->buffer[lexer->top++] = ch;
    return true;
}

static bool readToBuffer(Lexer* lexer) {
    char next = readChar(lexer);
    return appendToBuffer(lexer, next);
}

static void sealTheBuffer(Lexer* lexer) {
    assert(lexer->top <= MaxIdentifierLength);
    lexer->buffer[lexer->top++] = '\0';
}

static void resetTheBuffer(Lexer* lexer) {
    assert(lexer->top <= MaxIdentifierLength + 1);
    lexer->top = 0;
}

/// Peek is pointing to the unexpected character
static Token numberFormatError(Lexer* lexer, bool isReal) {
    // read the error character and now we are at the error position
    char unexpected = readChar(lexer);

#ifdef ReportErrors
    Issue issue = {
        .type = issueType.LexerError,
        .start = lexer->location,
        .data.lexerError = {
            .expecting = isReal ? Digits : DigitsOrDot,
            .unexpected = unexpected,
            .token = isReal ? token.Real : token.Integer
        }
    };
#endif

    skipToDelimeter(lexer);

#ifdef ReportErrors
    issue.end = lexer->location;
    addIssue(lexer->programInfo, &issue);
#endif

    return token.Error;
}

static void longIdentifierWarning(Lexer* lexer) {
#ifdef ReportErrors
    Issue issue = {
        .type = issueType.LexerWarning,
        .start = lexer->location,
        .data.lexerWarning = TooLongIdentifierLexerWarning
    };
#endif

    skipToDelimeter(lexer);

#ifdef ReportErrors
    issue.end = lexer->location;
    addIssue(lexer->programInfo, &issue);
#endif
}

// this is called when lexer is already at a digit (or a sign and peek is a digit)
static Token readNumber(Lexer* lexer) {
    resetTheBuffer(lexer);
    appendToBuffer(lexer, lexer->current);

    bool isReal = false;
    while (true) {
        if (isnumber(lexer->peek)) {
            readToBuffer(lexer);
            continue;
        }

        if (lexer->peek == '.') {
            if (isReal) // already real, so this dot is a duplicate
                return numberFormatError(lexer, true);
            isReal = true;
            readToBuffer(lexer);
            continue;
        }

        if (isDelimeter(lexer->peek)) {
            sealTheBuffer(lexer);
            if (isReal) // TODO: overflow errors
                lexer->value.real = strtof(lexer->buffer, NULL);
            else
                lexer->value.integer = (int)strtol(lexer->buffer, NULL, 10);
            return isReal ? token.Real : token.Integer;
        }

        // some unexpected character ahead
        return numberFormatError(lexer, isReal);
    }
}

/// Called when lexer is at some unknown character (we assume all such character are parts of valid identifiers)
static Token readIdentifier(Lexer* lexer) {
    resetTheBuffer(lexer);
    appendToBuffer(lexer, lexer->current);

    // nothing can really go wrong during reading the identifier,
    // everything except for delimeters are counted as a part of this identifier
    while (!isDelimeter(lexer->peek))
        if (!readToBuffer(lexer)) // identifier is too long, report it and skip the tail
            longIdentifierWarning(lexer);

    sealTheBuffer(lexer);

    // determine whether or not this is a keyword using trie lookup we constructed earlier
    const TrieNode* index = findInTrie(keywordTrie, lexer->buffer);
    if (index) {
        assert(index - keywordTrie < TrieMaxSize);
        // identifier is a keyword
        return keywordTable[index - keywordTrie];
    }

    // if not a keyword, store identifier in a special table, which removes duplicates
    lexer->value.identifierIndex = insertIntoStringTable(lexer->programInfo->strings,
                                                         lexer->buffer);
    return token.Identifier;
}

static Token readToken(Lexer* lexer) {
    while (true) {
        char current = readChar(lexer);
        // skip all spaces
        if (isspace(current)) continue;

        lexer->tokenStart = lexer->location;

        if (isnumber(current))
            return readNumber(lexer);

        switch (current) {
            case '(':
                return token.OpenParen;
            case ')':
                return token.CloseParen;
            case '\'':
                return token.QuoteSymbol;
            case '#':
                skipToLineEnd(lexer);
                return token.Comment;
            case EOF:
                return token.End;
            case '-': case '+':
                if (isnumber(lexer->peek))
                    return readNumber(lexer);
                // break is intentionally avoided
            default: // anything else is part of an identifier
                return readIdentifier(lexer);
        }
    }
    assert(false);
}

#ifdef LifpDebug
static void appendTokenInfo(Lexer* lexer, Token type) {
    TokenInfo info = (TokenInfo) {
        .token = type,
        .start = lexer->tokenStart,
        .end = lexer->location,
    };
    switch (type) {
        case token.Integer:
            info.integer = lexer->value.integer;
            break;
        case token.Real:
            info.real = lexer->value.real;
            break;
        case token.Identifier:
            info.identifier = getFromStringTable(lexer->programInfo->strings, lexer->value.identifierIndex);
            break;
        default: break;
    }
    addToken(lexer->programInfo, &info);
}
#endif

int getInteger(const Lexer* lexer) {
    return lexer->value.integer;
}

float getReal(const Lexer* lexer) {
    return lexer->value.real;
}

ID getIdentifier(const Lexer* lexer) {
    uint index = lexer->value.identifierIndex;
#ifdef LifpDebug
    return getFromStringTable(lexer->programInfo->strings, index);
#else
    return index;
#endif
}

Location getTokenStartLocation(const Lexer* lexer) {
    return lexer->tokenStart;
}

Location getCurrentLocation(const Lexer* lexer) {
    return lexer->location;
}

Token getToken(Lexer* lexer) {
    Token result = readToken(lexer);

    while (result == token.Error) {
        result = readToken(lexer);
    }

    DebugOnly(appendTokenInfo(lexer, result));

    return result;
}
