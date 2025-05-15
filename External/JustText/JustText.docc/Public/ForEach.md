# ``JTForEach``

Applies it's first argument to all the rest

## Overview

The cornerstone of this library. 
In fact, one of two reasons, I'm rewriting this whole thing is just to have this `ForEach`
macro looking more clear, than in the previous implementation of `MacroArgmap` version.
This library should define things a little bit more clearly and with less prefix garbage than my previous attempt.

- **Expands** to the space separated results of applying `mapper` to the rest
    of the arguments.

If you are not interested in debugging your macro output, then 
it's equivalent to the use of ``JTMap`` with an empty separator
