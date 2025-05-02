#include <stdio.h>
#include <dirent.h>

#include "Utils.h"

static const char* const dirs[] = {
    "Examples/lambda-calculus/",
    "Examples/Basics/",
    "Examples/Medium/",
    "Examples/Advanced/",
    "Examples/bad/lex/",
//    "Examples/bad"
};

static bool printTokens(const char* filepath, FILE* out) {
    int err = initialize(filepath);
    if (err) return false;

    Token t;
    do {
        t = getToken(lexer);
    } while (t != token.End);

    for (uint i = 0; i < program->tokensCount; ++i) {
        TokenInfo info = program->tokens[i];

        fprintf(out, "%s [%d:%d - %d:%d]", stringForToken(info.token),
               info.start.line, info.start.column,
               info.end.line, info.end.column);

        switch (info.token) {
            case token.Integer:
                fprintf(out, " %d", info.integer);
                break;
            case token.Real:
                fprintf(out, " %g", info.real);
                break;
            case token.Identifier:
                fprintf(out, " \"%s\"", info.identifier);
                break;
            default: break;
        }
        fprintf(out, "\n");
    }

    fprintf(out, "\n\n===============================\n");

    reportErrors(program, out);

    return true;
}

int main(int argc, const char * argv[]) {
    if (argc > 1) {
        printTokens(argv[1], stdout);
    }

    DIR *d;
    struct dirent *entry;

    for (int i = 0; i < sizeof dirs / sizeof dirs[0]; ++i) {
        const char* dir = withPrefix("Tests/", dirs[i]);
        d = opendir(dir);
        assert(d);
        while ((entry = readdir(d)) != NULL) {
            if (isLifpSource(entry->d_name)) {
                const char* filename = withPrefix(dirs[i], entry->d_name);
                const char* outputPath = withPrefix("Tests/Output/", filename);
                filename = withPrefix("Tests/", filename);
                printf("%s:\n\n", filename);
                char* out = malloc(strlen(outputPath) + 4);
                strcat(out, outputPath);
                strcat(out, ".out");
                FILE* output = fopen(out, "w");
                assert(output);
                printTokens(filename, output);
            }
        }
        closedir(d);
    }

    return 0;
}
