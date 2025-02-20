#ifndef AbstractSyntax_h
#define AbstractSyntax_h

#include <stdio.h>
#include <stdbool.h>

#include "macro.h"

//#define s(some, ...) __VA_OPT__(10)
//
//int a = s(30, 30);
//
//#define Elements (Atom, Literal, List)
//#define Lit
//
//MacroArgmap(<#mapper#>, <#sep, ...#>)


typedef const char* Identifier;

typedef enum: unsigned char {
    AtomElementType,
    LiteralElementType,
    ListElementType
} ElementType;


#define Literals Integer, Real, Boolean, Null
#define WithLiteral(name) name##Literal

typedef enum: unsigned char {
    IntegerLiteral,
    RealLiteral,
    TrueLiteral,
    FalseLiteral,
    NullLiteral
} LiteralType;

typedef struct {
    const LiteralType type;
    const union {
        int integer;
        float real;
    };
} LiteralValue;


/// Any Element, what element exactly is determined inspecting the `type`
typedef struct {
    const ElementType type;
} Element;

typedef struct {
    const ElementType type; // = AtomElement
    const Identifier name;
} Atom;

typedef struct {
    const ElementType type; // = LiteralElement
    const LiteralValue literal;
} Literal;

typedef struct {
    const ElementType type; // = ListElement
    const int count;
    const Element* const elements[];
} List;


#endif /* AbstractSyntax_h */
