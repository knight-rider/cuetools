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

#ifndef YY_YY_CUE_PARSE_H_INCLUDED
# define YY_YY_CUE_PARSE_H_INCLUDED
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
    CDTEXTFILE = 261,
    FFILE = 262,
    BINARY = 263,
    MOTOROLA = 264,
    AIFF = 265,
    WAVE = 266,
    MP3 = 267,
    FLAC = 268,
    TRACK = 269,
    AUDIO = 270,
    MODE1_2048 = 271,
    MODE1_2352 = 272,
    MODE2_2336 = 273,
    MODE2_2048 = 274,
    MODE2_2342 = 275,
    MODE2_2332 = 276,
    MODE2_2352 = 277,
    TRACK_ISRC = 278,
    FLAGS = 279,
    PRE = 280,
    DCP = 281,
    FOUR_CH = 282,
    SCMS = 283,
    PREGAP = 284,
    INDEX = 285,
    POSTGAP = 286,
    TITLE = 287,
    PERFORMER = 288,
    SONGWRITER = 289,
    COMPOSER = 290,
    ARRANGER = 291,
    MESSAGE = 292,
    DISC_ID = 293,
    GENRE = 294,
    TOC_INFO1 = 295,
    TOC_INFO2 = 296,
    UPC_EAN = 297,
    ISRC = 298,
    SIZE_INFO = 299,
    DATE = 300,
    DISCNUMBER = 301,
    GENRE0 = 302,
    REPLAYGAIN_ALBUM_GAIN = 303,
    REPLAYGAIN_ALBUM_PEAK = 304,
    REPLAYGAIN_TRACK_GAIN = 305,
    REPLAYGAIN_TRACK_PEAK = 306
  };
#endif
/* Tokens.  */
#define NUMBER 258
#define STRING 259
#define CATALOG 260
#define CDTEXTFILE 261
#define FFILE 262
#define BINARY 263
#define MOTOROLA 264
#define AIFF 265
#define WAVE 266
#define MP3 267
#define FLAC 268
#define TRACK 269
#define AUDIO 270
#define MODE1_2048 271
#define MODE1_2352 272
#define MODE2_2336 273
#define MODE2_2048 274
#define MODE2_2342 275
#define MODE2_2332 276
#define MODE2_2352 277
#define TRACK_ISRC 278
#define FLAGS 279
#define PRE 280
#define DCP 281
#define FOUR_CH 282
#define SCMS 283
#define PREGAP 284
#define INDEX 285
#define POSTGAP 286
#define TITLE 287
#define PERFORMER 288
#define SONGWRITER 289
#define COMPOSER 290
#define ARRANGER 291
#define MESSAGE 292
#define DISC_ID 293
#define GENRE 294
#define TOC_INFO1 295
#define TOC_INFO2 296
#define UPC_EAN 297
#define ISRC 298
#define SIZE_INFO 299
#define DATE 300
#define DISCNUMBER 301
#define GENRE0 302
#define REPLAYGAIN_ALBUM_GAIN 303
#define REPLAYGAIN_ALBUM_PEAK 304
#define REPLAYGAIN_TRACK_GAIN 305
#define REPLAYGAIN_TRACK_PEAK 306

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 50 "cue_parse.y" /* yacc.c:1909  */

	long ival;
	char *sval;

#line 161 "cue_parse.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_CUE_PARSE_H_INCLUDED  */
