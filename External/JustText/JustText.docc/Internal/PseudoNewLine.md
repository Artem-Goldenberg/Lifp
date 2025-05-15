# ``JTPseudoNewLine``

Either empty or a dummy name to be replaced with an actual new line

## Overview

In case we want to debug out macro output, it will be much more readable with the newlines inserted
You can't add new lines to your macros in C, but if we just generate this token called `JTPseudoNewLine`,
Then we can replace it later with external program to a `\n`, and see the output with newlines

If not in debug mode, then this PseudoNewLine token will just expand to the empty thing and not cause trouble

## Topics

### See also:

- <doc:Debugging>
