#define StackMaxSize 2048
#define MaxScopeDepth 128

/// Type which interpreter will return
///
/// All interpreter really does is it transform one syntax node into the other with, perhaps,
/// variables substituted for some other elements. It is very similar to evaluating terms of lambda calculus
typedef const Element* Value;

/// An entry to the context, specifying that the identifier `id` is now bound to the varaible `value`
typedef struct Bidning {
    ID id;
    Value value;
} Binding;

/// Context of the interpreter
///
/// Passed to all recursive invokations of the interpreter.
/// Mainly stores information abount current scope, i.e. available variables
struct Context {
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

};

/// Interpreters tree in the `syntaxRoot` of the `program`
Value interpret(ProgramInfo* program);

// MARK: - Util function

/// Put the result of evaluation of some node to the value stack
inline void putValue(Value value, Context* context) {
    *context->topValue++ = value;
}

/// Discard top value from the value stack
inline Value popValue(Context* context) {
    return *--context->topValue;
}

/// Add variable with name `id` and value `value` to the context
void bindVariable(ID id, Value value, Context* context);

/// Get the value of the variable named `id` in the context or `NULL` if no variable is found
Value lookupVariable(ID id, const Context* context);

/// This function is exposed only because it is used by the `eval` builtin function
Value evalList(const List* list, Context* context);

__attribute__((noreturn))
__printflike(1, 2)
void fatalError(const char* message, ...);
