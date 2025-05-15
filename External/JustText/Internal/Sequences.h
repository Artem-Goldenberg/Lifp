#ifndef JustText_Sequences_h
#define JustText_Sequences_h

/// Either empty or a dummy name to be replaced with an actual new line
#ifndef JTDebug
#define JTPseudoNewLine
#endif


/// Unpacks the parenthesised expression
#define JTParensSepExtractor(sep) JTId sep
/// Just return the separator as is
#define JTSepProducerExtractor(sep) sep()
/// Calls the given `sep` macro to get the separator
#define JTPlainSepExtractor(sep) sep


/// Lower-level mapping macro
#define JTCommonMap(mapper, sep, extractor, aux, ...) __VA_OPT__(\
    JTMassExpand(JTRealMap(mapper, sep, extractor, aux, __VA_ARGS__))\
)

#define JTInnerApp(next, ...) next(__VA_ARGS__)

/// Actual *recursivish* macro to map the arguments
#define JTRealMap(mapper, sep, extractor, aux, arg, ...) \
    JTInnerApp(mapper, arg JTAfterComma aux) \
    __VA_OPT__( \
        extractor(sep) \
        JTRealMapRef JTParens (mapper, sep, extractor, aux, __VA_ARGS__)\
    )

/// Deferred call of ``JTRealMap``
#define JTRealMapRef() JTRealMap


// Lower level macro for all the `Zip` macros.
#define JTCommonZip(zipper, sep, extractor, xs, ...) __VA_OPT__(\
    JTMassExpand(JTRealZip(zipper, sep, extractor, xs, __VA_ARGS__))\
)

/// Just expands to the first argument
#define JTHead(first, ...) first
/// Cuts off the first argument from a list
#define JTTail(first, ...) (__VA_ARGS__)
/// Apply directly to the parenthesized list to expand it's members outside
#define JTUnwrap(list) JTExpand list

/// Actual recursive like macro for zipping two lists
#define JTRealZip(zipper, sep, extractor, list, arg, ...) \
    zipper(JTHead list, arg) \
    __VA_OPT__(extractor(sep)) \
    __VA_OPT__(JTRealZipRef JTParens (zipper, sep, extractor, JTTail list, __VA_ARGS__))

/// Deferred call of the ``JTRealZip``
#define JTRealZipRef() JTRealZip


#endif /* JustText_Sequences_h */
