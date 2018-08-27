/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_TOC_PARSE_H_INCLUDED
# define YY_YY_TOC_PARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUMBER = 258,
    STRING = 259,
    CATALOG = 260,
    CD_DA = 261,
    CD_ROM = 262,
    CD_ROM_XA = 263,
    TRACK = 264,
    AUDIO = 265,
    MODE1 = 266,
    MODE1_RAW = 267,
    MODE2 = 268,
    MODE2_FORM1 = 269,
    MODE2_FORM2 = 270,
    MODE2_FORM_MIX = 271,
    MODE2_RAW = 272,
    RW = 273,
    RW_RAW = 274,
    NO = 275,
    COPY = 276,
    PRE_EMPHASIS = 277,
    TWO_CHANNEL_AUDIO = 278,
    FOUR_CHANNEL_AUDIO = 279,
    ISRC = 280,
    SILENCE = 281,
    ZERO = 282,
    AUDIOFILE = 283,
    DATAFILE = 284,
    FIFO = 285,
    START = 286,
    PREGAP = 287,
    INDEX = 288,
    CD_TEXT = 289,
    LANGUAGE_MAP = 290,
    LANGUAGE = 291,
    TITLE = 292,
    PERFORMER = 293,
    SONGWRITER = 294,
    COMPOSER = 295,
    ARRANGER = 296,
    MESSAGE = 297,
    DISC_ID = 298,
    GENRE = 299,
    TOC_INFO1 = 300,
    TOC_INFO2 = 301,
    UPC_EAN = 302,
    SIZE_INFO = 303
  };
#endif
/* Tokens.  */
#define NUMBER 258
#define STRING 259
#define CATALOG 260
#define CD_DA 261
#define CD_ROM 262
#define CD_ROM_XA 263
#define TRACK 264
#define AUDIO 265
#define MODE1 266
#define MODE1_RAW 267
#define MODE2 268
#define MODE2_FORM1 269
#define MODE2_FORM2 270
#define MODE2_FORM_MIX 271
#define MODE2_RAW 272
#define RW 273
#define RW_RAW 274
#define NO 275
#define COPY 276
#define PRE_EMPHASIS 277
#define TWO_CHANNEL_AUDIO 278
#define FOUR_CHANNEL_AUDIO 279
#define ISRC 280
#define SILENCE 281
#define ZERO 282
#define AUDIOFILE 283
#define DATAFILE 284
#define FIFO 285
#define START 286
#define PREGAP 287
#define INDEX 288
#define CD_TEXT 289
#define LANGUAGE_MAP 290
#define LANGUAGE 291
#define TITLE 292
#define PERFORMER 293
#define SONGWRITER 294
#define COMPOSER 295
#define ARRANGER 296
#define MESSAGE 297
#define DISC_ID 298
#define GENRE 299
#define TOC_INFO1 300
#define TOC_INFO2 301
#define UPC_EAN 302
#define SIZE_INFO 303

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 30 "toc_parse.y" /* yacc.c:1909  */

	long ival;
	char *sval;

#line 155 "toc_parse.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_TOC_PARSE_H_INCLUDED  */
