# ``JTCommonMap``

Lower-level mapping macro

## Overview

All three of the maps in this library just call this one with the appropriate extractor macro.
Now, extractor macros are easy, they do exactly what their name tells, they extract the separator
using different strategies.
This map just sets thing up for the ``JTRealMap`` to do the work. But real map needs a lot of expansions
to finish it's work. So, we conviniently already have our ``JTMassExpand`` macro which we use here as an
engine, driving our recursive expansions.

Of course, if no arguments are provided to the map, then it shouldn't run at all and just expand to empty thing,
`__VA_OPT__` helps us with that, it expands to it's argument only if `__VA_ARGS__` are not empty.

- Parameters:
    - mapper: Macro which is applied to every argument in the `...`
    - sep: Data on a separator of the resulting sequence
    - extractor: Macro, which given the `sep` data, expands to the actual separator
    - `...`: Sequence to process

## Topics

### Possible extractor values: 

- ``JTParensSepExtractor``
- ``JTSepProducerExtractor``
- ``JTPlainSepExtractor``
