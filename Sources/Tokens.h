#ifndef Tokens_h
#define Tokens_h

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "macro.h"

typedef uint32_t uint;
typedef uint32_t uint32;
typedef uint16_t uint16;

/**
    The order of token categories is imperative, **don't change it!**
 */

#define SpecialForms \
    Quote, Setq, Func, Lambda, Prog, Cond, While, Return, Break

#define Atoms \
    Identifier, Integer, Real, True, False, Null

#define Parens \
    OpenParen, ClosedParen, OpenBracket, ClosedBracket, OpenBrace, ClosedBrace

#define MiscTokens \
    QuoteSymbol, Comment, Error, End, Whitespace

#define Tokens \
    SpecialForms, Atoms, Parens, MiscTokens


#define ToOne(_) 1

#define Count(...) MacroArgmap(ToOne, +, __VA_ARGS__)

#define WithTokenType(name) name##TokenType

static const unsigned int SpecialFormsTotal = Count(SpecialForms);
static const unsigned int LiteralsTotal = Count(Atoms);
static const unsigned int ParensTotal = Count(Parens);
static const unsigned int MiscTokensTotal = Count(MiscTokens);
static const unsigned int TokensTotal = Count(Tokens);

static const unsigned int SpecialFormsBorder = 1 + SpecialFormsTotal; // mind zero token
static const unsigned int LiteralsBorder = SpecialFormsBorder + LiteralsTotal;
static const unsigned int ParensBorder = LiteralsBorder + ParensTotal;
static const unsigned int MiscTokensBorder = ParensBorder + MiscTokensTotal;

typedef union {
    int integer;
    float real;
} LiteralValue;

typedef enum: uint16 {
    ZeroTokenType = 0, // dummy type to avoid zero values for other types
    MacroArgmap(WithTokenType, (,), Tokens)
} TokenType;

static const char* TokenString[] = {
#define ToArrayIndex(name) \
    [WithTokenType(name)] = #name

    MacroArgmap(ToArrayIndex, (,), Tokens)
};

static const TokenType CharToParen[] = {
    ['('] = OpenParenTokenType,
    [')'] = ClosedParenTokenType,
    ['['] = OpenBracketTokenType,
    [']'] = ClosedBracketTokenType,
    ['{'] = OpenBraceTokenType,
    ['}'] = ClosedBraceTokenType
};

inline static bool isSpecialFormToken(TokenType type) {
    // mind the zero token
    return 1 <= type && type < SpecialFormsBorder;
}

inline static bool isLiteralToken(TokenType type) {
    return SpecialFormsBorder <= type && type < LiteralsBorder;
}

inline static bool isParensToken(TokenType type) {
    return LiteralsBorder <= type && type < ParensBorder;
}

inline static bool isMiscToken(TokenType type) {
    return ParensBorder <= type && type < MiscTokensBorder;
}


static const struct {
#define ToMemberDecl(name) \
    const TokenType name;

    MacroArgmap(ToMemberDecl, , Tokens)

} token = {
#define ToMemberDef(name) \
    .name = WithTokenType(name)

    MacroArgmap(ToMemberDef, (,), Tokens)
};

typedef struct {
    uint line; // do we really need more than 64000 lines of code !?
    uint column;
} Position;

typedef struct {
    const struct {
        uint32 line; // do we really need more than 64000 lines of code !?
        uint16 startColumn, endColumn;
    };
    TokenType type;
} Token;

typedef struct {
    const Token base;
    const LiteralValue value;
} ValueToken;

typedef struct {
    Token base;
    char identifier[];
} IdentifierToken;


#endif /* Tokens_h */
