#define TokenGroups Keyword, Value, Other, Special

#define KeywordTokens Quote, Setq, Func, Lambda, Prog, Cond, While, Return, Break
#define ValueTokens Identifier, Integer, Real, True, False, Null
#define OtherTokens QuoteSymbol, OpenParen, CloseParen, Comment, Error, End

#define Tokens KeywordTokens, ValueTokens, OtherTokens

#define SpecialTokens KeywordTokens, True, False, Null

#define TokenIndex(token) token##Token
typedef enum: byte {
    // QuoteToken, SetqToken, ... , TokensCount
    JTMapComma(TokenIndex, Tokens),
    TokensCount
} Token;

const char* stringForToken(Token token);


#define TokenGroupIndex(group) group##TokenGroup
typedef enum: byte {
    JTMapComma(TokenGroupIndex, TokenGroups),
    TokenGroupsCount
} TokenGroup;


/// Represents an exact position in the program text
typedef struct Location {
    /// Starts from 1, lines are separated by the '\n' character
    uint line;
    /// Starts from 1 as well, all characters are counted as one column (even tab characters)
    uint column;

#ifdef ReportErrors
    /// To quickly navigate to the location without reading the whole text we need it's byte offset
    uint offset;
#endif
} Location;

inline bool isValidLocation(const Location* loc) {
    return loc->line >= 1 && loc->column >= 1;
}

#ifdef ReportErrors
inline uint locationLineOffset(const Location* loc) {
    return loc->offset - (loc->column - 1);
}
#endif


#ifdef LifpDebug

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

#endif /* LifpDebug */


#define Member(token) const Token token;
#define Binding(token) .token = TokenIndex(token)

static const struct {
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
