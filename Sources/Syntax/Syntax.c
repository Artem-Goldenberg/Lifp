#include "Lifp.h"

// MARK: - Lists

static void finalizeList(void) {
    unreserve();
}

static bool appendElement(List* list, Element* element) {
    Element** allocated = allocateFromReserved(sizeof(Element*));
    if (!allocated) return false; // ran out of memory
    // assert our last reservation was for this `list`
    assert(allocated == list->elements + list->count);
    list->elements[list->count++] = element;
    return true;
}

static bool appendIdentifier(IdentifierList* list, ID id) {
    ID* allocated = allocateFromReserved(sizeof(ID));
    if (!allocated) return false; // ran out of memory
    assert(allocated == list->names + list->count);
    list->names[list->count++] = id;
    return true;
}

static List* newList(void) {
    reserve();
    // ???: what if alignment wrong?
    List* list = allocateFromReserved(sizeof(List));
    if (!list) return NULL;
    list->base.type = syntax.List;
    list->count = 0;
    return list;
}

static IdentifierList* newIdentifierList(void) {
    reserve();
    IdentifierList* list = allocateFromReserved(sizeof(IdentifierList));
    if (!list) return NULL;
    list->count = 0;
    return list;
}

// MARK: - Elements

#define CreatorDefinition(Node, var, children, ...) \
static Element* new##Node(DeclareNodeFullParameters(children, __VA_ARGS__)) { \
    Node* var = allocate(sizeof(Node)); \
    if (!var) return NULL; \
    ((Element*)var)->type = syntax.Node; \
    JTInnerMapAux(AssignField, ;, var, __VA_ARGS__ __VA_OPT__(,) JTId children); \
    return (Element*)var; \
}

JTForEachAux(DataApply, CreatorDefinition, ConvertNodeDatas(LeafNodes, RecursiveNodes))

static Element* newSimpleElement(ElementType type) {
    Element* result = allocate(sizeof(Element));
    if (!result) return NULL;
    result->type = type;
    return result;
}

#define CreatorBinding(Node, type, ...) .type = new##Node
const struct SyntaxBindings node = {
    JTMapCommaAux(DataApply, CreatorBinding, AllNodes),
    .appendElement = appendElement,
    .simple = newSimpleElement,
    .identifierList = newIdentifierList,
    .appendIdentifier = appendIdentifier,
    .finalize = finalizeList
};
