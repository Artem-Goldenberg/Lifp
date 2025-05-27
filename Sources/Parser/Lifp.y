%code requires {
    #include "Lifp.h"
}
%code {
#define AssignNonNull(var, expr) if (!(var = (expr))) YYNOMEM
    int yylex(ProgramInfo* program);
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

%type <Array*> manyElements program
%type <Element*> element listContent literal
%type <IdentifierList*> parameters manyIdentifiers

%param {ProgramInfo* program}

%%

program: manyElements { $$ = $1; }

manyElements:
    %empty { AssignNonNull($$, node.startArray()); }
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
  | "cond" element element               { AssignNonNull($$, node.cond($2, $3, NULL)); }
  | "cond" element element element       { AssignNonNull($$, node.cond($2, $3, $4));   }
  | "while" element element              { AssignNonNull($$, node.whileNode($2, $3));  }
  | "return" element                     { AssignNonNull($$, node.returnNode($2));     }
  | "break"                              { AssignNonNull($$, node.breakNode());        }
  | "prog" parameters <Element*>{
      // this is a mid-rule action, we know it's gonna be a prog node, so we create one and
      // then we allow actions defined on `manyElements` to fill in the rest of this prog
      AssignNonNull($$, node.prog($2));
  }
  '(' manyElements ')' {
      $$ = $3; // the value gained is the value from a mid-rule action
      node.finalize();
  }
  |
  <Element*>{ // mid rule action again, we know it's gonna be just a list node, so we allocate it
      $$ = node.list();
  }
  manyElements { $$ = $1; node.finalize(); }

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

    yytoken_kind_t translation[TokensCount] = {
        [token.Quote] = QUOTE,
        [token.Setq] = SETQ,
        [token.Func] = FUNC,
        [token.Lambda] = LAMBDA,
        [token.Prog] = PROG,
        [token.Cond] = COND,
        [token.While] = WHILE,
        [token.Return] = RETURN,
        [token.Break] = BREAK,
        [token.Identifier] = IDENTIFIER,
        [token.Integer] = INTEGER,
        [token.Real] = REAL,
        [token.True] = TRUE,
        [token.False] = FALSE,
        [token.Null] = NIL,
        [token.QuoteSymbol] = QUOTESYMBOL,
        [token.OpenParen] = '(',
        [token.CloseParen] = ')',
        [token.End] = YYEOF
    };

    int yylex(ProgramInfo* program) {
        Token t;

        // skip all commentss
        do {
            t = getToken(program->lexer);
            assert(t < TokensCount);
        } while (t == token.Comment);

        // set yylval if tokens has a value
        switch (t) {
            case token.Integer:
            yylval.INTEGER = getInteger(program->lexer);
            break;
            case token.Real:
            yylval.REAL = getReal(program->lexer);
            break;
            case token.Identifier:
            yylval.IDENTIFIER = getIdentifier(program->lexer);
            break;
            default: break;
        }

        return translation[t];
    }

    void yyerror(ProgramInfo* program, const char* message) {
        Issue error = {
            .type = issueType.SyntaxError,
            .start = getTokenStartLocation(program->lexer),
            .end = getCurrentLocation(program->lexer),
            .data = {
                .generalError = (GeneralError) {
                    .message = strdup(message)
                }
            }
        };
        addIssue(program, &error);
    }
