#include "Token.h"

#define StringEntry(token) [TokenIndex(token)] = #token
static const char* const tokenString[TokensCount] = {
    JTMapComma(StringEntry, Tokens)
};

const char* stringForToken(Token token) {
    return tokenString[token];
}
