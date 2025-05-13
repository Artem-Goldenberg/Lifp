#include "Lifp.h"

/// Context of the interpreter
///
/// Passed to all recursive invokations of the interpreter.
/// Mainly stores information abount current scope, i.e. available variables
typedef struct Context {
    // TODO: this
} Context;


// MARK: - Evaluation functions (see pdf spec for what to do in them)


static Value evalInteger(const Integer* integer, Context* context) {
    return NULL;
    // TODO: this
}

static Value evalReal(const Real* real, Context* context) {
    return NULL;
    // TODO: this
}

static Value evalIdentifier(const Identifier* identifier, Context* context) {
    return NULL;
    // TODO: this
}

static Value evalTrue(const True* trueNode, Context* context) {
    return NULL;
    // TODO: this
}

static Value evalFalse(const False* falseNode, Context* context) {
    return NULL;
    // TODO: this
}

static Value evalNull(const Null* nullNode, Context* context) {
    return NULL;
    // TODO: this
}

static Value evalList(const List* list, Context* context) {
    // TODO: this
    return NULL;
}

static Value evalQuote(const Quote* quote, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalSetq(const Setq* setq, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalFunc(const Func* func, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalLambda(const Lambda* lambda, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalProg(const Prog* prog, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalCond(const Cond* cond, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalWhile(const While* whileNode, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalReturn(const Return* returnNode, Context* context) {
    // TODO: this
    return NULL;
}
static Value evalBreak(const Break* breakNode, Context* context) {
    // TODO: this
    return NULL;
}

static Value eval(const Element* node, Context* context) {
    // Expands to a giant switch by a node->type varaible
    // And in each case branch just calls the appropriate function
    // To handle given type of node, one case for example:
    // ```
    // case syntax.Quote:
    //     return evalQuote((const Quote*)node, context);
    // ```
    ReturnNodeTypeSwitch(node, eval, context)
}

Value evalElement(const Element* node) {
    Context predefinedContext = {
        // TODO: fill with predefined functions as given in a pdf
    };
    return eval(node, &predefinedContext);
}
