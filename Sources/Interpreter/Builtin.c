#include "Lifp.h"

// MARK: - Arithmetic

static int extractInt(Value node, const char* caller) {
    if (node->type != syntax.Integer)
        fatalError("Not an integer in '%s'", caller);
    return ((Integer*)node)->value;
}

static Value plus(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    int a = extractInt(one, "+");
    int b = extractInt(two, "+");

    return (Value)node.integer(a + b);
}

static Value minus(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    int a = extractInt(one, "-");
    int b = extractInt(two, "-");

    return (Value)node.integer(a - b);
}

static Value times(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    int a = extractInt(one, "*");
    int b = extractInt(two, "*");

    return (Value)node.integer(a * b);
}

static Value divide(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    int a = extractInt(one, "/");
    int b = extractInt(two, "/");

    return (Value)node.integer(a / b);
}

static Value mod(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    int a = extractInt(one, "mod");
    int b = extractInt(two, "mod");

    return (Value)node.integer(a % b);
}

// MARK: - List functions

static List* extractList(Value node, const char* caller) {
    if (node->type != syntax.List)
        fatalError("Not a List in '%s'", caller);
    return (List*)node;
}

static Value head(Context* context) {
    Value value = popValue(context);
    List* list = extractList(value, "head");

    if (!list->count)
        fatalError("Cannot take the head of an empty list");

    return list->elements[0];
}

static void fillArray(Array* array, List* source, uint start, const char* caller) {
    for (uint i = start; i < source->count; ++i) {
        if (!node.appendElement(array, source->elements[i]))
            fatalError("No memory for new list in the '%s' call", caller);
    }
    node.finalize();
}

static Value tail(Context* context) {
    Value value = popValue(context);
    List* list = extractList(value, "tail");

    if (!list->count)
        fatalError("Cannot take the tail of an empty list");

    List* new = (List*)node.list();
    Array* array = node.startArray();
    fillArray(array, list, 1, "tail");
    new->count = list->count - 1;
    return (Value)new;
}

static Value cons(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    List* list = extractList(two, "cons");

    List* new = (List*)node.list();
    Array* array = node.startArray();
    node.appendElement(array, (Element*)one);
    fillArray(array, list, 0, "cons");
    new->count = list->count + 1;
    return (Value)new;
}

// MARK: - Comparisons

static float extractNumeric(Value node, const char* caller) {
    switch (node->type) {
        case syntax.Integer:
            return ((Integer*)node)->value;
        case syntax.Real:
            return ((Real*)node)->value;
        case syntax.True:
            return 1;
        case syntax.False:
            return 0;
        default:
            fatalError("Bad value type in the '%s' call", caller);
    }
}

static Value equal(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    float a = extractNumeric(one, "==");
    float b = extractNumeric(two, "==");

    if (a == b) return node.trueNode();
    else        return node.falseNode();
}

static Value nonequal(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    float a = extractNumeric(one, "!=");
    float b = extractNumeric(two, "!=");

    if (a != b) return node.trueNode();
    else        return node.falseNode();
}

static Value less(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    float a = extractNumeric(one, "<");
    float b = extractNumeric(two, "<");

    if (a < b) return node.trueNode();
    else        return node.falseNode();
}

static Value lesseq(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    float a = extractNumeric(one, "<=");
    float b = extractNumeric(two, "<=");

    if (a <= b) return node.trueNode();
    else        return node.falseNode();
}

static Value greater(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    float a = extractNumeric(one, ">");
    float b = extractNumeric(two, ">");

    if (a > b) return node.trueNode();
    else        return node.falseNode();
}

static Value greatereq(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    float a = extractNumeric(one, ">=");
    float b = extractNumeric(two, ">=");

    if (a >= b) return node.trueNode();
    else        return node.falseNode();
}

// MARK: - Predicates

static Value isint(Context* context) {
    Value value = popValue(context);

    if (value->type == syntax.Integer)
        return node.trueNode();
    else
        return node.falseNode();
}

static Value isreal(Context* context) {
    Value value = popValue(context);

    if (value->type == syntax.Real)
        return node.trueNode();
    else
        return node.falseNode();
}

static Value isbool(Context* context) {
    Value value = popValue(context);

    if (value->type == syntax.True || value->type == syntax.False)
        return node.trueNode();
    else
        return node.falseNode();
}

static Value isnull(Context* context) {
    Value value = popValue(context);

    if (value->type == syntax.Null)
        return node.trueNode();
    else
        return node.falseNode();
}

static Value isatom(Context* context) {
    Value value = popValue(context);

    if (value->type == syntax.Identifier)
        return node.trueNode();
    else
        return node.falseNode();
}

static Value islist(Context* context) {
    Value value = popValue(context);

    if (value->type == syntax.List)
        return node.trueNode();
    else
        return node.falseNode();
}

static Value isempty(Context* context) {
    Value value = popValue(context);

    List* list = extractList(value, "empty?");

    if (list->count == 0)
        return node.trueNode();
    else
        return node.falseNode();
}

// MARK: - Logical operators

static bool extractBool(Value node, const char* caller) {
    switch (node->type) {
        case syntax.True:
            return true;
        case syntax.False:
            return false;
        default:
            fatalError("Not a boolean in the '%s' call", caller);
    }
}

static Value and(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    bool a = extractBool(one, "and");
    bool b = extractBool(two, "and");

    if (a && b) return node.trueNode();
    else        return node.falseNode();
}

static Value or(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    bool a = extractBool(one, "or");
    bool b = extractBool(two, "or");

    if (a || b) return node.trueNode();
    else        return node.falseNode();
}

static Value xor(Context* context) {
    Value one = popValue(context);
    Value two = popValue(context);

    bool a = extractBool(one, "xor");
    bool b = extractBool(two, "xor");

    if (a ^ b) return node.trueNode();
    else       return node.falseNode();
}

static Value not(Context* context) {
    Value one = popValue(context);

    bool a = extractBool(one, "not");

    if (!a) return node.trueNode();
    else    return node.falseNode();
}

// MARK: - Evaluator

static Value eval(Context* context) {
    Value value = popValue(context);

    if (value->type == syntax.List)
        return evalList((List*)value, context);

    return value;
}

// MARK: - Builtin context

static const struct {
    const char* name;
    uint nparams;
    CodePointer func;
} builtins[] = {
    {"+", 2, plus},
    {"-", 2, minus},
    {"*", 2, times},
    {"/", 2, divide},
    {"mod", 2, mod},

    {"head", 1, head},
    {"tail", 1, tail},
    {"cons", 2, cons},

    {"==", 2, equal},
    {"!=", 2, nonequal},
    {"<", 2, less},
    {"<=", 2, lesseq},
    {">", 2, greater},
    {">=", 2, greatereq},

    {"int?", 1, isint},
    {"real?", 1, isreal},
    {"bool?", 1, isbool},
    {"null?", 1, isnull},
    {"atom?", 1, isatom},
    {"list?", 1, islist},
    {"empty?", 1, isempty},

    {"and", 2, and},
    {"or", 2, or},
    {"xor", 2, xor},
    {"not", 1, not},

    {"eval", 1, eval}
};

void initilizeBuiltinContext(ProgramInfo* program, Context* context) {
    for (uint i = 0; i < sizeof builtins / sizeof builtins[0]; ++i) {
        ID id = registerIdentifier(program, builtins[i].name);
        Value builtinFunc = node.builtin(builtins[i].nparams, builtins[i].func);
        bindVariable(id, builtinFunc, context);
    }
}
