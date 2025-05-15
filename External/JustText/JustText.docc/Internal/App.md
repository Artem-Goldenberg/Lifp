# ``JTApp``

Basic macro to apply the `next` argument on all 
the rest arguments and expand them along the way

## Overview

Convinient utility macro, it is used more in the old library, so will become more useful as this one grows
it's important to understand it, understand that it does an extra expansion of all arguments
and then, applies the first one (macro `next`) to the rest of the arguments, which could be empty, or whatever
