#include "Lifp.h"

#ifdef ReportErrors

static StaticString charGroupExpectedMessage[] = {
    [Digits] = "a digit",
    [DigitsOrDot] = "a digit or a '.'"
};

static void printLexerWarning(const LexerWarning* warning, FILE* out) {
    switch (*warning) {
        case TooLongIdentifierLexerWarning:
            fprintf(out, 
                    "Warning: Identifier longer than %d characters detected,\n"
                    "\tall overflowing characters are skipped and not considered\n",
                    MaxIdentifierLength);
            break;
        default: assert(false);
    }
}

static void printLexerError(const LexerError* error, FILE* out) {
    fprintf(out, "In '%s' token: unexpected character '%c', was expecting %s\n",
            stringForToken(error->token), error->unexpected,
            charGroupExpectedMessage[error->expecting]);
}

static void printSyntaxError(const SyntaxError* error, FILE* out) {
    const GeneralError* casted = (const GeneralError*)error;
    fprintf(out, "%s\n", casted->message);
}

static void printGeneralError(const GeneralError* error, FILE* out) {
    fprintf(out, "Error: %s", error->message);;
}

static void printIssue(const Issue* issue, FILE* out) {
#define PrintErrorCaseFor(Type) \
    case issueType.Type: \
        print##Type((const Type*)&issue->data, out); \
        break;

    fprintf(out, "[%d:%d - %d:%d] ",
            issue->start.line, issue->start.column,
            issue->end.line, issue->end.column);

    switch (issue->type) {
        JTForEach(PrintErrorCaseFor, IssueTypes)
        default: assert(false);
    }
}

void printIssues(uint count, const Issue issues[count], FILE* text, FILE* out) {
    static const char prefix[] = "";

    for (uint i = 0; i < count; ++i) {
        const Issue* issue = issues + i;

        printIssue(issue, out);

        int indent = fprintf(out, "%u | ", issue->start.line);

        fseek(text, locationLineOffset(&issue->start), SEEK_SET);

        uint linelen = 0;
        // move the line at the offset to out
        for (char c = getc(text); c != '\n' && c != EOF; c = getc(text), ++linelen)
            putc(c, out);

        fprintf(out, "\n");

        uint offset = 0;

        // print spacess to match the error position at the line above
        offset += fprintf(out, "%*s", indent + issue->start.column - (uint)sizeof(prefix), "");
        // print the error marker
        offset += fprintf(out, "%s^", prefix);

        // print the squigles ~~~~ to match the line
        if (issue->end.line != issue->start.line) {
            for (uint i = offset; i < linelen; ++i)
                fputc('~', out);
            fprintf(out, "...");
        } else for (uint i = issue->start.column; i < issue->end.column; ++i)
            fputc('~', out);

        // finally exit
        fprintf(out, "\n\n");
    }

    fprintf(out, "\n");

}

#endif // ReportErrors
