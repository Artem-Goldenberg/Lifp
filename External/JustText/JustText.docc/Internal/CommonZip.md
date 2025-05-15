# ``JTCommonZip``

Lower level macro for all the `Zip` macros.

## Overview

Just like with ``JTCommonMap``, this macro does the same thing, it accepts
an `extractor` argument, which again can be on of three for now, and starts the
expansion engine, giving it the recursive
(kind of) ``JTRealZip`` call to do the actual zipping work.


## Topics

### Possible extractor values: 

- ``JTParensSepExtractor``
- ``JTSepProducerExtractor``
- ``JTPlainSepExtractor``
