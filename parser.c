#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include <assert.h>
#include <stdlib.h>

#include "Utils.h"
#include "Lifp.tab.h"

yytoken_kind_t translation[TokensCount] = {
    [token.Quote] = QUOTE,
    [token.Setq] = SETQ,
    [token.Func] = FUNC,
    [token.Lambda] = LAMBDA,
    [token.Prog] = PROG,
    [token.Cond] = COND,
    [token.While] = WHILE,
    [token.Return] = RETURN,
    [token.Break] = BREAK,
    [token.Identifier] = IDENTIFIER,
    [token.Integer] = INTEGER,
    [token.Real] = REAL,
    [token.True] = TRUE,
    [token.False] = FALSE,
    [token.Null] = NIL,
    [token.QuoteSymbol] = QUOTESYMBOL,
    [token.OpenParen] = '(',
    [token.CloseParen] = ')',
//    [token.QuoteSymbol] = QUOTESYMBOL,
//    [token.OpenParen] = OPENPAREN,
//    [token.CloseParen] = CLOSEPAREN,
    [token.End] = YYEOF
};

int yylex(void) {
    Token t;

    // skip all commentss
    do {
        t = getToken(lexer);
        assert(t < TokensCount);
    } while (t == token.Comment);

    // set yylval if tokens has a value
    switch (t) {
        case token.Integer:
            yylval.INTEGER = getInteger(lexer);
            break;
        case token.Real:
            yylval.REAL = getReal(lexer);
            break;
        case token.Identifier:
            yylval.IDENTIFIER = getIdentifier(lexer);
            break;
        default: break;
    }

    return translation[t];
}

void yyerror(ProgramInfo* program, const char* message) {
    Issue error = {
        .type = issueType.SyntaxError,
        .start = getTokenStartLocation(lexer),
        .end = getCurrentLocation(lexer),
        .data = {
            .generalError = (GeneralError) {
                .message = strdup(message)
            }
        }
    };
    addIssue(program, &error);
//    fprintf(stderr, "%s\n", message);
}

int main(int argc, const char* argv[]) {
    int err = initialize(argv[1]);
    if (err) {
        printf("Provide file to parse!\n");
        return err;
    }

    yyparse(program);

    reportErrors(program, stderr);

    PresentationContext context = {
        .out = stdout,
    };

    print((Element*)program->syntaxRoot, &context);

    return 0;
}
