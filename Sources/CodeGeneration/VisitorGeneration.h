/* Utils for generating code related to syntax nodes */

/// Convert the list of datas provided in the user format to the inner format for convinience
#define ConvertNodeDatas(...) \
    JTMapCommaAux(DataApply, NodeDataWithConvertedChildren, __VA_ARGS__)

#define NodeDataWithConvertedChildren(Node, var, children, ...) \
    (Node, var, (InnerNamesToVarDatas(Element*, JTId children)) __VA_OPT__(,) __VA_ARGS__)


// Format of node data:
// (Type, var, (childrenData), parametersData), where:
//  Type - main name of the node type, starts with a capital
//  var - additional name of the node type, used for variables
//  childrenData - comma separated list of variable datas (but type is always Element*)
//  parametersData - comma separated list of variable datas for parameters (can be any type)

#define GetNodeName(data) JTGetFirst data
#define GetNodeVarName(data) JTGetSecond data
#define GetNodeChildren(data) JTGetThird data
#define GetNodeParameters(data) JTGetFourth data

#define CommaSeparate(data, ...) JTId data __VA_OPT__(JTCommaIfSome(JTId data)) __VA_ARGS__
#define GetNodeFullParameters(data) \
    CommaSeparate((GetNodeParameters(data)), JTId GetNodeChildren(data))

#define DeclareNodeFullParameters(children, ...) \
    InnerDeclareParameters(__VA_ARGS__ __VA_OPT__(,) JTId children)

#define RebuildFuncDefinition(Node, var, children, ...) \
Element* rebuild##Node(const Node* var, JTId children) { \
    if (JTInnerMapAux(CompareField, &&, var, JTId children)) \
        return var; \
    return node.var( \
        InnerGetFields(var, __VA_ARGS__)__VA_OPT__(,) \
        InnerGetVarNames(JTId children) \
    ); \
}

#define RebuildNodeSwitchCase(Node, node, children, func, var, ...) \
    case syntax.Node: \
        JTInnerForEachAux( \
            RecursiveCallUnpack, \
            (Node, func, var, __VA_ARGS__), \
            JTId children \
        ) \
        break;
    // TODO: this


#define StructDeclaration(Node, var, children, ...) \
struct Node { \
    Element base; \
    JTInnerMap(DeclareVar, ;, __VA_ARGS__ __VA_OPT__(,) JTId children); \
};

#define RecursiveCall(child, Node, func, var, ...) \
    func(((const Node*)var)->child __VA_OPT__(,) __VA_ARGS__);

#define RecursiveCallUnpack(child, inParens) \
    JTApp(RecursiveCall, GetVarName(child), JTId inParens)


#define ReturnNodeSwitchCase(Node, node, children, func, var, ...) \
    case syntax.Node: \
        return func##Node((const Node*)var __VA_OPT__(,) __VA_ARGS__);

#define ReturnNodeSwitchCaseUnpack(nodeData, funcData) \
    JTApp(ReturnNodeSwitchCase, JTTakeThree nodeData, JTId funcData)

#define ReturnNodeTypeSwitch(var, func, ...) \
    switch (var->type) { \
        JTForEachAux( \
            ReturnNodeSwitchCaseUnpack, \
            (func, var, __VA_ARGS__), \
            ConvertNodeDatas(AllNodes) \
        ) \
        default: assert(false); \
    }

/**
 For some recursive node type, generates a switch case to process it.

 Where a specific function for this node typs is called and then, all children of this node's type are traversed recursively.
  - Parameters:
    - Node: Name of the struct for the current node type
    - var:  Name of the variable indicating this node's type
    - children: Variable datas for the children of the current type
    - func: Function name to call recursively
    - `...`:     List of additional parameters to the `func` function (as variable datas)
 */
#define RecursiveNodeSwitchCase(Node, node, children, func, var, ...) \
    case syntax.Node: \
        if (func##Node##Ref) \
            func##Node##Ref( \
                (const Node*)var __VA_OPT__(,) __VA_ARGS__ \
            ); \
        JTInnerForEachAux( \
            RecursiveCallUnpack, \
            (Node, func, var, __VA_ARGS__), \
            JTId children \
        ) \
        break;

#define RecursiveNodeSwitchCaseUnpack(nodeData, funcData) \
    JTApp(RecursiveNodeSwitchCase, JTTakeThree nodeData, JTId funcData)


/* special case for array nodes */
#define ArrayNodeSwitchCase(Node, node, func, var, ...) \
    case syntax.Node: {  \
        const Node* node = (const Node*)var; \
        if (func##Node##Ref) \
            func##Node##Ref(node __VA_OPT__(,) __VA_ARGS__); \
        for (uint i = 0; i < node->count; ++i) \
            func(node->elements[i] __VA_OPT__(,) __VA_ARGS__); \
    }; break;

#define ArrayNodeSwitchCaseUnpack(nodeData, funcData) \
    JTApp(ArrayNodeSwitchCase, JTTakeTwo nodeData, JTId funcData)


#define NodeTypeSwitch(var, func, ...) \
    switch (var->type) { \
        JTForEachAux( \
            RecursiveNodeSwitchCaseUnpack, \
            (func, var, __VA_ARGS__), \
            ConvertNodeDatas(LeafNodes, RecursiveNodes) \
        ) \
        JTForEachAux( \
            ArrayNodeSwitchCaseUnpack, \
            (func, var, __VA_ARGS__), \
            ConvertNodeDatas(ArrayNodes) \
        ) \
        default: assert(false); \
    }

#define VisitFunctionParameters(Node, var, ...) \
    const Node* var __VA_OPT__(,) __VA_ARGS__

#define VisitFunctionRef(func, Node, var, ...) \
static void (*const func##Node##Ref)(VisitFunctionParameters(Node, var, __VA_ARGS__))

#define InnerVisitFunction(func, Node, var, ...) \
/* Forward declare */ \
static void func##Node(VisitFunctionParameters(Node, var, __VA_ARGS__)); \
/* Assign a reference */ \
VisitFunctionRef(func, Node, var, __VA_ARGS__) = func##Node; \
/* And now let the user actualy define the thing */ \
static void func##Node(VisitFunctionParameters(Node, var, __VA_ARGS__))

#define VisitFunction(func, Node, var, ...) \
    JTExpand3(InnerVisitFunction(func, Node, var, __VA_ARGS__))

//#define (Node, func) func##Node##Defined;
#define DefineReference(Node, var, func, ...) \
static void (*const func##Node##Ref)(VisitFunctionParameters(Node, var, __VA_ARGS__));


#define DefineReferencesUnpack(nodeData, funcData) \
    JTApp(DefineReference, JTTakeTwo nodeData, JTId funcData)


#define DefineReferencesFor(func, ...) \
    JTForEachAux(DefineReferencesUnpack, (func, __VA_ARGS__), AllNodes);

//    JTForEachAux(DefineReferenceUnpack, func, ConvertNodeDatas(AllNodes))
