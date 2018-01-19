/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_GRAMMAR_H_INCLUDED
# define YY_YY_GRAMMAR_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    EOL = 258,
    _RULE_ = 259,
    _META_ = 260,
    _STRINGS_ = 261,
    _CONDITION_ = 262,
    _NONE_ = 263,
    _REQUEST_ = 264,
    _AGENT_ = 265,
    _REFER_ = 266,
    _STATUS_ = 267,
    _REMOTE_USER_ = 268,
    _URL_DECODE_ = 269,
    _TEXTSTRING_ = 270,
    _STRING_IDENTIFIER_ = 271,
    _IDENTIFIER_ = 272,
    _AND_ = 273,
    _OR_ = 274,
    _NOT_ = 275,
    _DIGIT_NUMBER_ = 276,
    _REMOTEADDR_ = 277,
    _TIMELOCAL_ = 278,
    _REGEXP_ = 279,
    _REQUSET_METHOD_ = 280
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 21 "grammar.y" /* yacc.c:1909  */

  struct ast *a;
  char* c_string;
  char* c_string1;
  YR_META *meta; 
  int request_method;
  int64_t  integer;

#line 89 "grammar.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GRAMMAR_H_INCLUDED  */
