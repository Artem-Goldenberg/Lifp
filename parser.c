#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include <assert.h>
#include <stdlib.h>

#include "Lifp.h"
#include "Lexer.h"
#include "Syntax.h"
#include "Printer.h"

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
    [token.OpenParen] = OPENPAREN,
    [token.CloseParen] = CLOSEPAREN,
    [token.Comment] = COMMENT,
};

int yylex(void) {
    Token t = getToken(lexer);
    assert(t < TokensCount);
    return translation[t];
}

void yyerror(List** root, const char* message) {
    fprintf(stderr, "%s\n", message);
}

int main(int argc, const char* argv[]) {

    int abc = 30;
    int b = 30;

    int a = JTConcat(b);

    int err = initialize(argv[1]);
    if (err) return err;

    List* root;
    yyparse(&root);

    printf("here");

    return 0;
}
