#ifndef EnumGeneration_h
#define EnumGeneration_h

#include "CodeGeneration.h"

/*
 This assumes setup like this:
 - Category name to keep in mind, for example: Token to list all possible tokens, or Colors ...
 - Macro named as a plural form of the Category which expands to a comma-separated list of items
*/

#define ItemIndex(Item, Suffix) Item##Suffix

#define EnumDefinitionFor(Category) \
    EnumDefinitionCommon(Category, Category##s)

#define EnumDefinitionCommon(Category, ...) \
typedef enum: byte { \
    JTMapCommaAux(ItemIndex, Category, __VA_ARGS__), \
    Category##s##Count \
} Category;

#define BindingsDefinition(Category) \
    BindingsDefinitionCommon(Category, Category##s)

#define ItemBindingDecl(Item, EnumType) const EnumType Item;
#define ItemBindingInit(Item, Suffix) .Item = ItemIndex(Item, Suffix)

/// Generated a dummy global constant structure to provide namepsace for conviniently accessing token names
#define BindingsDefinitionCommon(Category, bindingName, ...) \
static const struct { \
    JTForEachAux(ItemBindingDecl, Category, __VA_ARGS__) \
} bindingName = { \
    JTMapCommaAux(ItemBindingInit, Category, __VA_ARGS__) \
};


#define UtilDefinitionsFor(Category, bindingName) \
    UtilDefinitionsCommon(Category, bindingName, Category##s)

#define UtilDefinitionsCommon(Category, bindingName, ...) \
    EnumDefinitionCommon(Category, __VA_ARGS__) \
    BindingsDefinitionCommon(Category, bindingName, __VA_ARGS__)


#endif /* EnumGeneration_h */
