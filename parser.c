#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include <assert.h>
#include <stdlib.h>

#include "Utils.h"
#include "Lifp.tab.h"


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
