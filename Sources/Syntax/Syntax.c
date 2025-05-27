#include "Lifp.h"

// MARK: - Arrays

static Array* startArray(void) {
    // this will be the continuation from the previously half-allocated List or Prog
//    Array* array = allocateFromReserved(sizeof(Array));
    Array* array = getReservedPointer();
    if (!array) return NULL;
    array->count = 0;
    return array;
}

static void finalize(void) {
    unreserve();
}

static bool appendElement(Array* array, Element* element) {
    Element** allocated = allocateFromReserved(sizeof(Element*));
    if (!allocated) return false; // ran out of memory
    // TODO: There is a problem if the reallocation happens when we call `allocateFromReserved`
    // function, but actually, in this case we would need to redirect the caller's array pointer,
    // so we need to return a pointer to a new array location. I will not do this right now
    // because it may lead to bugs I don't have time to partner with right now
    // assert our last reservation was for this `list`
    assert(allocated == array->elements + array->count);
    array->elements[array->count++] = element;
    return true;
}

static bool appendIdentifier(IdentifierList* list, ID id) {
    ID* allocated = allocateFromReserved(sizeof(ID));
    if (!allocated) return false; // ran out of memory
    assert(allocated == list->names + list->count);
    list->names[list->count++] = id;
    return true;
}

static IdentifierList* newIdentifierList(void) {
    reserve();
    IdentifierList* list = allocateFromReserved(sizeof(IdentifierList));
    if (!list) return NULL;
    list->count = 0;
    return list;
}


// MARK: - Elements

static Element* newList(void) {
    reserve();
    // next, the array allocation will be called, which will allocate the rest of the list
    List* list = allocateFromReserved(sizeof(List));
    if (!list) return NULL;
    setReservedPointer(&list->count);
    list->base.type = syntax.List;
    return (Element*)list;
}

static Element* newProg(const IdentifierList* context) {
    reserve();
    // next, the array allocation will be called, which will allocate the rest of the prog
    Prog* prog = allocateFromReserved(sizeof(Prog));
    if (!prog) return NULL;
    setReservedPointer(&prog->count);
    prog->base.type = syntax.Prog;
    prog->context = context;
    return (Element*)prog;
}

#define CreatorDefinition(Node, var, children, ...) \
static Element* new##Node(DeclareNodeFullParameters(children, __VA_ARGS__)) { \
    Node* var = allocate(sizeof(Node)); \
    if (!var) return NULL; \
    ((Element*)var)->type = syntax.Node; \
    JTInnerMapAux(AssignField, ;, var, __VA_ARGS__ __VA_OPT__(,) JTId children); \
    return (Element*)var; \
}

JTForEachAux(DataApply, CreatorDefinition, ConvertNodeDatas(ParametrizedLeafNodes, RecursiveNodes))

#define SingletonDefinition(Node, var, ...) \
static Element* new##Node(void) { \
    static Node var = (Node) { .base.type = syntax.Node }; \
    return (Element*)&var; \
}

JTForEachAux(DataApply, SingletonDefinition, ConvertNodeDatas(SingletonNodes))

static Element* newSimpleElement(ElementType type) {
    Element* result = allocate(sizeof(Element));
    if (!result) return NULL;
    result->type = type;
    return result;
}

#define CreatorBinding(Node, type, ...) .type = new##Node
const struct SyntaxBindings node = {
    JTMapCommaAux(DataApply, CreatorBinding, AllNodes),
    .simple = newSimpleElement,
    .startArray = startArray,
    .appendElement = appendElement,
    .identifierList = newIdentifierList,
    .appendIdentifier = appendIdentifier,
    .finalize = finalize
};
