#include "Lifp.h"

#define StackMaxSize 2048
#define MaxScopeDepth 128

/// An entry to the context, specifying that the identifier `id` is now bound to the varaible `value`
typedef struct Bidning {
    ID id;
    Value value;
} Binding;

/// Context of the interpreter
///
/// Passed to all recursive invokations of the interpreter.
/// Mainly stores information abount current scope, i.e. available variables
typedef struct Context {
    /// Contiguous array of variable bindings to values
    Binding variables[4096];

    /// Array of indices into the `variables` array
    ///
    /// Each index marks the **END** of the scope inside `variables` array, i.e.
    /// `variables[scopes[0]]` is the first variable in the second scope
    uint scopes[MaxScopeDepth];
    /// Pointer to the top-most scope, i.e. local scope, i.e. pointer to index inside the `variables` where
    /// the last scope ends and there is no scopes after that (there is always at least one scope, the global one)
    uint* currentScope;

    /// Elements which are queried to be evaluated
    const Element* evaluationStack[StackMaxSize];
    const Element** nextNode;

    /// Helper stack with flag indicating whether or not the current eval stack node's children has been processed
    /// And we can discard this node next time
    bool isProcessedStack[StackMaxSize];
    bool* isProcessed;

    /// Values of elements evaluated
    Value valueStack[StackMaxSize];
    Value* topValue;

} Context;

// MARK: - Util functions

static uint currentScopeStart(const Context* context) {
    if (context->currentScope != context->scopes)
        return context->currentScope[-1];
    return 0;
}

static void bindVariable(ID id, Value value, Context* context) {
    for (uint i = currentScopeStart(context); i < *context->currentScope; ++i) {
        // TODO: if var is found in local scope => replace it
        // otherwise => add new binding to the scope
    }
}

static Value lookupVariable(ID id, const Context* context) {
    // TODO: lookup in all scopes starting from the current one
    // just use == on two IDs
    // return `NULL` if not found
    return NULL;
}

static void newScope(Context* context) {
    // TODO: create new scope
}

static bool isLeafNode(const Element* node) {
    return node->type < JTCount(LeafNodes);
}

/// Put child on the stack to be evaluated
static void putNode(const Element* node, Context* context) {
    *context->nextNode++ = node;
    // leaf nodes don't need to process their children
    *context->isProcessed++ = isLeafNode(node);
}

static void popNode(Context* context) {
    assert(context->nextNode != context->evaluationStack);
    context->nextNode--;
    context->isProcessed--;
}

/// Put the result of evaluation of some node to the value stack
static void putValue(Value value, Context* context) {
    *context->topValue++ = value;
}

/// Discard top value from the value stack
static Value popValue(Context* context) {
    return *--context->topValue;
}

static const Element* peekNode(const Context* context) {
    // assert stack not empty
    assert(context->nextNode != context->evaluationStack);
    return context->nextNode[-1];
}

// MARK: - Evaluation functions (see pdf spec for what to do in them)

// MARK: Constant nodes evaluate to themselves

// dont need to do anything in these, they do not process children
static bool processInteger(const Integer* integer, Context* context) {
    return true;
}
static bool processReal(const Real* real, Context* context) {
    return true;
}
static bool processTrue(const True* trueNode, Context* context) {
    return true;
}
static bool processFalse(const False* falseNode, Context* context) {
    return true;
}
static bool processNull(const Null* null, Context* context) {
    return true;
}
static bool processIdentifier(const Identifier* identifier, Context* context) {
    return true;
}

// evaluate to themselves
static Value evalInteger(const Integer* integer, Context* context) {
    return (Value)integer;
}
static Value evalReal(const Real* real, Context* context) {
    return (Value)real;
}
static Value evalTrue(const True* trueNode, Context* context) {
    return (Value)trueNode;
}
static Value evalFalse(const False* falseNode, Context* context) {
    return (Value)falseNode;
}
static Value evalNull(const Null* nullNode, Context* context) {
    return (Value)nullNode;
}

static Value evalIdentifier(const Identifier* identifier, Context* context) {
    Value result = lookupVariable(identifier->id, context);
    if (!result) // if cannot be resolved => evaluates to itself
        return (Value)identifier;
    return result;
}

// MARK: Nodes with children
// They first need to process their children, and only after it's done
// and the children values are on the stack, then we can `eval` them

static bool processList(const List* list, Context* context) {
    // first evaluate all of it's children (put them on the stack for it)
    for (uint i = 0; i < list->count; ++i)
        putNode(list->elements[i], context);
    return false;
}
static Value evalList(const List* list, Context* context) {
    // this will be the value of the first element in the list,
    // because we put it first on the evaluation stack
    const Element* callee = popValue(context);
    const IdentifierList* parameters;
    const Element* body;

    // a little ugly, but understandable
    switch (callee->type) {
        case syntax.Func: {
            Func* func = (Func*)callee;
            parameters = func->parameters;
            body = func->body;
        } break;
        case syntax.Lambda: {
            Lambda* lambda = (Lambda*)callee;
            parameters = lambda->parameters;
            body = lambda->body;
        } break;
        case syntax.Prog: {
            Prog* prog = (Prog*)callee;
            parameters = prog->context;
            body = prog->body;
        } break;
        default:
            // TODO: error, non-function is called, terminate the program
            assert(false);
    }

    // evaluate function, need to start a new scope to add parameter bindings
    newScope(context);

    // for each paramter there is a value on the value stack
    // because we just requested it in the `processList` function
    for (uint i = 0; i < parameters->count; ++i) {
        Value argument = popValue(context);
        bindVariable(parameters->names[i], argument, context);
    }
    // finally evaluate the function
    putNode(body, context);

    // special case where we skip return here and delegate it to the `body` node
    return NULL;
}

static bool processQuote(const Quote* quote, Context* context) {
    return false;
    // TODO: this
}
static Value evalQuote(const Quote* quote, Context* context) {
    // TODO: this
    return NULL;
}
static bool processSetq(const Setq* setq, Context* context) {
    return false;
    // TODO: this
}
static Value evalSetq(const Setq* setq, Context* context) {
    // TODO: this
    return NULL;
}
static bool processFunc(const Func* func, Context* context) {
    return false;
    // TODO: this
}
static Value evalFunc(const Func* func, Context* context) {
    // TODO: this
    return NULL;
}
static bool processLambda(const Lambda* lambda, Context* context) {
    return false;
    // TODO: this
}
static Value evalLambda(const Lambda* lambda, Context* context) {
    // TODO: this
    return NULL;
}
static bool processProg(const Prog* prog, Context* context) {
    return false;
    // TODO: this
}
static Value evalProg(const Prog* prog, Context* context) {
    // TODO: this
    return NULL;
}
static bool processCond(const Cond* cond, Context* context) {
    return false;
    // TODO: this
}
static Value evalCond(const Cond* cond, Context* context) {
    // TODO: this
    return NULL;
}
static bool processWhile(const While* whileNode, Context* context) {
    return false;
    // TODO: this
}
static Value evalWhile(const While* whileNode, Context* context) {
    // TODO: this
    return NULL;
}
static bool processReturn(const Return* returnNode, Context* context) {
    return false;
    // TODO: this
}
static Value evalReturn(const Return* returnNode, Context* context) {
    // TODO: this
    return NULL;
}
static bool processBreak(const Break* breakNode, Context* context) {
    return false;
    // TODO: this
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

// it returns bool just for fun, fix later
static bool process(const Element* node, Context* context) {
    // same as from `eval` function
    ReturnNodeTypeSwitch(node, process, context)
}

Value evalElement(const Element* node) {
    Context context = {
        // TODO: fill global scope with predefined functions as given in a pdf
        // (hard, need to put them in StringTable first)
    };

    // set up stack pointers
    context.currentScope = context.scopes;

    context.topValue = context.valueStack;
    context.nextNode = context.evaluationStack;
    context.isProcessed = context.isProcessedStack;

    // put the node to be evaluated on the evaluation stack
    *context.nextNode++ = node;

    // while evaluation stack is not empty, evaluate elements from it!
    while (context.nextNode != context.evaluationStack) {
        // get top node
        const Element* nextNode = context.nextNode[-1];

        if (!context.isProcessed[-1]) {
            context.isProcessed[-1] = true;
            // add children to evaluating stack
            process(nextNode, &context);
            continue;
        }

        // remove node from the stack
        popNode(&context);

        Value result = eval(nextNode, &context);

        // special case when result is skipped
        if (!result) continue;

        *context.topValue++ = result;
    }

    assert(context.topValue != context.valueStack);

    // when evaluating list, there can be multiple result values on the stack
    // for now just return the last one of them
    return context.topValue[-1];
}
