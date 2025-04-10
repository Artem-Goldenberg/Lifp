#ifndef Syntax_h
#define Syntax_h

#include "Token.h"
#include "Common/Common.h"
#include "JustText/JustText.h"

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


#define Typedef(type) typedef struct type type;
JTForEach(Typedef, Elements)
#undef Typedef

typedef struct Element Element;

void printSetq(Setq* setq) {

}

Element* substitute(Element* element);


Element* rebuildSetq(const Setq* setq, Element* newBody) {
    if (newBody == setq->body)
        return setq;

    return node.setq(setq->name, newBody);
}


Element* substitute(Element* element) {
    switch (element->type) {
        case syntax.Setq: {
            Setq* setq = (Setq*)element;
            Element* newBody = substitute(setq->body);
            return rebuildSetq(setq, newBody);
        };

        default:
            break;
    }
}

void print(Element* element) {
    switch (element->type) {
        case Setq:
            printSetq((Setq*)element)
            break;

        default:
            break;
    }
}

#define SyntaxNodes \
    (Quote, quote, Element* inner), \
    (Setq, setq, ID variable, Element* value), \
    (Func, func, ID name, Element* body), \
    (Lambda, lambda, Element* body, IdentfierList* parameters)

#define NonRecursiveNodes \
    (Quote, quote, (inner)), \
    (Setq, setq, (value), (ID, variable)), \
    (Func, func, (body), \
        (ID, name), \
        (IdentifierList*, parameters), \
    ),


// TODO: const?

/// Identifier list is not an element, rather just a storage for names used by other elements
typedef struct {
    uint count;
    ID names[];
} IdentifierList;

bool appendElement(List* list, Element* element);
bool appendIdentfier(IdentifierList* list, ID id);

#define ElementIndex(type) type##Node
typedef enum ElementType: byte {
    JTMapComma(ElementIndex, Elements),
    ElementsCount
} ElementType;


struct Element {
    ElementType type;
};


struct List {
    Element base; // == ListNode
    // Everything below can be referd to same as ElementArray*
    uint count;
    Element* elements[];
};


struct Identifier {
    Element base; // == IdentifierNode
    ID id; // ???: for debug char*
};


struct Integer {
    Element base; // == IntegerNode
    int value;
};


struct Real {
    Element base; // == RealNode
    float value; // ???: fixpoint?
};


struct Quote {
    Element base; // == QuoteNode
    Element* inner;
};


struct Setq {
    Element base; // == SetqNode
    ID variable;
    Element* value;
};


struct Func {
    Element base; // == FuncNode
    ID name;
    IdentifierList* parameters;
    Element* body;
};


struct Lambda {
    Element base; // == LambdaNode
    Element* body;
    IdentifierList* parameters;
};


struct Prog {
    Element base; // == ProgNode
    Element* body;
    IdentifierList* context; // ???: What is that?
};


struct Cond {
    Element base; // == CondNode
    Element* condition;
    Element* onTrue;
    Element* onFalse; // can be NULL
};


struct While {
    Element base; // == WhileNode
    Element* condition;
    Element* body;
};


struct Return {
    Element base; // == ReturnNode
    Element* value;
};


#define ElementTypeMember(type) const ElementType type;
#define GroupCheckerMember(group) bool (*is##group)(ElementType type);
struct SyntaxBindings {
    // bool (*const isSimple)(ElementType type); ...
    JTForEach(GroupCheckerMember, ElementGroups);

    // Data Literals
    Element* (*const real)(float value);
    Element* (*const integer)(int value);
    Element* (*const identifier)(ID id);

    // Special Forms
    Element* (*const quote)(Element* inner);
    Element* (*const setq)(ID variable, Element* value);
    Element* (*const func)(ID name, IdentifierList* parameters, Element* body);
    Element* (*const lambda)(IdentifierList* parameters, Element* body);
    Element* (*const prog)(IdentifierList* context, Element* body);
    Element* (*const cond)(Element* condition, Element* onTrue, Element* onFalse);
    Element* (*const whileForm)(Element* condition, Element* body);
    Element* (*const returnForm)(Element* value);

    // General Purpose
    Element* (*const leaf)(ElementType type);
    Element* (*const new)(uint type, ...);

    // List starting functions
    List* (*const list)(void);
    List* (*const program)(void);

    // Utility
    IdentifierList* (*const identifierList)(void);

    // Call after finishing appending to a list
    void (*const finalize)(void);
};

extern const struct SyntaxBindings node;

#define Binding(element) .element = ElementIndex(element)
static const struct {
    // const ElementType List; const ElementType Identifier; ...
    JTForEach(ElementTypeMember, Elements);
} syntax = {
    JTMapComma(Binding, Elements)
};

#undef Bidning
#undef GroupCheckerMember
#undef ElementTypeMember

#endif /* Syntax_h */
