#include "Error.h"

#ifdef ReportErrors

#include "JustText/JustText.h"
#include "Lifp.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SizeArrayEntryFor(error) [ErrorIndex(error)] = sizeof(error##Error)

static size_t errorSizes[] = {
    JTMapComma(SizeArrayEntryFor, Errors)
};

#define AlignmentArrayEntryFor(error) [ErrorIndex(error)] = _Alignof(error##Error)

static size_t errorAlignments[] = {
    JTMapComma(AlignmentArrayEntryFor, Errors)
};

static StaticString charGroupExpectedMessage[] = {
    [Digits] = "a digit",
    [DigitsOrDot] = "a digit or a '.'"
};

static void printLexerError(const LexerError* error, FILE* out) {
    fprintf(out, "On '%s' token: unexpected character '%c', was expecting %s",
            stringForToken(error->token), error->unexpected,
            charGroupExpectedMessage[error->expecting]);
}

static void printSyntaxError(const SyntaxError* error, FILE* out) {

}

static void printGeneralError(const GeneralError* error, FILE* out) {
    fprintf(out, "Unknown error: %s", error->message);;
}

static void printError(const Error* error, FILE* out) {
#define PrintErrorCaseFor(type) \
    case ErrorIndex(type): \
        print##type##Error((const type##Error*)error, out); \
        break;

    switch (error->type) {
        JTForEach(PrintErrorCaseFor, Errors)
    }
}

size_t errorSize(const Error* error) {
    return errorSizes[error->type];
}

size_t errorAlignment(const Error* error) {
    return errorAlignments[error->type];
}

bool isNoErrors(const ErrorContainer* errors) {
    return errors->top == errors->storage;
}

static size_t freeSpaceOf(const ErrorContainer* errors) {
    return errors->end - errors->top;
}

void addError(ErrorContainer* container, const Error* error) {
    assert(freeSpaceOf(container) >= errorSize(error));

    container->top = AlignedAt(errorAlignment(error), container->top);

    memcpy(container->top, error, errorSize(error));

    container->top += errorSize(error);
}

void printErrorsIn(ErrorContainer* container, FILE* text, FILE* out) {
    static const char prefix[] = "here ";

    for (byte* errorPtr = container->storage; errorPtr != container->end;
         errorPtr += errorSize((Error*)errorPtr)) {

        Error* error = (Error*)errorPtr;

        printError(error, out);

        int indent = fprintf(out, "%u | ", error->start.line);

        fseek(text, locationLineOffset(&error->start), SEEK_SET);

        uint linelen = 0;
        // move the line at the offset to out
        for (char c = getc(text); c != '\n' && c != EOF; c = getc(text), ++linelen)
            putc(c, out);

        fprintf(out, "\n");

        uint offset = 0;

        // print spacess to match the error position at the line above
        offset += fprintf(out, "%*s", indent + error->start.column - (uint)sizeof(prefix), "");
        // print the error marker
        offset += fprintf(out, "%s^", prefix);

        // print the squigles ~~~~ to match the line
        if (error->end.line != error->start.line) {
            for (uint i = offset; i < linelen; ++i)
                fputc('~', out);
            fprintf(out, "...");
        } else for (uint i = error->start.column; i < error->end.column; ++i)
            fputc('~', out);

        // finally exit
        fprintf(out, "\n\n");
    }

    fprintf(out, "\n");

}

#endif // ReportErrors
