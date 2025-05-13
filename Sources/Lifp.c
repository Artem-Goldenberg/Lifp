#include "Lifp.h"

ProgramInfo* newProgram(FILE* text, const char* filename) {
    ProgramInfo* program = malloc(sizeof(ProgramInfo));
    if (!program) return NULL;

    program->filepath = filename;
    program->text = text;

    // initialized with default values
    program->strings = newStringTable(0, 0);

    return program;
}

#ifdef ReportErrors
void addIssue(ProgramInfo* info, const Issue* newIssue) {
    assert(info->issuesCount < MaxErrors);
    info->issues[info->issuesCount++] = *newIssue;
}

void reportErrors(const ProgramInfo* info, FILE* out) {
    if (!info->issuesCount) {
        fprintf(out, "No errors encountered");
        if (info->filepath)
            fprintf(out, " in %s", info->filepath);
        fprintf(out, "\n");
        return;
    }

    fprintf(out, "%d issue%s found", info->issuesCount, info->issuesCount == 1 ? "" : "s");
    if (info->filepath)
        fprintf(out, " in %s", info->filepath);
    fprintf(out, ":\n\n");

    printIssues(info->issuesCount, info->issues, info->text, out);
}
#endif

#ifdef LifpDebug
void addToken(ProgramInfo* info, const TokenInfo* token) {
    assert(info->tokensCount < MaxProgramTokens);
    info->tokens[info->tokensCount++] = *token;
}
#endif
