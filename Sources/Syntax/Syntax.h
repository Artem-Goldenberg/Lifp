#define RecursiveNodes \
    (Quote, quote, (inner)), \
    (Setq, setq, (value), (ID, variable)), \
    (Func, func, (body), \
        (ID, name), \
        (IdentifierListPtr, parameters) \
    ), \
    (Lambda, lambda, (body), \
        (IdentifierListPtr, parameters) \
    ), \
    (Cond, cond, (condition, onTrue, onFalse)), \
    (While, whileNode, (condition, body)), \
    (Return, returnNode, (value))

/// Special cased array nodes, they contain variadic amount of children in the `elements` field
#define ArrayNodes \
    (List, list, (elements[]), (uint, count)), \
    (Prog, prog, (elements[]), (IdentifierListPtr, context), (uint, count))

#define SingletonNodes \
    (True, trueNode, ()), \
    (False, falseNode, ()), \
    (Null, null, ()), \
    (Break, breakNode, ()), \
    (ScopeBarrier, barrier, ())

#define ParametrizedLeafNodes \
    (Integer, integer, (), (int, value)), \
    (Real, real, (), (float, value)), \
    (Identifier, identifier, (), (ID, id)), \
    /* special builtin function element will be used to insert C-code into the interpreter */ \
    (BuiltinFunc, builtin, (), (uint, nparams), (CodePointer, code))

#define LeafNodes SingletonNodes, ParametrizedLeafNodes
#define AllNodes JTExpand(LeafNodes, RecursiveNodes, ArrayNodes)

#define NodeNames JTMapComma(GetNodeName, AllNodes)

typedef struct Element Element;

typedef const Element* (*CodePointer)(Context* context);

// typedef for all nodes
#define Typedef(Node, ...) typedef struct Node Node;
JTForEachAux(DataApply, Typedef, AllNodes)

// TODO: const?

/// Identifier list is not an element, rather just a storage for names used by other elements
typedef struct {
    uint count;
    ID names[];
} IdentifierList;

typedef const IdentifierList* IdentifierListPtr;

/// A convinience struct to mark the Array region in the memory of `Prog` and `List` structs
typedef struct  Array {
    uint count;
    Element* elements[];
} Array;

#define ElementIndex(Node, ...) Node##Type
typedef enum ElementType: byte {
    JTMapComma(ElementIndex, NodeNames),
    ElementsCount
} ElementType;

struct Element {
    ElementType type;
};

// Declare structs for all node types
JTForEachAux(DataApply, StructDeclaration, ConvertNodeDatas(LeafNodes, RecursiveNodes))

struct List {
    Element base;
    uint dummyPadding;
    uint count;
    Element* elements[];
};

struct Prog {
    Element base;
    IdentifierListPtr context;
    uint count;
    Element* elements[];
};

#define CreatorMethodDeclaration(Node, var, children, ...) \
    Element* (*const var)(DeclareNodeFullParameters(children, __VA_ARGS__));

struct SyntaxBindings {
    JTForEachAux(DataApply, CreatorMethodDeclaration,
                 ConvertNodeDatas(LeafNodes, RecursiveNodes))

    // Special case for list as it's creation doesn't need it's children right away
    Element* (*const list)(void);
    Element* (*const prog)(const IdentifierList* context);

    // For creating simple elements
    Element* (*const simple)(ElementType type);

    // Call right after calling the `list` or `prog` creation methods,
    // these two methods allow you to fill the contents of a list or a prog nodes
    Array* (*const startArray)(void);
    bool (*const appendElement)(Array* list, Element* element);

    // Similar utility function for the identifier list
    IdentifierList* (*const identifierList)(void);
    bool (*const appendIdentifier)(IdentifierList* list, ID id);

    // Call after finishing appending to an array or an IdentifierList
    void (*const finalize)(void);
};

extern const struct SyntaxBindings node;

#define NodeTypeMember(Node) const ElementType Node;
#define Binding(Node) .Node = ElementIndex(Node),
static const struct {
    // const ElementType List; const ElementType Identifier; ...
    JTForEach(NodeTypeMember, NodeNames)
} syntax = {
    JTForEach(Binding, NodeNames)
};

#undef Bidning
#undef NodeTypeMember
