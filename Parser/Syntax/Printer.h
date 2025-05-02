#ifndef Printer_h
#define Printer_h

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "VisitorGeneration.h"
#include "Syntax.h"

//__printflike(1, 2)
//typedef void PrintContext(const char* format, ...);

typedef struct {
    FILE* out;

    uint indent;

    ElementType currentNode;

    union {
        uint listIndex;
        const char* const* fieldName;
    };

    union {
        uint listCount;
        const char* end;
    };

    bool shouldClose;

} PresentationContext;

#define Present(context, format, ...) do { \
    printf("%*s", context->indent, ""); \
} while (false)

__printflike(2, 3)
static inline void present(PresentationContext* context, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(context->out, format, args);
    va_end(args);
}

__printflike(2, 3)
static inline void presentEnter(PresentationContext* context, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(context->out, format, args);
    va_end(args);
    context->indent += 4;
    context->shouldClose = true;
    fprintf(context->out, "\n%*s", context->indent, "");
}

static inline void presentExit(PresentationContext* context) {
    context->indent -= 4;
    context->shouldClose = false;
    fprintf(context->out, "\n%*s}\n", context->indent, "");
    fprintf(context->out, "%*s", context->indent, "");
}

VisitFunction(print, Integer, integer, PresentationContext* context) {
    present(context, "Integer { %d }\n", integer->value);
}

VisitFunction(print, Real, real, PresentationContext* context) {
    present(context, "Real { %g }\n", real->value);
}

static inline void printIdentifierListPtr(const IdentifierList* list, PresentationContext* context) {
    present(context, "(");

    if (list->count)
        present(context, "%s", list->names[0]);

    for (uint i = 1; i < list->count; ++i)
        present(context, ", %s", list->names[i]);

    present(context, ")\n");
}

static inline void printID(ID id, PresentationContext* context) {
    present(context, "\"%s\"\n", id);
}

VisitFunction(print, Identifier, identifier, PresentationContext* context) {
    present(context, "Identifier { \"%s\" }\n", identifier->id);
}

VisitFunction(print, List, list, PresentationContext* context) {
    presentEnter(context, "List[%u] {", list->count);
    context->listIndex = 0;
}

#define PresentParameter(Type, name, var) \
    present(context, #name " = "); \
    print##Type(var->name, context);

#define PresentParameterUnpack(paramData, var) \
    JTApp(PresentParameter, JTId paramData, var)
//
#define VarNameToString(varData) JTApp(JTString, GetVarName(varData))

#define PrintRecursiveNodeDefinition(Node, var, children, ...) \
InnerVisitFunction(print, Node, var, PresentationContext* context) { \
    static const char* const childrenNames[] = { \
        JTInnerMapComma(VarNameToString, JTId children) \
    }; \
    context->fieldName = childrenNames; \
    presentEnter(context, #Node " {"); \
    JTInnerForEachAux(PresentParameterUnpack, var, __VA_ARGS__) \
}

JTForEachAux(DataApply, PrintRecursiveNodeDefinition, ConvertNodeDatas(RecursiveNodes))

//JTApp(JTString, 30)
//JTForEachAux(DataApply, PrintRecursiveNodeDefinition, RecursiveNodes)

DefineReferencesFor(print, PresentationContext* context)

static inline void print(const Element* node, PresentationContext* context) {
    switch (context->currentNode) {
        case syntax.List:
            present(context, "[%d] = ", context->listIndex++);
            break;
        default:
            if (context->fieldName)
                present(context, "%s = ", *context->fieldName++);
            break;
    }

    ElementType parentNode = context->currentNode;
    context->currentNode = node->type;

    NodeTypeSwitch(node, print, context)

    if (context->shouldClose)
        presentExit(context);

    context->currentNode = parentNode;
}

#endif /* Printer_h */
