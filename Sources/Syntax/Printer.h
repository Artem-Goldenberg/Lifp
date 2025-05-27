#ifndef Printer_h
#define Printer_h

#include "Lifp.h"

/// Either stores the next index in the list or the next child name of the node
typedef union {
    uint listIndex;
    const char* const* fieldName;
} ChildrenIterator;

/// Passed to all print functions
typedef struct {
    /// File to print to
    FILE* out;
    /// Current amount of indent before printing
    uint indent;
    /// Current node type which we are printing
    ElementType currentNode;
    /// Current child of the node which we are printing
    ChildrenIterator currentChild;
    /// Whether we should insert closing brace after the node is fully printed
    bool shouldClose;
} PresentationContext;

// MARK: - Printing primitives

__printflike(2, 3) /// Just prints the format string like `printf` to the ouput file of the `context`
static inline void present(PresentationContext* context, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(context->out, format, args);
    va_end(args);
}

__printflike(2, 3) /// Prints the format string with indentation
static inline void presentMember(PresentationContext* context, const char* format, ...) {
    fprintf(context->out, "%*s", context->indent, "");
    va_list args;
    va_start(args, format);
    vfprintf(context->out, format, args);
    va_end(args);
}

__printflike(2, 3) /// Called when need to print a compound multiline object
static inline void presentEnter(PresentationContext* context, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(context->out, format, args);
    va_end(args);
    context->indent += 4;
    context->shouldClose = true;
    fprintf(context->out, "\n");
}

/// Called when done printing the compound object
static inline void presentExit(PresentationContext* context) {
    context->indent -= 4;
    context->shouldClose = false;
    fprintf(context->out, "%*s}\n", context->indent, "");
}

// MARK: - Special case print functions

static inline void printIdentifierListPtr(const IdentifierList* list,
                                          PresentationContext* context) {
    present(context, "(");

    if (list->count)
        present(context, "\"%s\"", list->names[0]);

    for (uint i = 1; i < list->count; ++i)
        present(context, ", \"%s\"", list->names[i]);

    present(context, ")\n");
}

static inline void printID(ID id, PresentationContext* context) {
    present(context, "\"%s\"\n", id);
}

VisitFunction(print, Integer, integer, PresentationContext* context) {
    present(context, "Integer { %d }\n", integer->value);
}

VisitFunction(print, Real, real, PresentationContext* context) {
    present(context, "Real { %g }\n", real->value);
}

VisitFunction(print, Identifier, identifier, PresentationContext* context) {
    present(context, "Identifier { \"%s\" }\n", identifier->id);
}

VisitFunction(print, True, trueNode, PresentationContext* context) {
    present(context, "true\n");
}

VisitFunction(print, False, falseNode, PresentationContext* context) {
    present(context, "false\n");
}

VisitFunction(print, Null, nullNode, PresentationContext* context) {
    present(context, "null\n");
}

VisitFunction(print, Break, breakNode, PresentationContext* context) {
    present(context, "break\n");
}

VisitFunction(print, List, list, PresentationContext* context) {
    present(context, "List[%u] ", list->count);
    if (list->count == 0) {
        present(context, "{}\n");
    } else {
        presentEnter(context, "{");
        context->currentChild.listIndex = 0;
    }
}

VisitFunction(print, Prog, prog, PresentationContext* context) {
    present(context, "Prog[%u] ", prog->count);
    presentEnter(context, "{");
    presentMember(context, "context = ");
    printIdentifierListPtr(prog->context, context);
    context->currentChild.listIndex = 0;
}

// MARK: - General printing functions

#define PresentParameter(Type, name, var) \
    presentMember(context, #name " = "); \
    print##Type(var->name, context);

#define PresentParameterUnpack(paramData, var) \
    JTApp(PresentParameter, JTId paramData, var)

#define VarNameToString(varData) JTApp(JTString, GetVarName(varData))

// definitions for printing functions for all the other nodes
#define PrintRecursiveNodeDefinition(Node, var, children, ...) \
InnerVisitFunction(print, Node, var, PresentationContext* context) { \
    static const char* const childrenNames[] = { \
        JTInnerMapComma(VarNameToString, JTId children) \
    }; /* static array with all children names for this node */ \
    /* place children iterator at the start of the array */ \
    context->currentChild.fieldName = childrenNames; \
    presentEnter(context, #Node " {"); \
    /* present all parameters of the node */ \
    JTInnerForEachAux(PresentParameterUnpack, var, __VA_ARGS__) \
}

JTForEachAux(DataApply, PrintRecursiveNodeDefinition, ConvertNodeDatas(RecursiveNodes))
DefineReferencesFor(print, PresentationContext* context)

// MARK: - Main function

static inline void print(const Element* node, PresentationContext* context) {
    // save parent's node context in local variables
    ElementType parentNode = context->currentNode;
    ChildrenIterator child = context->currentChild;
    bool shouldClose = context->shouldClose;

    switch (parentNode) {
        case syntax.List:
        case syntax.Prog:
            presentMember(context, "[%d] = ", child.listIndex++);
            break;
        default:
            if (child.fieldName)
                presentMember(context, "%s = ", *child.fieldName++);
            break;
    }

    if (!node) {
        present(context, "NULL\n");
        if (context->shouldClose)
            presentExit(context);
        return;
    }

    // reset context before processing the child node
    context->currentChild.fieldName = NULL;
    context->shouldClose = false;
    context->currentNode = node->type;

    NodeTypeSwitch(node, print, context)

    if (context->shouldClose)
        presentExit(context);

    // restore context to the parent's node context
    context->currentNode = parentNode;
    context->currentChild = child;
    context->shouldClose = shouldClose;
}

#endif /* Printer_h */
