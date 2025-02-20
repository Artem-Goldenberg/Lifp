#include "Lexer.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include "Tokens.h"

#define MaxTokens 10000

static bool tokenizeLine(Lexer* l);

static bool tokenize(Lexer* l) {
    size_t capacity = 0;
    char* base = NULL;
    while (getline(&base, &capacity, l->source) > 0) {
        l->next = base;
        if (!tokenizeLine(l)) {
            if (l->error) {
                fprintf(stderr, "%s\n", l->error);
                return false;
            }
            assert(*l->next == EOF);
            return true;
        }
        l->currentLine++;
    }
    return true;
}

Lexer doLex(FILE* source) {
    Token** space = malloc(sizeof(Token*) * 1000);
    if (!space) {
        exit(-1);
    }
    Lexer l = {
        .source = source,
        .currentLine = 1,
        .next = NULL,
        .error = NULL,
        .base = space,
        .stream = space
    };
    tokenize(&l);
    return l;
}

static bool appendToken(Lexer* l, const char* lineBase);

static bool tokenizeLine(Lexer* l) {
    const char* lineBase = l->next;
    while (*l->next != '\n') {
        if (!appendToken(l, lineBase)) {
            return false;
        }
    }
    return true;
}

static TokenType getTypeFor(Lexer* l, LiteralValue* storage);

static bool appendToken(Lexer* l, const char* lineBase) {
    LiteralValue number;

    const char* old = l->next;

    TokenType type = getTypeFor(l, &number);

    Token base = {
        .line = l->currentLine,
        .startColumn = old - lineBase + 1,
        .endColumn = l->next - lineBase + 1,
        .type = type
    };

    switch (type) {
        case token.Integer:
        case token.Real: {
            ValueToken t = {
                .base = base,
                .value = number
            };
            *l->stream = malloc(sizeof t);
            memcpy(*l->stream, &t, sizeof t);
            l->stream++;

//            l->stream = (Token*)((char*)l->stream + sizeof t);
        }; break;
        case token.Identifier: {
            *l->stream = malloc(sizeof(Token) + (l->next - old) + 8);

            IdentifierToken* t = (IdentifierToken*)(*l->stream);

            **l->stream = base;
            memcpy(&t->identifier, old, l->next - old);
            l->stream++;
//
//            t->base = base;
//            l->stream += sizeof base + l->next - old;
        }; break;

        case token.Comment:
            *l->stream = malloc(sizeof base);
            while (*l->next != EOF && *l->next != '\n') l->next++;
            base.endColumn = l->next - lineBase;
            **l->stream = base;
            l->stream++;
            return true;

        case token.End:
        case token.Error:
            return false;

        default:
            *l->stream = malloc(sizeof base);
            **l->stream = base;
            l->stream++;
    }
    return true;
}

#define Digits 1, 2, 3, 4, 5, 6, 7, 8, 9, 0

#define NumberStarts +, -, Digits

#define ToString(sym) #sym

#define NumberChars \
    NumberDigits, .,

#define ToCase(start) \
    case #start[0]

#define CaseNumberStart MacroArgmap(ToCase, :, NumberStarts)

bool insideToken(char next) {
    return !isspace(next) && next != '\0' && next != EOF;
}

uint HashFunction(const char* start, const char* end);

static const uint maxHash = 211;
static const TokenType hashToKeywordToken[maxHash] = {
    [2] = token.Null,
    [104] = token.True,
    [29] = token.False,
    [141] = token.Quote,
    [61] = token.Setq,
    [173] = token.Func,
    [147] = token.Lambda,
    [84] = token.Prog,
    [65] = token.Cond,
    [1] = token.While,
    [100] = token.Return,
    [196] = token.Break
};

static TokenType getTypeFor(Lexer* l, LiteralValue* storage) {
    const char* old = l->next;
    switch (*l->next++) {
        case '(': return token.OpenParen;
        case ')': return token.ClosedParen;
        case '[': return token.OpenBracket;
        case ']': return token.ClosedBracket;
        case '{': return token.OpenBrace;
        case '}': return token.ClosedBrace;
        case '\'': return token.QuoteSymbol;
        CaseNumberStart:
            storage->integer = (int)strtol(old, &l->next, 10);
            if (*l->next == '.') {
                storage->real = strtof(old, &l->next);
                if (insideToken(*l->next)) {
                    l->error = "expected digit or space";
                    return token.Error;
                }
                return token.Real;
            }
            if (*l->next == '-') { // annoying case for comments starting with `--`
                if (old[1] == '-')
                    return token.Comment;
                l->error = "expected digit or another '-' for comment";
                return token.Error;
            }
//            if (insideToken(*l->next)) {
//                l->error = "expected digit or '.' or space";
//                return token.Error;
//            }
            return token.Integer;
        case '/':
            if (old[1] == '/') return token.Comment;
            l->error = "expected another '/' for comment";
            return token.Error;
        case EOF: return token.End;

        default: // arbitrary character
            if (isspace(l->next[-1])) {
                while (isspace(*l->next)) l->next++;
                return token.Whitespace;
            }
            while (isalnum(*l->next)) l->next++;
            uint tokenHash = HashFunction(old, l->next);
            TokenType result = hashToKeywordToken[tokenHash];
            if (result) return result;
            return token.Identifier;
    }
}


uint HashFunction(const char* start, const char* end) {
    uint g; // for calculating hash
    const uint hash_mask = 0xF0000000;
    uint hash_value = 0;
    for (; start != end; start++) {
        // Calculating hash: see Dragon Book, Fig. 7.35
        hash_value = (hash_value << 4) + *start;
        if ((g = hash_value & hash_mask) != 0) {
            // hash_value ^= g >> 24 ^ g;
            hash_value = hash_value ^ (hash_value >> 24);
            hash_value ^= g;
        }
    }
    // final hash value for identifier
    return hash_value % maxHash;
}
