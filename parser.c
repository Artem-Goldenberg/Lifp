#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include <assert.h>
#include <stdlib.h>

#include "Utils.h"
#include "Lifp.tab.h"


int main(int argc, const char* argv[]) {
    if (argc != 2) {
        printf("Provide file to parse!\n");
        return EXIT_FAILURE;
    }
    int err = initialize(argv[1]);
    if (err) {
        printf("Error opening the file\n");
        return err;
    }

    IdentifierList* globals = node.identifierList();
    node.finalize();

    program->syntaxRoot = (Prog*)node.prog(globals);

    yyparse(program);

    reportErrors(program, stderr);

    PresentationContext context = {
        .out = stdout,
    };

    print((Element*)program->syntaxRoot, &context);

    return 0;
}
