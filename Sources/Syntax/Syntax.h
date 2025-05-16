#define AllNodes LeafNodes, RecursiveNodes, ListNode

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
    (Prog, prog, (body), (IdentifierListPtr, context)), \
    (Cond, cond, (condition, onTrue, onFalse)), \
    (While, whileNode, (condition, body)), \
    (Return, returnNode, (value))

//(Prog, prog, (statements[]),
//    (IdentifierListPtr, context),
//    (uint, statementsCount)
//), 

/// Special cased list node as it contains variadic amount of children in `elements`
#define ListNode (List, list, (elements[]), (uint, count))

#define LeafNodes \
    (True, trueNode, ()), \
    (False, falseNode, ()), \
    (Null, null, ()), \
    (Integer, integer, (), (int, value)), \
    (Real, real, (), (float, value)), \
    (Identifier, identifier, (), (ID, id)), \
    (Break, breakNode, ())

#define NodeNames JTMapComma(GetNodeName, AllNodes)

typedef struct Element Element;

// typedef for all nodes
#define Typedef(Node, ...) typedef struct Node Node;
JTForEachAux(DataApply, Typedef, AllNodes)

// TODO: const?

/// Identifier list is not an element, rather just a storage for names used by other elements
typedef struct {
    uint count;
    ID names[];
} IdentifierList;

typedef IdentifierList* IdentifierListPtr;

#define ElementIndex(Node, ...) Node##Type
typedef enum ElementType: byte {
    JTMapComma(ElementIndex, NodeNames),
    ElementsCount
} ElementType;

struct Element {
    ElementType type;
};

// Declare structs for all node types
JTForEachAux(DataApply, StructDeclaration, ConvertNodeDatas(AllNodes))

#define CreatorMethodDeclaration(Node, var, children, ...) \
    Element* (*const var)(DeclareNodeFullParameters(children, __VA_ARGS__));

struct SyntaxBindings {
    JTForEachAux(DataApply, CreatorMethodDeclaration,
                 ConvertNodeDatas(LeafNodes, RecursiveNodes))

    // For creating simple elements
    Element* (*const simple)(ElementType type);

    // Special case for list as it's creation doesn't need it's children right away
    List* (*const list)(void);

    bool (*const appendElement)(List* list, Element* element);

    // Similar utility function for the identifier list
    IdentifierList* (*const identifierList)(void);

    bool (*const appendIdentifier)(IdentifierList* list, ID id);

    // Call after finishing appending to a list
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
