#ifndef JustText_h
#define JustText_h

#include "Internal/Internal.h"
#include "Internal/Sequences.h"

/// Simulates an if condition on a macro level.
#define JTIf(detecter, onTrue, ...) JTGetThird(detecter, onTrue, __VA_ARGS__)

/// Converts all args to one string
#define JTString(...) JTGetStringForReal(__VA_ARGS__)

/// Counts it's arguments
#define JTCount(...) JTMap(JTOne, +, __VA_ARGS__)

// MARK: - Projections and dummy dumbs

/// Fancy name for taking argument at some position
#define JTGetFirst(one, ...) one
#define JTGetSecond(one, two, ...) two
#define JTGetThird(one, two, three, ...) three
#define JTGetFourth(one, two, three, four, ...) four
// enough for now

/// Just take some prefix of the list
#define JTTakeTwo(one, two, ...) one, two
#define JTTakeThree(one, two, three, ...) one, two, three

// MARK: - Maps

/// Applies it's first argument to all the rest
#define JTForEach(mapper, ...) JTMap(mapper, JTPseudoNewLine, __VA_ARGS__)

/// Applies first argument to all the rest, and custom glue the results
#define JTMap(mapper, sep, ...) JTMapPlain(mapper, sep, __VA_ARGS__)

/// Maps arguments using a simple separator in-between
#define JTMapPlain(mapper, sep, ...) \
    JTCommonMap(mapper, sep, JTPlainSepExtractor, (), __VA_ARGS__)

/// Maps arguments, putting comma in-between
#define JTMapComma(mapper, ...) JTMapParenSep(mapper, (,), __VA_ARGS__)

/// Maps arguments, putting unpacked separator in-between
#define JTMapParenSep(mapper, sep, ...) \
    JTCommonMap(mapper, sep, JTParensSepExtractor, (), __VA_ARGS__)

/// Maps arguments, putting a specifically produced separator in-between
#define JTMapMakeSep(mapper, sep, ...) \
    JTCommonMap(mapper, sep, JTSepProducerExtractor, (), __VA_ARGS__)

/// This and the following maps are for very specific usage: when you need to map inside a map, expaling later
#define JTInnerMap(mapper, sep, ...) __VA_OPT__( \
    JTRealMapRef JTParens (mapper, sep, JTPlainSepExtractor, (), __VA_ARGS__) \
)

#define JTInnerMapComma(mapper, ...) __VA_OPT__( \
    JTRealMapRef JTParens (mapper, (,), JTParensSepExtractor, (), __VA_ARGS__) \
)

#define JTInnerForEach(mapper, ...) JTInnerMap(mapper, , __VA_ARGS__)

// MARK: - Zips

/// Sequentially applies first argument to two elements from the lists, customly gluing the results.
#define JTZip(zipper, sep, xs, ys) JTZipPlain(zipper, sep, xs, ys)

/// Zips two argument lists, putting user-provided separator in-between
#define JTZipPlain(zipper, sep, xs, ys) \
    JTCommonZip(zipper, sep, JTPlainSepExtractor, xs, JTExpand ys)

/// Zips two argument lists, separating the results by a comma
#define JTZipComma(zipper, xs, ys) \
    JTCommonZip(zipper, (,), JTParensSepExtractor, xs, JTExpand ys)

// MARK: - Other utilities

// TODO: sort this out later

#define JTInnerMapCommaAux(mapper, aux, ...) __VA_OPT__( \
    JTRealMapRef JTParens (mapper, (,), JTParensSepExtractor, (aux), __VA_ARGS__) \
)

#define JTMapAux(mapper, sep, aux, ...) \
    JTCommonMap(mapper, sep, JTPlainSepExtractor, (aux), __VA_ARGS__)

#define JTMapCommaAux(mapper, aux, ...) \
    JTCommonMap(mapper, (,), JTParensSepExtractor, (aux), __VA_ARGS__)

#define JTInnerMapAux(mapper, sep, aux, ...) __VA_OPT__( \
    JTRealMapRef JTParens (mapper, sep, JTPlainSepExtractor, (aux), __VA_ARGS__) \
)

#define JTInnerMapParenSepAux(mapper, sep, aux, ...) __VA_OPT__( \
    JTRealMapRef JTParens (mapper, sep, JTParensSepExtractor, (aux), __VA_ARGS__) \
)

#define JTInnerMapParenSep(mapper, sep, ...) __VA_OPT__( \
    JTRealMapRef JTParens (mapper, sep, JTParensSepExtractor, (), __VA_ARGS__) \
)


//#define JTInnerMapAux(mapper, sep, aux, ...)

#define JTCommaIfSome(...) __VA_OPT__(,)
#define JTUnwrapWithComma(list) JTCommaIfSome(JTId list) JTId list

#define JTWithCommaAfter(...) __VA_ARGS__ __VA_OPT__(,)
#define JTWithCommaBefore(...) __VA_OPT__(,) __VA_ARGS__

#define JTForEachAux(mapper, aux, ...) \
    JTMapAux(mapper, JTPseudoNewLine, aux, __VA_ARGS__)

#define JTInnerForEachAux(mapper, aux, ...) \
    JTInnerMapAux(mapper, JTPseudoNewLine, aux, __VA_ARGS__)


#define JTRealReduceRef() JTRealReduce

#define JTRealReduce(reducer, result, arg, ...) \
    __VA_OPT__(JTRealReduceRef JTParens) JTIf(__VA_OPT__(,), \
        (reducer, reducer(result, arg), __VA_ARGS__), \
        reducer(result, arg) \
    )

#define JTReduce(reducer, init, ...) JTIf(__VA_OPT__(,),\
    JTMassExpand(JTRealReduce(reducer, init, __VA_ARGS__)), \
    init \
)

#define JTDropLastReducer(old, arg)
#define JTRealDropLastRef() JTRealDropLast
#define JTRealDropLast(arg, ...) __VA_OPT__(arg, JTRealDropLastRef JTParens (__VA_ARGS__))
#define JTDropLast(...) __VA_OPT__(JTMassExpand(JTRealDropLast(__VA_ARGS__)))


#define JTConcat2(a, b) a##b

#define JTRealConcat(a, ...) JTReduce(JTConcat2, a, __VA_ARGS__)

#define JTConcat(...) __VA_OPT__(JTRealConcat(__VA_ARGS__))




#endif /* JustText_h */
