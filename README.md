#  Lifp

Learning project to make a compiler for a Lisp-like language


Need to have `bison` tool installed and available in PATH

To run the evaluator:
```
make eval
./eval Tests/Examples/simple.lifp
```
*For now it hits assertion on run*


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

It should output a list of files which were tokenized.  
Token streams are written to the files of the form `Test/Output/<original_file>.lifp.out`

