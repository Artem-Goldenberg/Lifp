%code top {
    #include <stdio.h>
}
%code requires {
    #include "Lifp.h"
    #include "Syntax.h"
}
%code {
    int yylex(void);
    void yyerror(List** root, const char* message);
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
    OPENPAREN   "("
    CLOSEPAREN  ")"
    COMMENT
    <float> REAL
    <int>   INTEGER
    <ID>    IDENTIFIER

%type <List*> manyElements program
%type <Element*> element listContent literal
%type <IdentifierList*> parameters manyIdentifiers

%parse-param {List** root}

%%

program: manyElements { *root = $$ = $1; }

manyElements:
    %empty { $$ = node.list(); }
  | manyElements element { $$ = $1; node.appendElement($1, $2); }

element:
    "'" element { $$ = node.quote($2);      }
  | IDENTIFIER  { $$ = node.identifier($1); }
  | literal
  | "(" listContent ")" { $$ = $2; }

listContent:
    "quote" element                      { $$ = node.quote($2);          }
  | "setq" IDENTIFIER element            { $$ = node.setq($2, $3);       }
  | "func" IDENTIFIER parameters element { $$ = node.func($2, $3, $4);   }
  | "lambda" parameters element          { $$ = node.lambda($2, $3);     }
  | "prog" parameters element            { $$ = node.prog($2, $3);       }
  | "cond" element element               { $$ = node.cond($2, $3, NULL); }
  | "cond" element element element       { $$ = node.cond($2, $3, $4);   }
  | "while" element element              { $$ = node.whileNode($2, $3);  }
  | "return" element                     { $$ = node.returnNode($2);     }
  | "break"                              { $$ = node.breakNode(); }
  | manyElements {
      $$ = (Element*)$1;
      node.finalize();
  }

parameters: "(" manyIdentifiers ")" { $$ = $2; node.finalize(); }

manyIdentifiers:
    %empty { $$ = node.identifierList(); }
    | manyIdentifiers IDENTIFIER { $$ = $1; node.appendIdentifier($1, $2); }

literal:
    INTEGER { $$ = node.integer($1); }
  | REAL    { $$ = node.real($1);    }
  | "true"  { $$ = node.trueNode();  }
  | "false" { $$ = node.falseNode(); }
  | "null"  { $$ = node.null();      }

%%

