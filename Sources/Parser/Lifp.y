%code requires {
    #include "Lifp.h"
}
%code {
#define AssignNonNull(var, expr) if (!(var = (expr))) YYNOMEM
    int yylex(void);
    void yyerror(ProgramInfo* program, const char* message);
}

%define parse.error verbose

%define api.value.type union

%token
    QUOTE  "quote"
    SETQ   "setq"
    FUNC   "func"
    LAMBDA "lambda"
    PROG   "prog"
    COND   "cond"
    WHILE  "while"
    RETURN "return"
    BREAK  "break"
    TRUE   "true"
    FALSE  "false"
    NIL    "null"
    QUOTESYMBOL "'"
    <float> REAL
    <int>   INTEGER
    <ID>    IDENTIFIER

%type <List*> manyElements program
%type <Element*> element listContent literal
%type <IdentifierList*> parameters manyIdentifiers

%parse-param {ProgramInfo* program}

%%

program: manyElements { program->syntaxRoot = $$ = $1; }

manyElements:
    %empty { AssignNonNull($$, node.list()); }
  | manyElements element { $$ = $1; if (!node.appendElement($1, $2)) YYNOMEM; }
  | manyElements error

element:
    "'" element { AssignNonNull($$, node.quote($2));      }
  | IDENTIFIER  { AssignNonNull($$, node.identifier($1)); }
  | literal
  | '(' listContent ')' { $$ = $2; }

listContent:
    "quote" element                      { AssignNonNull($$, node.quote($2));          }
  | "setq" IDENTIFIER element            { AssignNonNull($$, node.setq($2, $3));       }
  | "func" IDENTIFIER parameters element { AssignNonNull($$, node.func($2, $3, $4));   }
  | "lambda" parameters element          { AssignNonNull($$, node.lambda($2, $3));     }
  | "prog" parameters element            { AssignNonNull($$, node.prog($2, $3));       }
  | "cond" element element               { AssignNonNull($$, node.cond($2, $3, NULL)); }
  | "cond" element element element       { AssignNonNull($$, node.cond($2, $3, $4));   }
  | "while" element element              { AssignNonNull($$, node.whileNode($2, $3));  }
  | "return" element                     { AssignNonNull($$, node.returnNode($2));     }
  | "break"                              { AssignNonNull($$, node.breakNode());        }
  | manyElements {
      $$ = (Element*)$1;
      node.finalize();
  }

parameters: '(' manyIdentifiers ')' { $$ = $2; node.finalize(); }

manyIdentifiers:
    %empty { AssignNonNull($$, node.identifierList()); }
    | manyIdentifiers IDENTIFIER { $$ = $1; if (!node.appendIdentifier($1, $2)) YYNOMEM; }

literal:
    INTEGER { AssignNonNull($$, node.integer($1)); }
  | REAL    { AssignNonNull($$, node.real($1));    }
  | "true"  { AssignNonNull($$, node.trueNode());  }
  | "false" { AssignNonNull($$, node.falseNode()); }
  | "null"  { AssignNonNull($$, node.null());      }

%%
