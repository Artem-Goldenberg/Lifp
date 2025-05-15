# ``JTZip``

Sequentially applies first argument to two elements from the lists, 
customly gluing the results.

## Overview

Most importants stuff for now:

- Parameter zipper: function-like macro to which two arguments will be provided,
    one from `xs` and one from `ys` until they are exhausted
- Parameter sep: Then, the results will go in a sequence, separated by the 
    whatever `sep` is
- Parameter xs: comma separated list **in parentheses**, for example: `(1, 2, 3)`
- Parameter ys: also comma separated list in parens, just like `xs`

Two lists: `xs` and `ys` for now absolutely **MUsT** be the same size, or everything will break
