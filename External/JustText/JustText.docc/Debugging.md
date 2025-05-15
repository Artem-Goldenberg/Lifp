# Debugging

Some capabilities this library provides for debugging the output of it's macroses.

## Overview

Well, there are actually not a lot of capabilities right now, so...

> Important:
    First of all, you enable the debugging `mode` by defining the `JTDebug` macro.

Now, when you are in debugging mode, mapping macros like ``JTForEach`` 
(and only this one for now), will insert special ``JTPseudoNewLine`` tokens.
You can convert this tokens later by some simple external program into new lines.
This will allow you to better understand visually the output of this macro.
Combining it with the known compiler error messsage should give you enough information
to figure out what's wrong, in the most and simple cases at least.

So, for example, consider we want to debug some macro code with a simple mistake:

```c
#define JTDebug
#include "JustText.h"

#define Functions some, more, nomore
#define Declare(func) void func(void)

JTForEach(Delcare, Functions)
```

It expands to

```c
void some(void) JTPseudoNewLine void more(void) JTPseudoNewLine void nomore(void)
```

And after converting the new lines

```c
void some(void)
void more(void)
void nomore(void)
```

Now it's more clear what the mistake is: we forgot to add `;` at the end of the line
in our `Declare` macro
