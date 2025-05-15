# ``JTRealMap``

Actual *recursivish* macro to map the arguments

## Overview

It is so real. It was first called `RecursiveMap`, but Real is cooler and shorter.
So all this map does is exactly what you'd expect.
1. It applies our `mapper` macro to the first argument (out of the arguments we need to process)
2. If more arguments are left in the list, we put down our separator with the help of the `extractor`,
3. After the extracted separator, we need to somehow call `JTRealMap` to process all other elements
4. C macros can't directly call themselves, so we use a trick I picked up from this article:
`https://www.scs.stanford.edu/~dm/blog/va-opt.html`

Idea of the trick is that we can defer the invokation of this ``JTRealMapRef``
(which would eventually give us our RealMap back) by putting ``JTParens`` macro instead
of the real parentheses. So it will require an extra expansion to evaluate this thing, and that means,
when it is evaluated, all the bits and stuff for our initial `JTRealMap` macro will be cleared, it will be expanded again,
finally creating our so desired recursion! So, because of these delayed operations, preprocessor forgets that it has
already expanded our `JTRealMap` macro. Still don't understand it in comlete details, stuff about replacing bits, but the
basic idea is this.

Because we are just using this `__VA_OPT__` thing, the implementation is really straightforwrad here.
We don't have to explicitly clear any bits (still not sure if it's even possible), or try to implement this `__VA_OPT__`
from scratch, which is actually not as easy as it may seem.
