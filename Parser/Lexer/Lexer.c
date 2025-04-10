#include "Lexer.h"
#include "Trie.h"
#include "StringTable.h"

#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

// TODO: read errors

typedef enum {
    Digits,

} CharGroup;

struct Lexer {
    FILE* source;
    char current;
    char peek;

    Location tokenStart;
    Location location;
    uint lineOffset;

    /// buffer for reading the identifier
    char buffer[MaxIdentifierLength + 1];
    uint top;

    StringTable* strings;

    union {
        int integer;
        float real;
        /// Identifier string index in the string table
        uint identifierIndex;
    } value;

    struct {
        Location location;
        uint offset;
        char unexpected;
        Token expectedToken;
        CharGroup expecting;
        ErrorKind type;
    } error;

    ProgramInfo* programInfo;
};


// Trie structure for quick search of keywords
static TrieNode keywordTrie[TrieMaxSize];
// Translated trie answers to the real token codes
static Token keywordTable[TrieMaxSize];


// Declare arrays of the `Token` enum members for each token group,
// so that we know exactly what tokens are in each group at runtime
#define GroupIndices(group) \
static const Token group##Tokens##Indices[] = { \
    JTInnerMapComma(TokenIndex, group##Tokens) \
};
JTForEach(GroupIndices, TokenGroups)


void initializeLexing(void) {
#define GetSize(key) sizeof #key
    static const uint keywordsCount = JTCount(SpecialTokens);
    // size of all keyword strings concatenated (including \0)
    static const uint totalSize = JTMap(GetSize, +, SpecialTokens);
    // actual array with all keywords strings concatenated (including \0)
    static char keywordsStorage[totalSize] = JTMap(JTString, "\0", SpecialTokens);

#define OffsetsAccumulator(old, key) (JTHead old + sizeof #key, JTId old)
    // array of pointers to strings in the keywordsStorage array,
    // offsets are calculated via a reduce macro (see member expressions below)
    static char* keywords[keywordsCount] = {
        // keywordsStorage + sizeof "Quote",
        // keywordsStorage + sizeof "Quote" + sizeof "Setq",
        // ... (but in reversed order)
        JTUnwrap(JTReduce(OffsetsAccumulator, (keywordsStorage), JTDropLast(SpecialTokens)))
    };

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


Lexer* newLexer(FILE* source, ProgramInfo* info) {
    Lexer* result = malloc(sizeof(Lexer));
    if (!result) return NULL;

    *result = (Lexer) {
        .source = source,
        .peek = getc(source),
        .location = (Location) {
            .line = 1, .column = 0
        }, // column is explicitly zeroed out, becuase we haven't read anything yet
        // other fields are zero
        .strings = newStringTable(1001, 0),
        .programInfo = info
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

static bool isLetter(char next) {
    return !isDelimeter(next) && !isnumber(next) && !isSign(next);
}

static char readChar(Lexer* lexer) {
    if (lexer->current == EOF) // we done with the file
        return EOF; // will return EOF all the time and do nothing

    lexer->location.column++;

    if (lexer->current == '\n') {
        lexer->lineOffset += lexer->location.column - 1;
        lexer->location.line++;
        lexer->location.column = 1;
    }

    lexer->current = lexer->peek;
    lexer->peek = getc(lexer->source);

    return lexer->current;
}

/// Better name for the weird for loop, just moves lexer to the end of the line
static void skipLine(Lexer* lexer) {
    for (char next = lexer->current;
         next != '\n' && next != EOF;
         next = readChar(lexer));
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
    assert(lexer->top <= MaxIdentifierLength);
    lexer->top = 0;
}

/// Peek is pointing to the unexpected character
static Token numberFormatError(Lexer* lexer, bool isReal) {
    // read the error character and now we are at the error position
    char unexpected = readChar(lexer);

    lexer->error.location = lexer->location;
    lexer->error.unexpected = unexpected;
    lexer->error.expecting = Digits;
    lexer->error.type = NumberFormat;
    lexer->error.expectedToken = isReal ? token.Real : token.Integer;

    skipToDelimeter(lexer);
    return token.Error;
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
    while (!isDelimeter(lexer->peek)) readToBuffer(lexer);

    sealTheBuffer(lexer);

    // determine whether or not this is a keyword using trie lookup we constructed earlier
    const TrieNode* index = findInTrie(keywordTrie, lexer->buffer);
    if (index) {
        assert(index - keywordTrie < TrieMaxSize);
        // identifier is a keyword
        return keywordTable[index - keywordTrie];
    }

    // if not a keyword, store identifier in a special table, which removes duplicates
    lexer->value.identifierIndex = insertIntoStringTable(lexer->strings, lexer->buffer);

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
                skipLine(lexer);
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

static void handleError(const Lexer* lexer) {
    assert(lexer->error.type);

    static char message[128];
    switch (lexer->error.type) { // only one error for now
        case NumberFormat:
            sprintf(message, "Unexpected character '%c' in a number token",
                    lexer->error.unexpected);
            break;
        default:
            assert(false);
    }

    addError(lexer->programInfo,
             lexer->error.location,
             lexer->lineOffset,
             lexer->error.type,
             lexer->error.expectedToken,
             message);
}

#ifdef DEBUG
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
            info.identifier = getFromStringTable(lexer->strings, lexer->value.identifierIndex);
            break;
        default: break;
    }
    addToken(lexer->programInfo, &info);
}
#endif

Token getToken(Lexer* lexer) {
    Token result = readToken(lexer);

    if (result == token.Error) {
        handleError(lexer);
        return getToken(lexer);
    }

    DebugOnly(appendTokenInfo(lexer, result));

    return result;
}
