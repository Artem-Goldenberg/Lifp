// MARK: - Widely used Stuff

/// Apply macro `next` to the rest of the arguments (they expand)
#define MacroApply(next, ...) next(__VA_ARGS__)

/// Identity macro (useful to help expand the arguments)
#define MacroId(...) __VA_ARGS__
/// Primitive when need to get rid of everything
#define MacroNone(...)

#define IMacroFirst(_1, ...) _1
#define IMacroSecond(_1, _2, ...) _2

#define IMacroConcat2(a, b) MacroId(a ## b)

#define IMacroComma(...) ,
#define IMacroIsEmpty(...) MacroApply(IMacroSecond, IMacroComma __VA_OPT__(()) 0, 1)

// MARK: - Basic Boolean Logic

#define IMacroTrue(...) 1
#define IMacroFalse(...) 0

#define IMacroIf(cond, a, b) IMacroConcat2(IMacroIf_, cond)(a, b)
#define IMacroIf_1(a, b) a
#define IMacroIf_0(a, b) b

#define IMacroAnd(a, b) IMacroConcat2(IMacroAnd_, a) (b)
#define IMacroAnd_1(b) b
#define IMacroAnd_0(b) 0

#define IMacroOr(a, b) IMacroConcat2(IMacroOr_, a) (b)
#define IMacroOr_1(b) 1
#define IMacroOr_0(b) b

#define IMacroNot(a) IMacroConcat2(IMacroNot_, a)
#define IMacroNot_1 0
#define IMacroNot_0 1

// MARK: - Equality

#define MacroEq_(a) a
#define MacroEq_0(a) a
#define MacroEq_1(a) a
#define IMacroEq(a, b) MacroApply(IMacroNot, IMacroIsEmpty(MacroEq_ ## a (MacroEq_ ## b) ()))

#define IMacroEquatable(a) IMacroIsEmpty(MacroEq_ ## a ())

/// Expands to `1` if `a` and `b` are the same, expands to `0` otherwise
/// At least one of `MacroEq_a` or `MacroEq_b` must be defined as an identity functional macros, i.e.
/// ```C
/// #define MacroEq_<name of the arg value here>(a) a
/// ```
/// See examples in `primitives.h`
#define MacroEq(a, b) IMacroIf(IMacroAnd(IMacroEquatable(a), IMacroEquatable(b)), \
IMacroEq(a, b), 0 \
)

/// Opposite to `MacroEq` same conditions apply on `a` and `b`
#define MacroNotEq(a, b) IMacroNot(MacroEq(a, b))

// MARK: - Other

/// Expands to `0` if `a` is `0` **or** empty, expands to `1`
#define MacroBool(a) IMacroNot(IMacroOr(IMacroIsEmpty(a), MacroEq(a, 0)))

/// Expands to `b` if `cond` is `0` **or** empty, expands to `a` otherwise
#define MacroSwitch(cond, a, b) IMacroIf(MacroBool(cond), a, b)

#define MacroMaxArgs 63
/// Counts the number of arguments passed to it, works for at most `MacroMaxArgs` arguments
#define MacroArgCount(...) MacroApply(IMacro64th, __VA_ARGS__ __VA_OPT__(,) IMacro63to0)


#define MacroDefer(next, ...) next MacroNone() (__VA_ARGS__)
#define MacroDefer2(next, ...) next MacroDefer(MacroNone) (__VA_ARGS__)
#define MacroDefer3(next, ...) next MacroDefer2(MacroNone) (__VA_ARGS__)

#define IMacroExpand0(...) __VA_ARGS__
#define IMacroExpand1(...) IMacroExpand0(IMacroExpand0(IMacroExpand0(__VA_ARGS__)))
#define IMacroExpand2(...) IMacroExpand1(IMacroExpand1(IMacroExpand1(__VA_ARGS__)))
#define IMacroExpand3(...) IMacroExpand2(IMacroExpand2(IMacroExpand2(__VA_ARGS__)))
#define IMacroExpand4(...) IMacroExpand3(IMacroExpand3(IMacroExpand3(__VA_ARGS__)))
#define IMacroExpand5(...) IMacroExpand4(IMacroExpand4(IMacroExpand4(__VA_ARGS__)))

#define IMacroMassExpand(...) IMacroExpand5(__VA_ARGS__)

//#define IMacroArgmap(mapper, arg, ...) \
//MacroSwitch(MacroNotEq(arg, IMacroListTerminator), MacroId, MacroNone) \
//(mapper(arg) Defer2(IMacroArgmapRef) (mapper, __VA_ARGS__))

#define IMacroGetLast(arg, ...) \
__VA_OPT__(MacroDefer(IMacroGetLastRef) (__VA_ARGS__) MacroNone)(arg)

#define IMacroGetLastRef() IMacroGetLast

#define MacroGetLast(arg, ...) IMacroMassExpand(MacroId IMacroGetLast(arg, __VA_ARGS__))


#define IMacroDropLast(arg, last, ...) \
arg __VA_OPT__(, MacroDefer(IMacroDropLastRef)(last, __VA_ARGS__))

#define IMacroDropLastRef() IMacroDropLast

#define MacroDropLast(arg, ...) __VA_OPT__( \
    IMacroMassExpand(IMacroDropLast(arg, __VA_ARGS__)) \
)


#define IMacroArgmap(mapper, sep, arg, ...) \
mapper(arg) __VA_OPT__(IMacroExtractSep(sep) MacroDefer(IMacroArgmapRef) (mapper, sep, __VA_ARGS__))

#define IMacroArgmapRef() IMacroArgmap

#define MacroArgmap(mapper, sep, ...) \
__VA_OPT__(IMacroMassExpand(IMacroArgmap(mapper, sep, __VA_ARGS__)))
//IMacroMassExpand(IMacroArgmap(mapper, __VA_ARGS__, IMacroListTerminator))

#define IMacroHasComma(...) MacroApply(IMacro64th, __VA_ARGS__, IMacro62ones2zeros)

#define IMacroExtractSep(expr) MacroApply(IMacroConcat2, \
IMacroExtractSep_, IMacroInParentheses(expr) \
)(expr)
#define IMacroInParentheses(expr) IMacroIsEmpty(MacroNone expr)
#define IMacroExtractSep_0(expr) expr
#define IMacroExtractSep_1(expr) IMacroFirstOrComma expr

#define IMacroFirstOrComma(...) __VA_OPT__( \
MacroApply(IMacroConcat2, IMacroFirstOrComma_, IMacroHasComma(__VA_ARGS__)) (__VA_ARGS__) \
)
#define IMacroFirstOrComma_0(a, ...) a
#define IMacroFirstOrComma_1(...) ,


#define IMacroReduce(reducer, result, arg, ...) IMacroIf(IMacroIsEmpty(__VA_ARGS__), \
reducer(result, arg), \
MacroDefer3(IMacroReduceRef) (reducer, reducer(result, arg), __VA_ARGS__) \
)

//#define IMacroReduce(reducer, result, arg, ...) \
//__VA_OPT__(Defer(IMacroReduceRef)) (__VA_OPT__(reducer,) reducer(result, arg) \
//__VA_OPT__(, __VA_ARGS__) \
//)

#define IMacroReduceRef() IMacroReduce

#define MacroReduce(reducer, init, ...) MacroSwitch(IMacroIsEmpty(__VA_ARGS__), init, \
IMacroMassExpand(IMacroReduce(reducer, init, __VA_ARGS__)) \
)


#define MacroConcat(...) MacroReduce(IMacroConcat2,  , __VA_ARGS__)
