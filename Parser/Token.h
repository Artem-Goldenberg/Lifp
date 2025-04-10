#ifndef Token_h
#define Token_h

#include "Common/Common.h"
#include "JustText/JustText.h"

#define TokenGroups Keyword, Value, Other, Special

#define KeywordTokens Quote, Setq, Func, Lambda, Prog, Cond, While, Return, Break
#define ValueTokens   Identifier, Integer, Real, True, False, Null
#define OtherTokens   QuoteSymbol, OpenParen, CloseParen, Comment, Error, End

#define Tokens KeywordTokens, ValueTokens, OtherTokens

#define SpecialTokens KeywordTokens, True, False, Null

#define TokenIndex(token) token##Token

typedef enum: byte {
    // QuoteToken, SetqToken, ...
    JTMapComma(TokenIndex, Tokens),
    TokensCount
} Token;


const char* stringForToken(Token token);


typedef struct {
    uint line;
    uint column;
} Location;



#ifdef DEBUG

/// Unique identifier value to quickly determine equal strings
///
/// If debug mode is on, this is a pointer to the place in the string table, where
/// this string is stored. It is still unique, because string table doesn't allow duplicate entries.
/// And we can view it's value at any moment through the pointer.
typedef const char* ID;

typedef struct {
    Token token;
    Location start;
    Location end;
    union {
        int integer;
        float real;
        ID identifier;
    };
} TokenInfo;

#else

/// Unique identifier value to quickly determine equal strings
///
/// If debug mode is off, it is just an index in the string table,
/// because we don't need to know what the actual string was. All we need to know
/// is what other strings in the program matches this one. And comparing an index is enough to determine it
typedef uint ID;

#endif /* DEBUG */


#define Member(token) const Token token;
#define Binding(token) .token = TokenIndex(token)

static const struct { // TODO: maybe extern?
    // const Token Quote; const Token Setq; ...
    JTForEach(Member, Tokens)
}
/// Dummy global constant structure to provide namepsace for conviniently accessing token names
token = {
    // .Quote = QuoteToken, .Setq = SetqToken, ...
    JTMapComma(Binding, Tokens)
};

#undef Binding
#undef Member

#endif /* Token_h */
