#  Lifp

Learning project to make a compiler for a Lisp-like language


Need to have `bison` tool installed and available in PATH


To run parser:
```
make parser
./parser Tests/Examples/Advanced/mergeSort.lifp
```

It should output parse tree with some syntax errors


To run lexer:
```
make lexer
./lexer
```

It should output a list of filepaths where streams of tokens were written

