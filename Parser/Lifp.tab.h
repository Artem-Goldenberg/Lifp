/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_LIFP_TAB_H_INCLUDED
# define YY_YY_LIFP_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 4 "Lifp.y"

    #include "Lifp.h"
    #include "Syntax.h"

#line 54 "Lifp.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    QUOTE = 258,                   /* "quote"  */
    SETQ = 259,                    /* "setq"  */
    FUNC = 260,                    /* "func"  */
    LAMBDA = 261,                  /* "lambda"  */
    PROG = 262,                    /* "prog"  */
    COND = 263,                    /* "cond"  */
    WHILE = 264,                   /* "while"  */
    RETURN = 265,                  /* "return"  */
    BREAK = 266,                   /* "break"  */
    TRUE = 267,                    /* "true"  */
    FALSE = 268,                   /* "false"  */
    NIL = 269,                     /* "null"  */
    QUOTESYMBOL = 270,             /* "'"  */
    OPENPAREN = 271,               /* "("  */
    CLOSEPAREN = 272,              /* ")"  */
    COMMENT = 273,                 /* COMMENT  */
    REAL = 274,                    /* REAL  */
    INTEGER = 275,                 /* INTEGER  */
    IDENTIFIER = 276               /* IDENTIFIER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
  Element* start;                          /* start  */
  Element* element;                        /* element  */
  Element* listContent;                    /* listContent  */
  Element* literal;                        /* literal  */
  ID IDENTIFIER;                           /* IDENTIFIER  */
  IdentifierList* parameters;              /* parameters  */
  IdentifierList* manyIdentifiers;         /* manyIdentifiers  */
  List* program;                           /* program  */
  List* manyElements;                      /* manyElements  */
  float REAL;                              /* REAL  */
  int INTEGER;                             /* INTEGER  */

#line 104 "Lifp.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (Element **root);


#endif /* !YY_YY_LIFP_TAB_H_INCLUDED  */
