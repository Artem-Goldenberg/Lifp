#ifndef Printer_h
#define Printer_h

#include "Syntax.h"
#include <assert.h>

__printflike(1, 2)
typedef void Printer(const char* format, ...);

static int indent = 0;

static void printInteger(const Integer* integer, Printer* out) {
    out("Literal[Integer[%d]]", integer->value);
}

static void printReal(const Real* real, Printer* out) {
    out("Literal[Integer[%g]]", real->value);
}

static void printNode(const Element* element, Printer* out);

static void printNewLine(Printer* out) {
    out("\n%*s", indent, "");
}
static void innerStart(Printer* out) {
    indent += 4;
    printNewLine(out);
}
static void innerEnd(Printer* out) {
    indent -= 4;
    printNewLine(out);
    out("]");
    printNewLine(out);
}

static void printIdentifierList(const IdentifierList* list, Printer* out) {
    out("(");
    if (!list->count) {
        out(")");
        return;
    }
    out("%s", list->names[0]);
    for (uint i = 1; i < list->count; ++i) {
        out(", %s", list->names[i]);
    }
    out(")");
}

static void printQuote(const Quote* quote, Printer* out) {
    out("Quote[");
    innerStart(out);
    printNode(quote->inner, out);
    innerEnd(out);
}
static void printSetq(const Setq* setq, Printer* out) {
    out("Setq[%s,", setq->variable);
    innerStart(out);
    printNode(setq->value, out);
    innerEnd(out);
}
static void printFunc(const Func* func, Printer* out) {
    out("Func[%s,", func->name);
    printIdentifierList(func->parameters, out);
    out(",");
    innerStart(out);
    printNode(func->body, out);
    innerEnd(out);
}
static void printLambda(const Lambda* lambda, Printer* out) {
    out("Lambda[");
    printIdentifierList(lambda->parameters, out);
    out(",");
    innerStart(out);
    printNode(lambda->body, out);
    innerEnd(out);
}
static void printProg(const Prog* prog, Printer* out) {
    out("Prog[");
    printIdentifierList(prog->context, out);
    out(",");
    innerStart(out);
    printNode(prog->body, out);
    innerEnd(out);
}
static void printCond(const Cond* cond, Printer* out) {
    out("Cond[");
    innerStart(out);
    printNode(cond->condition, out);
    out(",");
    printNewLine(out);
    printNode(cond->onTrue, out);
    out(",");
    printNewLine(out);
    printNode(cond->onFalse, out);
    innerEnd(out);
}
static void printWhile(const While* whileN, Printer* out) {
    out("While[");
    innerStart(out);
    printNode(whileN->condition, out);
    out(",");
    printNewLine(out);
    printNode(whileN->body, out);
    innerEnd(out);
}
static void printReturn(const Return* returnN, Printer* out) {
    out("Return[");
    innerStart(out);
    printNode(returnN->value, out);
    innerEnd(out);
}
static void printList(const List* list, Printer* out) {
    out("List[");
    innerStart(out);
    if (!list->count) goto End;
    printNode(list->elements[0], out);
    for (uint i = 1; i < list->count; ++i) {
        out(",");
        printNewLine(out);
        printNode(list->elements[i], out);
    }
End:
    innerEnd(out);
}

static void printNode(const Element* element,
                      __printflike(1, 2) Printer* out) {

    switch (element->type) {
        case syntax.True:
            out("Literal[true]");
            break;
        case syntax.False:
            out("Literal[false]");
            break;
        case syntax.Null:
            out("Literal[Null]");
            break;
        case syntax.Integer:
            printInteger((Integer*)element, out);
            break;
        case syntax.Real:
            printReal((Real*)element, out);
            break;
        case syntax.Quote:
            printQuote((Quote*)element, out);
        case syntax.Setq:
            printSetq((Setq*)element, out);
            break;
        case syntax.Func:
            printFunc((Func*)element, out);
            break;
        case syntax.Lambda:
            printLambda((Lambda*)element, out);
            break;
        case syntax.Prog:
            printProg((Prog*)element, out);
            break;
        case syntax.Cond:
            printCond((Cond*)element, out);
            break;
        case syntax.While:
            printWhile((While*)element, out);
            break;
        case syntax.Return:
            printReturn((Return*)element, out);
            break;
        case syntax.Break:
            out("Break");
            break;
        case syntax.List:
            printList((List*)element, out);
            break;
        default:
            assert(false);
    }
}

#endif /* Printer_h */
