/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_CREATE = 258,
     T_DROP = 259,
     T_USE = 260,
     T_DATABASE = 261,
     T_SHOW = 262,
     T_TABLES = 263,
     T_DESC = 264,
     T_TABLE = 265,
     T_NOT = 266,
     T_NULL = 267,
     T_SET = 268,
     T_AND = 269,
     T_SELECT = 270,
     T_FROM = 271,
     T_WHERE = 272,
     T_INSERT = 273,
     T_UPDATE = 274,
     T_DELETE = 275,
     T_INTO = 276,
     T_VALUES = 277,
     T_EQ = 278,
     T_LT = 279,
     T_LE = 280,
     T_GT = 281,
     T_GE = 282,
     T_NE = 283,
     NOTOKEN = 284,
     T_INT = 285,
     T_REAL = 286,
     T_STRING = 287,
     T_QSTRING = 288
   };
#endif
/* Tokens.  */
#define T_CREATE 258
#define T_DROP 259
#define T_USE 260
#define T_DATABASE 261
#define T_SHOW 262
#define T_TABLES 263
#define T_DESC 264
#define T_TABLE 265
#define T_NOT 266
#define T_NULL 267
#define T_SET 268
#define T_AND 269
#define T_SELECT 270
#define T_FROM 271
#define T_WHERE 272
#define T_INSERT 273
#define T_UPDATE 274
#define T_DELETE 275
#define T_INTO 276
#define T_VALUES 277
#define T_EQ 278
#define T_LT 279
#define T_LE 280
#define T_GT 281
#define T_GE 282
#define T_NE 283
#define NOTOKEN 284
#define T_INT 285
#define T_REAL 286
#define T_STRING 287
#define T_QSTRING 288




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 29 "yacc.y"
{
	int ival;
	float rval;
	char *sval;
	CompOp cval;
	NODE *n;
}
/* Line 1529 of yacc.c.  */
#line 123 "yacc.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

