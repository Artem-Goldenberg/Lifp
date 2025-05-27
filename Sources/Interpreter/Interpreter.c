#include "Lifp.h"

// MARK: - Util functions

void fatalError(const char* message, ...) {
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

/// Put the result of evaluation of some node to the value stack
extern inline void putValue(Value value, Context* context);

/// Discard top value from the value stack
extern inline Value popValue(Context* context);

static bool isLeafNode(const Element* node) {
    return node->type < JTCount(LeafNodes);
}

static bool doNeedToProcessChildren(const Element* node) {
    if (isLeafNode(node)) return false;

    switch (node->type) {
        case syntax.Func:
        case syntax.Lambda:
        case syntax.Prog:
        case syntax.Quote:
            return false;

        default: return true;
    }
}

/// Put child on the stack to be evaluated
static void putNode(const Element* node, Context* context) {
    *context->nextNode++ = node;
    // leaf nodes don't need to process their children
    *context->isProcessed++ = !doNeedToProcessChildren(node);
}

static void popNode(Context* context) {
    assert(context->nextNode != context->evaluationStack);
    context->nextNode--;
    context->isProcessed--;
}

static const Element* peekNode(const Context* context) {
    // assert stack not empty
    assert(context->nextNode != context->evaluationStack);
    return context->nextNode[-1];
}

static uint currentScopeStart(const Context* context) {
    if (context->currentScope != context->scopes)
        return context->currentScope[-1];
    return 0;
}

void bindVariable(ID id, Value value, Context* context) {
    for (uint i = currentScopeStart(context); i < *context->currentScope; ++i) {
        // if var is found in local scope => replace it
        // otherwise => add new binding to the scope
        if (context->variables[i].id == id) {
            context->variables[i].value = value;
            return;
        }
    }
    context->variables[context->currentScope[0]++] = (Binding) {id, value};
}

Value lookupVariable(ID id, const Context* context) {
    // 06:52 should ve writeen -1 ...
    for (int i = *context->currentScope - 1; i >= 0; --i) {
        if (context->variables[i].id == id)
            return context->variables[i].value;
    }
    return NULL;
}

static void newScope(Context* context) {
    assert(context->currentScope + 1 - context->scopes < StackMaxSize);
    context->currentScope++;
    context->currentScope[0] = context->currentScope[-1];
}

static void discardScopeValues(Context* context) {
    while (context->topValue != context->valueStack)
        if (popValue(context)->type == syntax.ScopeBarrier)
            return;

    fatalError("Return statement out of local scope");
}

static void discardNodesUntil(ElementType type, Context* context) {
    while (context->nextNode != context->evaluationStack) {
        const Element* node = peekNode(context);
        popNode(context);
        if (node->type == type)
            return;
    }
    // return or break called in a global scope, whole program terminates
}

static void discardScope(Context* context) {
    assert(context->currentScope != context->scopes);
    discardScopeValues(context);
    context->currentScope--;
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
    if (!result) { // if cannot be resolved => evaluates to itself
        return (Value)identifier;
    }
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
// have to make it public, as it is used in the Builtin.c
Value evalList(const List* list, Context* context) {
    // empty list evaluates to itself
    if (list->count == 0) return (Value)list;

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
        case syntax.BuiltinFunc: {
            BuiltinFunc* builtin = (BuiltinFunc*)callee;
            if (builtin->nparams != list->count - 1)
                fatalError("The amount of arguments (%d) doesn't match the amount of "
                           "parameters (%d) in the builtin function",
                           list->count - 1, builtin->nparams);
            return builtin->code(context);
        };
        default:
            fatalError("Call of the non-functional element");
            assert(false);
    }

    if (parameters->count != list->count - 1)
        fatalError("The amount of arguments (%d) doesn't match the amount of parameters (%d)",
                   list->count - 1, parameters->count);

    // evaluate function, need to start a new scope to add parameter bindings
    newScope(context);

    // for each paramter there is a value on the value stack
    // because we just requested it in the `processList` function
    for (uint i = 0; i < parameters->count; ++i) {
        Value argument = popValue(context);
        bindVariable(parameters->names[i], argument, context);
    }

    // start of new scope can be traced by looking to the barrier
    putValue(node.barrier(), context);

    // 01:50 - invented scope barrier
    putNode(node.barrier(), context);

    // artificialy add node right after the body node in the stack
    // it indicates that we should drop our current scope when we encounter it
//    putNode(node.breakNode(), context);

    // finally evaluate the function
    putNode(body, context);

    // special case where we skip return here and delegate it to the `body` node
    return NULL;
}

static bool processQuote(const Quote* quote, Context* context) {
    // nothing to do here, we don't need to evaluate anything
    return true;
}
static Value evalQuote(const Quote* quote, Context* context) {
    return quote->inner;
}

static bool processSetq(const Setq* setq, Context* context) {
    putNode(setq->value, context);
    return true;
}
static Value evalSetq(const Setq* setq, Context* context) {
    Value value = popValue(context);
    bindVariable(setq->variable, value, context);
    return node.null();
}

static bool processFunc(const Func* func, Context* context) {
    return true;
}
static Value evalFunc(const Func* func, Context* context) {
    bindVariable(func->name, (Value)func, context);
    // functions evaluates to themselves
    return (Value)func;
}

static bool processLambda(const Lambda* lambda, Context* context) {
    return true;
}
static Value evalLambda(const Lambda* lambda, Context* context) {
    return (Value)lambda;
}

static bool processProg(const Prog* prog, Context* context) {
    return true;
}
static Value evalProg(const Prog* prog, Context* context) {
    // evaluate to null if doesn't have any elements to eval
    if (prog->count == 0) return node.null();

    const IdentifierList* locals = prog->context;

    newScope(context);

    // program locals at first are initialized as nulls
    for (uint i = 0; i < locals->count; ++i)
        bindVariable(locals->names[i], node.null(), context);

    putValue(node.barrier(), context);

    // again put an indicator to drop local scope after reaching this
    putNode(node.barrier(), context);

    for (int i = prog->count - 1; i >= 0; --i)
        putNode(prog->elements[i], context);

    return NULL;
}

static bool processCond(const Cond* cond, Context* context) {
    putNode(cond->condition, context);
    return true;
}
static Value evalCond(const Cond* cond, Context* context) {
    Value condition = popValue(context);

    switch (condition->type) {
        case syntax.True:
            putNode(cond->onTrue, context);
            break;
        case syntax.False:
            if (cond->onFalse)
                putNode(cond->onFalse, context);
            else
                return node.null();
            break;
        default:
            // TODO: report where, etc... (but how ???)
            fatalError("Condition value is not a boolean");
            break;
    }

    // delegate return to one of the nodes put on the stack
    return NULL;
}

static bool processWhile(const While* whileNode, Context* context) {
    putNode(whileNode->condition, context);
    return true;
}
static Value evalWhile(const While* whileNode, Context* context) {
    Value condition = popValue(context);

    switch (condition->type) {
        case syntax.True:
            putNode((Value)whileNode, context);
            putNode(whileNode->body, context);
            return NULL;
        case syntax.False:
            return node.null();
        default:
            fatalError("Condition is not a boolean value");
    }
}

static bool processReturn(const Return* returnNode, Context* context) {
    putNode(returnNode->value, context);
    return true;
}
static Value evalReturn(const Return* returnNode, Context* context) {
    Value value = popValue(context);
    discardScope(context);
    // discard all other code in current scope
    discardNodesUntil(syntax.ScopeBarrier, context);
    return value;
}

static bool processBreak(const Break* breakNode, Context* context) {
    return true;
}
static Value evalBreak(const Break* breakNode, Context* context) {
//    while (context->nextNode != context->evaluationStack) {
//        if (context->nextNode[-1]->type == syntax.While)
//            // why don't we return the breakNode on break
//            return (Value)breakNode;
//        popNode(context);
//    }
    discardNodesUntil(syntax.While, context);
    // why don't we return the breakNode on break
    return (Value)breakNode;
}

static bool processBuiltinFunc(const BuiltinFunc* builtin, Context* context) {
    return true;
}
static Value evalBuiltinFunc(const BuiltinFunc* builtin, Context* context) {
    return (Value)builtin;
}

static bool processScopeBarrier(const ScopeBarrier* barrier, Context* context) {
    return true;
}
static Value evalScopeBarrier(const ScopeBarrier* barrier, Context* context) {
    Value lastValue = popValue(context);
    assert(lastValue->type != syntax.ScopeBarrier);

    discardScope(context);

    return lastValue;
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

Value interpret(ProgramInfo* program) {
    Context context = {};

    // set up stack pointers
    context.currentScope = context.scopes;

    context.topValue = context.valueStack;
    context.nextNode = context.evaluationStack;
    context.isProcessed = context.isProcessedStack;

    initilizeBuiltinContext(program, &context);

    const Prog* prog = program->syntaxRoot;

    // put elements on evaluation stack in reversed order,
    // so the last elem will be evaluated last
    for (int i = prog->count - 1; i >= 0; --i)
        putNode(prog->elements[i], &context);

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
