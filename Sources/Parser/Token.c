#include "Lifp.h"

#define StringEntry(token) [TokenIndex(token)] = #token
static const char* const tokenString[TokensCount] = {
    JTMapComma(StringEntry, Tokens)
};

const char* stringForToken(Token token) {
    return tokenString[token];
}

extern inline bool isValidLocation(const Location* loc);

#ifdef ReportErrors
extern inline uint locationLineOffset(const Location* loc);
#endif
