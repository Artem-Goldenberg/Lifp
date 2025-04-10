#include "Syntax.h"

#include "Allocator.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include <errno.h>

// MARK: - Lists

static void finalizeList(void) {
    unreserve();
}

bool appendElement(List* list, Element* element) {
    Element** allocated = allocateFromReserved(sizeof(Element*));
    if (!allocated) return false; // ran out of memory
    // assert our last reservation was for this `list`
    assert(allocated == list->elements + list->count);
    list->elements[list->count++] = element;
    return true;
}

bool appendIdentfier(IdentifierList* list, ID id) {
    ID* allocated = allocateFromReserved(sizeof(ID));
    if (!allocated) return false; // ran out of memory
    assert(allocated == list->names + list->count);
    list->names[list->count++] = id;
    return true;
}

static inline List* newList(void) {
    reserve();
    // ???: what if alignment wrong?
    List* list = allocateFromReserved(sizeof(List));
    list->base.type = syntax.List;
    list->count = 0;
    return list;
}

static inline List* newProgram(void) {
    List* program = newList();
    program->base.type = syntax.Root;
    return program;
}

static inline IdentifierList* newIdentifierList(void) {
    reserve();
    IdentifierList* list = allocateFromReserved(sizeof(IdentifierList));
    list->count = 0;
    return list;
}

// MARK: - Elements

static inline Element* newIdentifier(ID id) {
    Identifier* identifier = allocate(sizeof(Identifier));
    identifier->base.type = IdentifierNode;
    identifier->id = id;
    return (Element*)identifier;
}

static inline Element* newReal(float value) {
    Real* real = allocate(sizeof(Real));
    real->base.type = syntax.Real;
    real->value = value;
    return (Element*)real;
}

static inline Element* newInteger(int value) {
    Integer* integer = allocate(sizeof(Integer));
    integer->base.type = syntax.Integer;
    integer->value = value;
    return (Element*)integer;
}

static inline Element* newSimpleElement(ElementType type) {
    Element* result = allocate(sizeof(Element));
    result->type = type;
    return result;
}

static inline Element* newElement(uint type, ...);

#define SingleCase(type) case ElementIndex(type)


#define Case(group) JTInnerMap(SingleCase, :, group##Elements)

#define GroupCheckerDeclarationFor(group) \
static inline bool is##group##Type(ElementType type) { \
    switch(type) { \
        Case(group): \
            return true; \
        default: \
            return false; \
    } \
}

JTForEach(GroupCheckerDeclarationFor, ElementGroups)


#define Enum(group) \
typedef enum: byte { \
    JTInnerMap() \
} group##Elements##Enum;

Element* newQuote(Element* inner) {
    Quote* result = allocate(sizeof(Quote));
    result->base.type = syntax.Quote;
    result->inner = inner;
    return (Element*)result;
}
Element* newSetq(ID variable, Element* value) {
    Setq* result = allocate(sizeof(Setq));
    result->base.type = syntax.Setq;
    result->variable = variable;
    result->value = value;
    return (Element*)result;
}
Element* newFunc(ID name, IdentifierList* parameters, Element* body) {
    Func* result = allocate(sizeof(Func));
    result->base.type = syntax.Func;
    result->name = name;
    result->parameters = parameters;
    result->body = body;
    return (Element*)result;
}
Element* newLambda(IdentifierList* parameters, Element* body) {
    Lambda* result = allocate(sizeof(Lambda));
    result->base.type = syntax.Lambda;
    result->parameters = parameters;
    result->body = body;
    return (Element*)result;
}
Element* newProg(IdentifierList* context, Element* body) {
    Prog* result = allocate(sizeof(Prog));
    result->base.type = syntax.Prog;
    result->context = context;
    result->body = body;
    return (Element*)result;
}
Element* newCond(Element* condition, Element* onTrue, Element* onFalse) {
    Cond* result = allocate(sizeof(Cond));
    result->base.type = syntax.Cond;
    result->condition = condition;
    result->onTrue = onTrue;
    result->onFalse = onFalse;
    return (Element*)result;
}
Element* newWhileForm(Element* condition, Element* body) {
    While* result = allocate(sizeof(While));
    result->base.type = syntax.While;
    result->condition = condition;
    result->body = body;
    return (Element*)result;
}
Element* newReturnForm(Element* value) {
    Return* result = allocate(sizeof(Return));
    result->base.type = syntax.Return;
    result->value = value;
    return (Element*)result;
}

#define ElementTypeBinding(type) .type = ElementIndex(type)
#define GroupCheckerBinding(group) .is##group = is##group##Type
const struct SyntaxBindings node = {
    JTMapComma(GroupCheckerBinding, ElementGroups),
    .list = newList,
    .integer = newInteger,
    .real = newReal,
    .identifier = newIdentifier,
    .simple = newSimpleElement,
    .new = newElement,
    .finalize = finalizeList,
    .quote = newQuote,
    .setq = newSetq,
    .func = newFunc,
    .lambda = newLambda,
    .prog = newProg,
    .cond = newCond,
    .whileForm = newWhileForm,
    .returnForm = newReturnForm,

};

// need uint for `type` because va_list cannot work with bytes
static inline Element* newElement(uint type, ...) {
    assert(false); // not implemented
    if (node.isSimple(type))
        return newSimpleElement(type);

    Element* result = NULL;
    va_list args;
    va_start(args, type);
    switch (type) {
        case syntax.Integer:
            result = (Element*)newInteger(va_arg(args, int));
            break;
        case syntax.Real:
            result = (Element*)newReal((float)va_arg(args, double));
            break;
        case syntax.Identifier:
            result = (Element*)newIdentifier(va_arg(args, uint));
            break;
//        case syntax.List: { // need to allocate list in place
//            uint elementsCount = va_arg(args, uint);
//            List* list = allocate(sizeof(List) + elementsCount * sizeof(Element*));
//            list->base.type = node.List;
//            list->count = elementsCount;
//            for (int i = 0; i < elementsCount; ++i)
//                list->elements[i] = va_arg(args, Element*);
//            result = (Element*)list;
//        } break;
    }
    va_end(args);
    assert(result);
    return result;
}
