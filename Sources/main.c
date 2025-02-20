#include <stdio.h>
#include <stdlib.h>

#include "Tokens.h"
#include "Lexer.h"

void printUsage(void) {
    printf("Usage: lexifp <source-file>\n");
}


int main(int argc, const char * argv[]) {
    if (argc != 2) {
        printUsage();
        return -1;
    }

    FILE* source = fopen(argv[1], "r");

    if (!source) {
        perror("Failed to open the file");
        return -1;
    }

#define MaxTokens 1000

    Lexer l = doLex(source);

    for (Token** p = l.base; p != l.stream; p++) {
        Token* t = *p;
        printf("%d:(%d,%d) = %s",
               t->line, t->startColumn, t->endColumn,
               TokenString[t->type]
               );

        switch (t->type) {
            case token.Integer: {
                ValueToken* vt = (ValueToken*)t;
                printf(" (%d)", vt->value.integer);
            }; break;
            case token.Real: {
                ValueToken* vt = (ValueToken*)t;
                printf(" (%f)", vt->value.real);
            }; break;
            case token.Identifier: {
                IdentifierToken* it = (IdentifierToken*)t;
                printf(" (\"%s\")", it->identifier);
            }; break;
        }
        printf("\n");
    }


    return 0;
}
