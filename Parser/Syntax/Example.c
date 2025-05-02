#include <stdio.h>

//void printSetq(const Setq* setq) {
//
//}
//
//Element* substitute(Element* element);
//
//
//Element* rebuildSetq(const Setq* setq, Element* body) {
//
//    if (body == setq->body)
//        return setq;
//
//    return node.setq(setq->name, body);
//}
//
//Element* rebuildCond(const Cond* cond, Element* onTrue, Element* onFalse) {
//
//    return node.cond(onTrue, onFalse);
//}
//
//
//Element* substitute(Element* element) {
//    switch (element->type) {
//        case syntax.Setq: {
//            Setq* setq = (Setq*)element;
//            Element* newBody = substitute(setq->body);
//            return rebuildSetq(setq, newBody);
//        };
//
//        default:
//            break;
//    }
//}

#include "Syntax.h"

#define LiteralElements True, False, Null, Integer, Real
#define BasicElements   List, Identifier
#define SpecialElements Quote, Setq, Func, Lambda, Prog, Cond, While, Return, Break

#define Elements \
    Root, /* Special element - means the whole program */ \
    LiteralElements, BasicElements, SpecialElements

#define ElementGroups \
    Simple, /* Elements that don't need to store additional data, just their type */ \
    Basic, Literal, Special

#define SimpleElements True, False, Null, Break

#define ATypedef(type) typedef struct type type;
JTForEach(Typedef, Elements)
#undef ATypedef


void printSetq(const Setq* some) {
    
}

static void (*const printSetqPtr)(const Setq*) = printSetq;

static void (*const printSetqPtr)(const Setq*);

#define More Some(10)
#define Some(...) __VA_ARGS__

int a = Some(More);

void print(Element* element) {
    switch (element->type) {
        case 0: //syntax.Setq:
            printSetq((Setq*)element);
            break;

        case 1: {
            int a = 0;
        } break;

        default:
            break;
    }

    int a;
    a = 0;

}

//
//struct List {
//    Element base; // == ListNode
//    // Everything below can be referd to same as ElementArray*
//    uint count;
//    Element* elements[];
//};


//struct Identifier {
//    Element base; // == IdentifierNode
//    ID id; // ???: for debug char*
//};
//
//
//struct Integer {
//    Element base; // == IntegerNode
//    int value;
//};
//
//
//struct Real {
//    Element base; // == RealNode
//    float value; // ???: fixpoint?
//};


//struct Quote {
//    Element base; // == QuoteNode
//    Element* inner;
//};
//
//
//struct Setq {
//    Element base; // == SetqNode
//    ID variable;
//    Element* value;
//};
//
//
//struct Func {
//    Element base; // == FuncNode
//    ID name;
//    IdentifierList* parameters;
//    Element* body;
//};
//
//
//struct Lambda {
//    Element base; // == LambdaNode
//    Element* body;
//    IdentifierList* parameters;
//};
//
//
//struct Prog {
//    Element base; // == ProgNode
//    Element* body;
//    IdentifierList* context; // ???: What is that?
//};
//
//
//struct Cond {
//    Element base; // == CondNode
//    Element* condition;
//    Element* onTrue;
//    Element* onFalse; // can be NULL
//};
//
//
//struct While {
//    Element base; // == WhileNode
//    Element* condition;
//    Element* body;
//};
//
//
//struct Return {
//    Element base; // == ReturnNode
//    Element* value;
//};

// Data Literals
//Element* (*const real)(float value);
//Element* (*const integer)(int value);
//Element* (*const identifier)(ID id);
//
//// Special Forms
//Element* (*const quote)(Element* inner);
//Element* (*const setq)(ID variable, Element* value);
//Element* (*const func)(ID name, IdentifierList* parameters, Element* body);
//Element* (*const lambda)(IdentifierList* parameters, Element* body);
//Element* (*const prog)(IdentifierList* context, Element* body);
//Element* (*const cond)(Element* condition, Element* onTrue, Element* onFalse);
//Element* (*const whileForm)(Element* condition, Element* body);
//Element* (*const returnForm)(Element* value);
//
//// General Purpose
//Element* (*const leaf)(ElementType type);
//Element* (*const new)(uint type, ...);
//
//// List starting functions
//List* (*const list)(void);
//List* (*const program)(void);
//
//// Utility
//IdentifierList* (*const identifierList)(void);

#define GroupCheckerMember(group) bool (*is##group)(ElementType type);

//static inline Element* newIdentifier(ID id) {
//    Identifier* identifier = allocate(sizeof(Identifier));
//    identifier->base.type = syntax.Identifier;
//    identifier->id = id;
//    return (Element*)identifier;
//}
//
//static inline Element* newReal(float value) {
//    Real* real = allocate(sizeof(Real));
//    real->base.type = syntax.Real;
//    real->value = value;
//    return (Element*)real;
//}
//
//static inline Element* newInteger(int value) {
//    Integer* integer = allocate(sizeof(Integer));
//    integer->base.type = syntax.Integer;
//    integer->value = value;
//    return (Element*)integer;
//}

//#define SingleCase(type) case ElementIndex(type)
//
//
//#define Case(group) JTInnerMap(SingleCase, :, group##Elements)
//
//#define GroupCheckerDeclarationFor(group) \
//static inline bool is##group##Type(ElementType type) { \
//    switch(type) { \
//        Case(group): \
//            return true; \
//        default: \
//            return false; \
//    } \
//}
//
//JTForEach(GroupCheckerDeclarationFor, ElementGroups)
//
//
//#define Enum(group) \
//typedef enum: byte { \
//    JTInnerMap() \
//} group##Elements##Enum;

#define GroupCheckerBinding(group) .is##group = is##group##Type

//Element* newQuote(Element* inner) {
//    Quote* result = allocate(sizeof(Quote));
//    result->base.type = syntax.Quote;
//    result->inner = inner;
//    return (Element*)result;
//}
//Element* newSetq(ID variable, Element* value) {
//    Setq* result = allocate(sizeof(Setq));
//    result->base.type = syntax.Setq;
//    result->variable = variable;
//    result->value = value;
//    return (Element*)result;
//}
//Element* newFunc(ID name, IdentifierList* parameters, Element* body) {
//    Func* result = allocate(sizeof(Func));
//    result->base.type = syntax.Func;
//    result->name = name;
//    result->parameters = parameters;
//    result->body = body;
//    return (Element*)result;
//}
//Element* newLambda(IdentifierList* parameters, Element* body) {
//    Lambda* result = allocate(sizeof(Lambda));
//    result->base.type = syntax.Lambda;
//    result->parameters = parameters;
//    result->body = body;
//    return (Element*)result;
//}
//Element* newProg(IdentifierList* context, Element* body) {
//    Prog* result = allocate(sizeof(Prog));
//    result->base.type = syntax.Prog;
//    result->context = context;
//    result->body = body;
//    return (Element*)result;
//}
//Element* newCond(Element* condition, Element* onTrue, Element* onFalse) {
//    Cond* result = allocate(sizeof(Cond));
//    result->base.type = syntax.Cond;
//    result->condition = condition;
//    result->onTrue = onTrue;
//    result->onFalse = onFalse;
//    return (Element*)result;
//}
//Element* newWhileForm(Element* condition, Element* body) {
//    While* result = allocate(sizeof(While));
//    result->base.type = syntax.While;
//    result->condition = condition;
//    result->body = body;
//    return (Element*)result;
//}
//Element* newReturnForm(Element* value) {
//    Return* result = allocate(sizeof(Return));
//    result->base.type = syntax.Return;
//    result->value = value;
//    return (Element*)result;
//}

//    .list = newList,
//    .integer = newInteger,
//    .real = newReal,
//    .identifier = newIdentifier,
//    .simple = newSimpleElement,
//    .new = newElement,
//    .finalize = finalizeList,
//    .quote = newQuote,
//    .setq = newSetq,
//    .func = newFunc,
//    .lambda = newLambda,
//    .prog = newProg,
//    .cond = newCond,
//    .whileForm = newWhileForm,
//    .returnForm = newReturnForm,

// need uint for `type` because va_list cannot work with bytes
//static inline Element* newElement(uint type, ...) {
//    assert(false); // not implemented
//    if (node.isSimple(type))
//        return newSimpleElement(type);
//
//    Element* result = NULL;
//    va_list args;
//    va_start(args, type);
//    switch (type) {
//        case syntax.Integer:
//            result = (Element*)newInteger(va_arg(args, int));
//            break;
//        case syntax.Real:
//            result = (Element*)newReal((float)va_arg(args, double));
//            break;
//        case syntax.Identifier:
//            result = (Element*)newIdentifier(va_arg(args, uint));
//            break;
////        case syntax.List: { // need to allocate list in place
////            uint elementsCount = va_arg(args, uint);
////            List* list = allocate(sizeof(List) + elementsCount * sizeof(Element*));
////            list->base.type = node.List;
////            list->count = elementsCount;
////            for (int i = 0; i < elementsCount; ++i)
////                list->elements[i] = va_arg(args, Element*);
////            result = (Element*)list;
////        } break;
//    }
//    va_end(args);
//    assert(result);
//    return result;
//}
