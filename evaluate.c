#include "Utils.h"
#include "Lifp.tab.h"

int main(int argc, const char* argv[]) {
    int err = initialize(argv[1]);
    if (err) {
        printf("Provide a lifp source file to evaluate!\n");
        return err;
    }

    yyparse(program);

    if (reportErrors(program, stderr)) return -1;

    const List* root = program->syntaxRoot;

    const Element* result = evalElement((const Element*)program->syntaxRoot);

    PresentationContext context = {
        .out = stdout
    };

    print(result, &context);

    return 0;
}
