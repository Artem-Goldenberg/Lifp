# ``JTMap``

Applies first argument to all the rest, and custom glue the results

## Overview

Much more important than ``JTForEach`` is the `Map`. It is more generalised version of foreach, which supports
arbitrary separators between token outputs, instead of just empty space as in the case of `ForEach`.
Of course we could generate separators in our own `mapper` macro which is invoked on every argument. But than,
this corner case with the last argument, where we don't need to insert a separator, we would have to always think about
this. Instead, `Map` will take care of it for you and only insert separator between elements and not after the last one.


### About separator

The thing about `sep` is that it is a real pain. Because it is very common to want to separate
results by a comma, but if we just pass `,` when invoking `Map`, obviously it would be wrong, because 
preprocessor considers`,` as argument separator and not as argument itself. So why don't we just create a 
special version of Argmap which would separate it's outputs by a comma? Well, sometimes you need more complicated
separators, with a comma inside of them somewhere. Maybe if user provides a macro which expands to comma, maybe
this would help? Well, if this macro needs to be applied to a set of parenthesys, then yes, it would work just fine. But to
create a separate macro for each separator, even if it is a simple one seems like unnececcary work, besides adding more
macros to the code may decrease readability of code.

The old library solution was to detect whether `sep` is enclosed in parenthesys, and if it is, then use what's inside the parens
as a sep. If `sep` is not in parentheses, then just use it as is. 
This solution was very easy to use for the user, but the macro definition itself turned out to be very unreadable because of it.

So in this library, there are three different maps (at least for now): this one which accepts a simple plan seperator without
commas, another one which, like in the old library, extracts separator from the parens around it, and last one which would
accept a separator producer and invoke it when the separator is needed, it may even take as arguments the two elements
which needs to be separated by this in the future.

- Parameter sep: a separator which will be used to separate the resulting sequence
