# ``JustText``
*After all, it's just a bunch of text*

@Metadata {
    @PageColor(orange)
}

## Overview

A little library providing utility macros to use in C like languages.

This is a refined version of my older macro libraries. 
The naming tries to be consistent, by that I mean I just use the `JT` prefix everywhere.
I don't bother with internal prefixes, because library is too small for that.

> Warning:
    This library uses `__VA_OPT__` which is not standard C (not yet, I think). 
    `__VA_OPT__` is provided either by the GNU C extension or by the C++ 20 Standard. 
    So if you don't have any of those,  then `__VA_OPT__` is probably not working. 
    And that means the most important macroses of this library will not work for you correctly. 

So, gotta have `__VA_OPT__`,
without it it's very hard to implement recursion in macros, at least that's what I concluded. That the most readable version
of the `ForEach` macros and such can be achived only with this `__VA_OPT__` stuff; I think it can be simulated more or
less with standard stuff, but not worth the effor I think.

Perhaps there are other things I used here, which are not standard, but I have no idea of them

### Internal and Public Macros

The separation on public and internal macroses are not very strict, meaning you can
use internal macroses as well as public ones. I just suppose public macros are gonna 
be much more useful in practice, then the internal ones.
But I provide documentation for some of them anyway, because they are 
important to understand how this library works.

@Links(visualStyle: list) { 
    - <doc:Debugging>
    - <doc:WhyMacros>
}

### Things to be done

1. Divide into sub files
2. Write zipWith variadic macro for 2 variadic lists ---> Done, but make work for different sizes
3. Write genericTranspose macro for variadic amount of variadic lists ??

## Topics

### Public Commons

- ``JTIf``
- ``JTString``
- ``JTCount``

### Public Mappers

- ``JTForEach``
- ``JTMap``
- ``JTMapPlain``
- ``JTMapComma``
- ``JTMapParenSep``
- ``JTMapMakeSep``

### Public Zippers

- ``JTZip``
- ``JTZipPlain``
- ``JTZipComma``

### Basic internals

- ``JTApp``
- ``JTGetThird``
- ``JTParens``
- ``JTGetStringForReal``
- ``JTOne``
- ``JTExpand``
- ``JTMassExpand``
- ``JTPseudoNewLine``

### Internal Separator Extractors

- ``JTParensSepExtractor``
- ``JTSepProducerExtractor``
- ``JTPlainSepExtractor``

### Internal Map Support

- ``JTCommonMap``
- ``JTRealMap``
- ``JTRealMapRef``

### Internal Zip Support

- ``JTCommonZip``
- ``JTHead``
- ``JTTail``
- ``JTRealZip``
- ``JTRealZipRef``
