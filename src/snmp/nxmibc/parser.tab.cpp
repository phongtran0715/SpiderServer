/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         mpparse
#define yylex           mplex
#define yyerror         mperror
#define yydebug         mpdebug
#define yynerrs         mpnerrs

#define yylval          mplval
#define yychar          mpchar

/* Copy the first part of user declarations.  */
#line 1 "parser.y" /* yacc.c:339  */

/*
 * Copyright 2001, THE AUTHOR <mibparser@cvtt.net>
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *  
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *  
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * A parser for the basic grammar to use for snmp V2c modules
 */

#pragma warning(disable : 4065 4102)

/*
#ifndef __STDC__
#define __STDC__	1
#endif
*/

#define YYMALLOC	malloc
#define YYFREE		free

#include <nms_common.h>
#include <nms_util.h>
#include <string.h>
#include <time.h>
#include "mibparse.h"
#include "nxmibc.h"
#include "nxsnmp.h"

#define YYINCLUDED_STDLIB_H		1

#ifdef YYTEXT_POINTER
extern char *mptext;
#else
extern char mptext[];
#endif

#ifdef __64BIT__
#define YYSIZE_T  INT64
#endif

extern FILE *mpin, *mpout;
extern int g_nCurrLine;

static MP_MODULE *m_pModule;
#ifdef UNICODE
static char s_currentFilename[MAX_PATH];
#else
static const char *s_currentFilename;
#endif

int mperror(const char *pszMsg);
int mplex(void);
void ResetScanner();

MP_SYNTAX *create_std_syntax(int nSyntax)
{
   MP_SYNTAX *p = new MP_SYNTAX;
   p->nSyntax = nSyntax;
   return p;
}

static int AccessFromText(const char *pszText)
{
   static const char *pText[] = { "read-only", "read-write", "write-only",
                                  "not-accessible", "accessible-for-notify",
                                  "read-create", NULL };
   char szBuffer[256];
   int i;

   for(i = 0; pText[i] != NULL; i++)
      if (strcmp(pszText, pText[i]))
         return i + 1;
   sprintf(szBuffer, "Invalid ACCESS value \"%s\"", pszText);
   mperror(szBuffer);
   return -1;
}


#line 181 "parser.tab.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.tab.hpp".  */
#ifndef YY_MP_PARSER_TAB_HPP_INCLUDED
# define YY_MP_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int mpdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ENTERPRISE_SYM = 258,
    TRAP_TYPE_SYM = 259,
    VARIABLES_SYM = 260,
    EXPLICIT_SYM = 261,
    IMPLICIT_SYM = 262,
    IMPLIED_SYM = 263,
    RIGHT_BRACE_SYM = 264,
    LEFT_BRACE_SYM = 265,
    RIGHT_BRACKET_SYM = 266,
    LEFT_BRACKET_SYM = 267,
    DEFINITIONS_SYM = 268,
    ASSIGNMENT_SYM = 269,
    BEGIN_SYM = 270,
    END_SYM = 271,
    FROM_SYM = 272,
    IMPORTS_SYM = 273,
    EXPORTS_SYM = 274,
    COMMA_SYM = 275,
    SEMI_COLON_SYM = 276,
    DOT_SYM = 277,
    DESCRIPTION_SYM = 278,
    ORGANIZATION_SYM = 279,
    CONTACT_SYM = 280,
    UPDATE_SYM = 281,
    MODULE_IDENTITY_SYM = 282,
    MODULE_COMPLIANCE_SYM = 283,
    OBJECT_IDENTIFIER_SYM = 284,
    OBJECT_TYPE_SYM = 285,
    OBJECT_GROUP_SYM = 286,
    OBJECT_IDENTITY_SYM = 287,
    OBJECTS_SYM = 288,
    MANDATORY_GROUPS_SYM = 289,
    GROUP_SYM = 290,
    AGENT_CAPABILITIES_SYM = 291,
    KEYWORD_SYM = 292,
    KEYWORD_VALUE_SYM = 293,
    KEYWORD_BIND_SYM = 294,
    TOKEN_SYM = 295,
    INTEGER_SYM = 296,
    INTEGER32_SYM = 297,
    UNSIGNED32_SYM = 298,
    GAUGE32_SYM = 299,
    COUNTER_SYM = 300,
    COUNTER32_SYM = 301,
    COUNTER64_SYM = 302,
    BITS_SYM = 303,
    STRING_SYM = 304,
    OCTET_SYM = 305,
    SEQUENCE_SYM = 306,
    OF_SYM = 307,
    TIMETICKS_SYM = 308,
    IP_ADDRESS_SYM = 309,
    NETWORK_ADDRESS_SYM = 310,
    OPAQUE_SYM = 311,
    REVISION_SYM = 312,
    TEXTUAL_CONVENTION_SYM = 313,
    ACCESS_SYM = 314,
    MAX_ACCESS_SYM = 315,
    MIN_ACCESS_SYM = 316,
    SYNTAX_SYM = 317,
    STATUS_SYM = 318,
    INDEX_SYM = 319,
    REFERENCE_SYM = 320,
    DEFVAL_SYM = 321,
    LEFT_PAREN_SYM = 322,
    RIGHT_PAREN_SYM = 323,
    NOTIFICATIONS_SYM = 324,
    NOTIFICATION_GROUP_SYM = 325,
    NOTIFICATION_TYPE_SYM = 326,
    SIZE_SYM = 327,
    BAR_SYM = 328,
    VARIATION_SYM = 329,
    WRITE_SYNTAX_SYM = 330,
    SUPPORTS_SYM = 331,
    INCLUDES_SYM = 332,
    CREATION_REQUIRES_SYM = 333,
    PRODUCT_RELEASE_SYM = 334,
    CHOICE_SYM = 335,
    UNITS_SYM = 336,
    AUGMENTS_SYM = 337,
    OBJECT_SYM = 338,
    TAGS_SYM = 339,
    AUTOMATIC_SYM = 340,
    MIN_SYM = 341,
    MAX_SYM = 342,
    MODULE_SYM = 343,
    MACRO_SYM = 344,
    UCASEFIRST_IDENT_SYM = 345,
    LCASEFIRST_IDENT_SYM = 346,
    BSTRING_SYM = 347,
    HSTRING_SYM = 348,
    CSTRING_SYM = 349,
    DISPLAY_HINT_SYM = 350,
    NUMBER_SYM = 351
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 114 "parser.y" /* yacc.c:355  */

   int nInteger;
   char *pszString;
   MP_NUMERIC_VALUE number;
   Array *pList;
   ObjectArray<MP_SUBID> *pOID;
   ObjectArray<MP_IMPORT_MODULE> *pImportList;
   MP_IMPORT_MODULE *pImports;
   MP_OBJECT *pObject;
   MP_SUBID *pSubId;
   MP_SYNTAX *pSyntax;

#line 331 "parser.tab.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE mplval;

int mpparse (void);

#endif /* !YY_MP_PARSER_TAB_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 348 "parser.tab.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   482

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  97
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  99
/* YYNRULES -- Number of rules.  */
#define YYNRULES  217
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  426

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   351

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   256,   256,   257,   261,   262,   266,   270,   274,   278,
     282,   286,   290,   291,   295,   299,   300,   304,   308,   309,
     313,   321,   325,   332,   340,   351,   355,   369,   376,   380,
     388,   395,   402,   412,   413,   417,   418,   425,   426,   427,
     431,   438,   445,   452,   469,   496,   515,   523,   530,   531,
     535,   536,   540,   549,   556,   563,   570,   577,   582,   588,
     589,   590,   594,   595,   596,   602,   603,   604,   608,   615,
     622,   629,   636,   641,   648,   652,   656,   660,   664,   668,
     672,   686,   701,   716,   722,   728,   734,   743,   747,   751,
     755,   762,   771,   775,   779,   783,   787,   791,   795,   799,
     803,   807,   812,   816,   823,   828,   833,   838,   843,   848,
     853,   858,   863,   871,   875,   882,   891,   892,   896,   900,
     913,   931,   964,   965,   969,   976,   977,   981,   982,   986,
     993,   994,   998,   999,  1003,  1013,  1021,  1025,  1026,  1030,
    1031,  1035,  1054,  1072,  1086,  1087,  1091,  1097,  1101,  1105,
    1106,  1110,  1111,  1115,  1130,  1131,  1135,  1140,  1148,  1169,
    1174,  1179,  1185,  1191,  1214,  1218,  1222,  1226,  1230,  1234,
    1238,  1242,  1243,  1244,  1248,  1249,  1253,  1257,  1258,  1262,
    1266,  1270,  1277,  1284,  1291,  1295,  1303,  1313,  1317,  1325,
    1329,  1333,  1340,  1348,  1349,  1353,  1357,  1358,  1362,  1363,
    1367,  1374,  1375,  1376,  1377,  1378,  1379,  1380,  1384,  1385,
    1386,  1387,  1394,  1398,  1405,  1412,  1419,  1423
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ENTERPRISE_SYM", "TRAP_TYPE_SYM",
  "VARIABLES_SYM", "EXPLICIT_SYM", "IMPLICIT_SYM", "IMPLIED_SYM",
  "RIGHT_BRACE_SYM", "LEFT_BRACE_SYM", "RIGHT_BRACKET_SYM",
  "LEFT_BRACKET_SYM", "DEFINITIONS_SYM", "ASSIGNMENT_SYM", "BEGIN_SYM",
  "END_SYM", "FROM_SYM", "IMPORTS_SYM", "EXPORTS_SYM", "COMMA_SYM",
  "SEMI_COLON_SYM", "DOT_SYM", "DESCRIPTION_SYM", "ORGANIZATION_SYM",
  "CONTACT_SYM", "UPDATE_SYM", "MODULE_IDENTITY_SYM",
  "MODULE_COMPLIANCE_SYM", "OBJECT_IDENTIFIER_SYM", "OBJECT_TYPE_SYM",
  "OBJECT_GROUP_SYM", "OBJECT_IDENTITY_SYM", "OBJECTS_SYM",
  "MANDATORY_GROUPS_SYM", "GROUP_SYM", "AGENT_CAPABILITIES_SYM",
  "KEYWORD_SYM", "KEYWORD_VALUE_SYM", "KEYWORD_BIND_SYM", "TOKEN_SYM",
  "INTEGER_SYM", "INTEGER32_SYM", "UNSIGNED32_SYM", "GAUGE32_SYM",
  "COUNTER_SYM", "COUNTER32_SYM", "COUNTER64_SYM", "BITS_SYM",
  "STRING_SYM", "OCTET_SYM", "SEQUENCE_SYM", "OF_SYM", "TIMETICKS_SYM",
  "IP_ADDRESS_SYM", "NETWORK_ADDRESS_SYM", "OPAQUE_SYM", "REVISION_SYM",
  "TEXTUAL_CONVENTION_SYM", "ACCESS_SYM", "MAX_ACCESS_SYM",
  "MIN_ACCESS_SYM", "SYNTAX_SYM", "STATUS_SYM", "INDEX_SYM",
  "REFERENCE_SYM", "DEFVAL_SYM", "LEFT_PAREN_SYM", "RIGHT_PAREN_SYM",
  "NOTIFICATIONS_SYM", "NOTIFICATION_GROUP_SYM", "NOTIFICATION_TYPE_SYM",
  "SIZE_SYM", "BAR_SYM", "VARIATION_SYM", "WRITE_SYNTAX_SYM",
  "SUPPORTS_SYM", "INCLUDES_SYM", "CREATION_REQUIRES_SYM",
  "PRODUCT_RELEASE_SYM", "CHOICE_SYM", "UNITS_SYM", "AUGMENTS_SYM",
  "OBJECT_SYM", "TAGS_SYM", "AUTOMATIC_SYM", "MIN_SYM", "MAX_SYM",
  "MODULE_SYM", "MACRO_SYM", "UCASEFIRST_IDENT_SYM",
  "LCASEFIRST_IDENT_SYM", "BSTRING_SYM", "HSTRING_SYM", "CSTRING_SYM",
  "DISPLAY_HINT_SYM", "NUMBER_SYM", "$accept", "ModuleDefinition",
  "AssignmentList", "Assignment", "ModuleIdentifierAssignment",
  "ModuleIdentifier", "ObjectIdentifierAssignment", "AssignedIdentifier",
  "AssignedIdentifierList", "ObjectIdentifierList", "ObjectIdentifier",
  "NumericValue", "NumberOrMinMax", "DefinedValue", "Number",
  "ObjectIdentityAssignment", "ObjectTypeAssignment",
  "ModuleIdentityAssignment", "ImportsAssignment", "ExportsAssignment",
  "SnmpRevisionPart", "SnmpRevisionList", "SnmpRevisionObject",
  "SnmpIdentityPart", "SnmpOrganisationPart", "SnmpContactInfoPart",
  "SnmpUpdatePart", "SnmpDescriptionPart", "ValueConstraint",
  "NumericValueConstraintList", "SnmpKeywordAssignment", "SnmpKeywordName",
  "SnmpKeywordValue", "SnmpKeywordBinding", "SnmpSyntax", "SnmpSyntaxPart",
  "SnmpUnitsPart", "SnmpWriteSyntaxPart", "SnmpCreationPart",
  "TypeOrValueAssignment", "Type", "NamedType", "BuiltInType",
  "BuiltInTypeAssignment", "TypeOrTextualConvention", "MacroAssignment",
  "TokenList", "TokenObject", "TextualConventionAssignment",
  "SnmpNotificationTypeAssignment", "SnmpTrapVariablePart",
  "SnmpTrapVariableList", "SnmpMandatoryGroupPart",
  "SnmpMandatoryGroupList", "SnmpMandatoryGroup", "SnmpCompliancePart",
  "SnmpComplianceList", "SnmpComplianceObject", "SnmpObjectsPart",
  "SnmpObjectGroupAssignment", "SnmpNotificationGroupAssignment",
  "ModuleComplianceAssignment", "SnmpModuleComplianceList",
  "SnmpModuleComplianceObject", "OptionalModuleName",
  "SnmpVariationsListPart", "SnmpVariationsList", "SnmpVariationPart",
  "ModuleCapabilitiesList", "ModuleCapabilitiesAssignment",
  "AgentCapabilitiesAssignment", "SnmpAccessPart", "SnmpStatusPart",
  "SnmpReferencePart", "SnmpDisplayHintPart", "SnmpIndexPart",
  "SnmpDefValPart", "DefValList", "DefValListElement", "BinaryString",
  "HexString", "CharString", "SymbolsFromModuleList", "SymbolsFromModule",
  "SymbolList", "Symbol", "SequenceItem", "SequenceList",
  "SequenceAssignment", "SnmpTypeTagPart", "SnmpTypeTagList",
  "SnmpTypeTagItem", "OctetStringType", "Value", "Identifier",
  "UCidentifier", "LCidentifier", "End", "Begin", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351
};
# endif

#define YYPACT_NINF -286

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-286)))

#define YYTABLE_NINF -214

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -50,  -286,    47,   377,  -286,    36,  -286,  -286,    55,    55,
      41,    78,    87,    93,   143,   145,   172,   180,   185,   191,
     196,  -286,   377,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
     153,  -286,   114,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,    37,   198,   411,  -286,   202,    80,    23,  -286,   173,
    -286,  -286,  -286,   171,   141,     8,     8,     8,     8,     8,
       8,     8,     8,     8,   206,  -286,  -286,   141,   208,   223,
     227,  -286,   204,   181,   210,   241,     9,  -286,   182,   233,
       6,   182,   175,     6,     6,  -286,     3,  -286,  -286,  -286,
     -50,    55,  -286,  -286,  -286,   -50,   203,   161,  -286,    19,
      19,  -286,  -286,   344,   248,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,   224,  -286,   141,   240,   141,
     259,  -286,    18,  -286,   189,    28,   261,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,    80,    82,   261,
    -286,  -286,    80,   250,    16,  -286,   265,   270,   182,   250,
     141,   182,   148,   201,  -286,  -286,  -286,   186,   248,   141,
     182,   176,   162,  -286,  -286,   237,     7,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,   239,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,    12,  -286,  -286,
     141,   250,  -286,    55,  -286,   141,   148,   137,  -286,  -286,
     284,     4,  -286,  -286,  -286,   229,   269,   230,  -286,  -286,
     141,   234,    27,    53,   250,   234,   182,   250,    80,    80,
      80,   182,   206,   287,  -286,   137,  -286,   250,  -286,  -286,
      26,   278,  -286,  -286,   296,   242,   252,   253,   258,  -286,
     271,  -286,  -286,  -286,  -286,   251,    45,  -286,   182,  -286,
      88,    18,   312,   250,  -286,  -286,  -286,   186,    80,  -286,
     141,   235,  -286,   107,  -286,   131,   234,   233,   250,   234,
    -286,  -286,  -286,   250,  -286,  -286,   234,   176,   176,   302,
    -286,   127,   176,  -286,  -286,  -286,   141,   233,   251,  -286,
    -286,   250,  -286,   137,  -286,    55,   234,   263,  -286,  -286,
     -50,     2,  -286,  -286,  -286,   233,  -286,   234,   233,   234,
     181,  -286,  -286,   127,   264,   266,   250,  -286,  -286,   234,
    -286,   138,   233,  -286,   292,  -286,  -286,  -286,  -286,   -39,
    -286,   233,  -286,  -286,  -286,   267,  -286,   -32,  -286,  -286,
     317,   -11,   292,  -286,   -50,   256,    44,  -286,  -286,  -286,
     319,   326,   273,    55,    80,    80,  -286,   -11,  -286,  -286,
     272,    55,  -286,  -286,    55,   137,   330,   233,   147,   250,
     181,  -286,   331,    13,   157,   333,   269,  -286,    32,  -286,
    -286,  -286,  -286,   268,    55,    80,  -286,   274,  -286,  -286,
    -286,    74,   337,   336,   348,   349,   350,    18,   148,   160,
     181,  -286,  -286,   165,  -286,   351,  -286,  -286,  -286,  -286,
     250,   274,   268,  -286,  -286,  -286,  -286,   148,   282,   352,
     273,    55,   250,   167,  -286,  -286
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   214,     0,     0,    20,     0,     1,   216,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   215,     0,     5,     6,     7,     8,    17,    18,    19,
       0,    15,    65,    10,    80,     9,    11,    14,    13,    12,
      16,     0,   212,   213,     3,     0,     0,     0,   185,     0,
     188,   189,   190,     0,     0,   197,   197,   197,   197,   197,
     197,   197,   197,   197,     0,     4,     2,     0,     0,     0,
       0,    66,     0,    73,   197,     0,     0,    53,     0,     0,
     140,     0,     0,   140,   140,    24,     0,   191,    46,   184,
       0,     0,    47,   183,    68,     0,     0,   167,   113,    88,
      87,   106,   114,     0,   196,   199,   107,   112,   109,   110,
     111,   108,   104,   105,   217,     0,    56,     0,     0,     0,
       0,    67,   197,    72,    75,     0,     0,   181,   182,    42,
     208,    82,    81,   210,   209,   211,    86,     0,     0,     0,
      25,    26,     0,    58,     0,    23,     0,     0,     0,    58,
       0,     0,   162,     0,    21,   186,   187,     0,   197,     0,
       0,     0,     0,    90,    89,     0,     0,   102,    92,    94,
      95,    99,    96,    97,    98,   207,     0,   100,   204,   205,
     206,   103,    91,   212,   213,   198,   118,     0,   117,    54,
       0,    58,    69,     0,    71,     0,   162,     0,    83,    85,
     123,     0,    29,    31,    30,     0,   212,    41,    84,   163,
       0,   165,     0,     0,    58,   165,     0,    58,     0,     0,
       0,     0,     0,     0,   101,     0,   166,    58,    38,    39,
      64,    33,    34,    37,     0,     0,     0,     0,     0,    93,
       0,   201,   116,   115,    55,    49,     0,    74,     0,   193,
       0,   197,     0,    58,   122,    27,    28,     0,     0,    57,
       0,     0,   137,     0,   139,     0,   165,     0,    58,   165,
     159,   160,   161,    58,    22,   200,   165,     0,     0,     0,
      61,     0,     0,    59,   202,   203,     0,     0,    48,    51,
      70,    58,   195,     0,   192,     0,   165,     0,    40,   164,
     148,     0,   145,   136,   138,     0,    43,   165,     0,   165,
      73,    63,    62,     0,     0,     0,    58,    45,    50,   165,
     194,     0,     0,    32,   126,   147,   143,   144,   141,     0,
     142,     0,   119,    35,    36,     0,    52,   170,   124,   121,
       0,   131,   125,   128,     0,     0,     0,   155,   120,    60,
       0,     0,   176,     0,     0,     0,   146,   130,   133,   127,
       0,     0,   158,   154,     0,     0,     0,     0,     0,    58,
      73,   132,     0,   150,     0,     0,     0,    41,     0,   175,
      44,   129,   135,    77,     0,     0,   157,   149,   152,   168,
     169,     0,     0,   178,     0,     0,     0,   197,   162,     0,
      73,   151,   180,     0,   174,     0,   171,   172,   173,    76,
      58,   150,    77,   179,   177,   134,   156,   162,    79,     0,
     176,     0,    58,     0,   153,    78
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -286,  -286,  -286,   341,  -286,   365,  -286,   -72,     5,  -286,
     166,  -286,  -240,  -127,   -68,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,    81,  -286,  -286,  -286,  -286,   -85,   276,  -147,
    -286,  -286,   298,  -286,  -286,  -285,  -286,   -44,  -286,  -286,
     -69,   214,  -286,  -286,   255,  -286,  -286,   187,   306,  -286,
    -286,  -286,  -286,  -286,    39,  -286,  -286,    46,   156,  -286,
    -286,  -286,  -286,   100,  -286,   -28,  -286,    15,  -286,    58,
    -286,  -187,   -67,  -198,  -286,  -286,   -15,    11,  -286,    29,
      30,   -40,  -286,   359,    -9,   318,   117,  -286,   -83,   260,
    -286,   307,  -286,  -286,     0,     1,    -1,   -12,   -74
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    22,    23,     3,   345,    24,    85,   140,   201,
     202,   230,   231,   232,   233,    25,    26,    27,    28,    29,
     287,   288,   289,    30,   118,   191,    68,   211,   163,   234,
      31,    32,    71,    72,   123,   124,   196,   398,   420,    33,
      98,    99,   100,    34,   101,    35,   187,   188,   102,    36,
     253,   254,   341,   342,   343,   356,   357,   358,   148,    37,
      38,    39,   301,   302,   324,   386,   387,   388,   346,   347,
      40,   221,   143,   261,   160,   352,   367,   392,   393,   133,
     134,    94,    47,    48,    49,    50,   249,   250,   198,   103,
     104,   105,   181,   136,   235,    51,    52,    44,   115
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      53,     5,    43,    41,    42,   131,   130,   145,   141,   248,
      66,   203,   154,   255,   149,   237,   144,   267,   114,   138,
      95,    43,    41,    42,   354,   332,   138,   116,     7,   161,
      95,    46,   350,    91,   135,    46,   262,   344,   197,   146,
       1,   314,   391,   199,    88,    87,   277,     6,   239,    45,
     351,     1,   186,   194,   290,    54,   208,   240,   144,    96,
     139,    46,   264,    46,   215,    91,    97,    73,   305,    96,
     204,   308,   355,   333,   203,   147,   141,   189,   310,   192,
     158,   214,    46,   402,   217,   383,   162,   385,   153,   223,
     300,   155,    55,   227,     1,    21,   157,   292,   322,   278,
     129,    56,   184,   182,   183,   129,   245,    57,   293,   329,
     216,   331,   129,     1,    21,   412,   303,     1,    21,   226,
     344,   337,     1,    21,   127,   128,    93,    91,   129,   266,
     311,   312,   269,   204,     1,   315,   200,   207,   205,   206,
     304,   209,   276,     1,    21,     1,    21,   338,   274,   268,
     244,    91,    69,    70,   273,   247,   381,    58,    91,    59,
     207,   207,   206,   206,     1,    21,   389,    91,   296,   411,
     259,    21,     1,    21,   413,   243,   425,    91,   129,    67,
      91,   291,   294,   307,   246,    91,    60,    91,   309,   297,
      90,    91,    92,    91,    61,   306,   184,   251,   183,    62,
     207,   205,   206,   263,   265,    63,   319,   218,   219,   220,
      64,   410,    74,   228,   229,   317,    86,   270,   271,   272,
     299,   114,    95,   129,   184,   182,   183,     1,    21,   326,
     418,   336,   117,   328,   236,    93,   330,   119,   375,   151,
     152,   120,    69,   122,   137,   142,   316,   144,   228,   229,
     339,   203,     1,    21,   150,   158,   159,   298,   129,   348,
      95,   125,   228,   229,   186,   190,     1,    21,    97,   193,
     195,   197,   129,   210,   362,   212,   207,   207,   206,   206,
     213,   207,   129,   206,   382,   222,   321,   238,   241,   252,
     126,   258,   184,   251,   183,   380,   257,  -213,   275,   260,
     279,   325,   127,   128,    93,   280,   129,   284,   286,   281,
     204,   106,   107,   108,   109,   110,   111,   112,   113,   282,
     285,   283,   295,   300,   313,   415,   340,   353,   409,   364,
       5,   323,   334,   361,   335,   349,   365,   424,   396,   366,
     378,   384,   390,   397,   368,   360,   404,     5,   385,   372,
     165,   166,   373,   369,   370,   374,   405,   406,   407,   408,
     419,   391,   421,    65,   377,     4,   376,   256,   417,   318,
     121,   379,   224,   167,   242,   399,   164,   207,   205,   206,
     132,   359,   403,   416,   400,   168,   169,   170,   171,   172,
     173,   174,   175,     7,   176,     8,     9,   177,   178,   179,
     180,   327,   401,   371,   363,   422,    89,   394,   395,   156,
     320,   185,   423,     0,    10,    75,   414,     0,   225,    11,
      12,    13,    14,    15,    16,    76,     0,     0,     0,     0,
      17,    18,     0,    19,     1,    21,     0,     0,    77,    78,
      79,     0,    80,    81,     0,     0,     0,    82,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    20,     1,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    83,    84
};

static const yytype_int16 yycheck[] =
{
       9,     0,     3,     3,     3,    74,    74,    79,    76,   196,
      22,   138,    86,     9,    81,   162,    14,   215,    15,    10,
      12,    22,    22,    22,    35,   310,    10,    67,    16,    10,
      12,     8,    64,    20,    74,     8,     9,    76,    10,    33,
      90,   281,    10,   126,    21,    46,    20,     0,    41,    13,
      82,    90,    40,   122,     9,    14,   139,    50,    14,    51,
      51,     8,     9,     8,   149,    20,    58,    30,   266,    51,
     138,   269,    83,   313,   201,    69,   144,   117,   276,   119,
      52,   148,     8,     9,   151,   370,    67,    74,    85,   157,
      88,    90,    14,   160,    90,    91,    95,     9,   296,    73,
      96,    14,   103,   103,   103,    96,   191,    14,    20,   307,
     150,   309,    96,    90,    91,   400,     9,    90,    91,   159,
      76,   319,    90,    91,    92,    93,    94,    20,    96,   214,
     277,   278,   217,   201,    90,   282,   137,   138,   138,   138,
       9,   142,   227,    90,    91,    90,    91,     9,   222,   216,
     190,    20,    38,    39,   221,   195,     9,    14,    20,    14,
     161,   162,   161,   162,    90,    91,     9,    20,   253,     9,
     210,    91,    90,    91,     9,   187,     9,    20,    96,    26,
      20,   248,   251,   268,   193,    20,    14,    20,   273,   257,
      17,    20,    21,    20,    14,   267,   197,   197,   197,    14,
     201,   201,   201,   212,   213,    14,   291,    59,    60,    61,
      14,   398,    14,    86,    87,   287,    14,   218,   219,   220,
     260,    15,    12,    96,   225,   225,   225,    90,    91,   301,
     417,   316,    24,   305,    72,    94,   308,    14,   365,    83,
      84,    14,    38,    62,     3,    63,   286,    14,    86,    87,
     322,   378,    90,    91,    79,    52,    95,   258,    96,   331,
      12,    51,    86,    87,    40,    25,    90,    91,    58,    10,
      81,    10,    96,    23,   346,    10,   277,   278,   277,   278,
      10,   282,    96,   282,   369,    84,   295,    50,    49,     5,
      80,    22,   293,   293,   293,   367,    67,    67,    11,    65,
      22,   300,    92,    93,    94,     9,    96,    49,    57,    67,
     378,    56,    57,    58,    59,    60,    61,    62,    63,    67,
      49,    68,    10,    88,    22,   410,    34,    10,   397,    10,
     329,    68,    68,    77,    68,    68,    10,   422,   378,    66,
      10,    10,     9,    75,   353,   344,     9,   346,    74,    77,
       6,     7,   361,   354,   355,   364,    20,     9,     9,     9,
      78,    10,    10,    22,   365,     0,   365,   201,   412,   288,
      72,   366,   158,    29,   187,   384,   100,   378,   378,   378,
      74,   342,   391,   411,   385,    41,    42,    43,    44,    45,
      46,    47,    48,    16,    50,    18,    19,    53,    54,    55,
      56,   301,   387,   357,   346,   420,    47,   378,   378,    91,
     293,   104,   421,    -1,    37,     4,   405,    -1,   158,    42,
      43,    44,    45,    46,    47,    14,    -1,    -1,    -1,    -1,
      53,    54,    -1,    56,    90,    91,    -1,    -1,    27,    28,
      29,    -1,    31,    32,    -1,    -1,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    90,    98,   101,   102,   192,     0,    16,    18,    19,
      37,    42,    43,    44,    45,    46,    47,    53,    54,    56,
      89,    91,    99,   100,   103,   112,   113,   114,   115,   116,
     120,   127,   128,   136,   140,   142,   146,   156,   157,   158,
     167,   191,   192,   193,   194,    13,     8,   179,   180,   181,
     182,   192,   193,   181,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,   100,   194,    26,   123,    38,
      39,   129,   130,    30,    14,     4,    14,    27,    28,    29,
      31,    32,    36,    70,    71,   104,    14,   193,    21,   180,
      17,    20,    21,    94,   178,    12,    51,    58,   137,   138,
     139,   141,   145,   186,   187,   188,   141,   141,   141,   141,
     141,   141,   141,   141,    15,   195,   178,    24,   121,    14,
      14,   129,    62,   131,   132,    51,    80,    92,    93,    96,
     111,   137,   145,   176,   177,   178,   190,     3,    10,    51,
     105,   111,    63,   169,    14,   104,    33,    69,   155,   169,
      79,   155,   155,    85,   195,   192,   182,   192,    52,    95,
     171,    10,    67,   125,   125,     6,     7,    29,    41,    42,
      43,    44,    45,    46,    47,    48,    50,    53,    54,    55,
      56,   189,   191,   192,   193,   188,    40,   143,   144,   178,
      25,   122,   178,    10,   137,    81,   133,    10,   185,   185,
     193,   106,   107,   110,   111,   191,   192,   193,   185,   193,
      23,   124,    10,    10,   169,   124,   178,   169,    59,    60,
      61,   168,    84,   111,   138,   186,   178,   169,    86,    87,
     108,   109,   110,   111,   126,   191,    72,   126,    50,    41,
      50,    49,   144,   194,   178,   124,   181,   178,   168,   183,
     184,   191,     5,   147,   148,     9,   107,    67,    22,   178,
      65,   170,     9,   181,     9,   181,   124,   170,   169,   124,
     193,   193,   193,   169,   195,    11,   124,    20,    73,    22,
       9,    67,    67,    68,    49,    49,    57,   117,   118,   119,
       9,   169,     9,    20,   137,    10,   124,   111,   193,   178,
      88,   159,   160,     9,     9,   170,   104,   124,   170,   124,
     170,   126,   126,    22,   109,   126,   178,   104,   119,   124,
     183,   181,   170,    68,   161,   192,   104,   160,   104,   170,
     104,   170,   132,   109,    68,    68,   124,   170,     9,   104,
      34,   149,   150,   151,    76,   102,   165,   166,   104,    68,
      64,    82,   172,    10,    35,    83,   152,   153,   154,   151,
     192,    77,   104,   166,    10,    10,    66,   173,   181,   193,
     193,   154,    77,   181,   181,   110,   192,   193,    10,   105,
     104,     9,   124,   132,    10,    74,   162,   163,   164,     9,
       9,    10,   174,   175,   176,   177,   178,    75,   134,   181,
     193,   164,     9,   181,     9,    20,     9,     9,     9,   137,
     168,     9,   132,     9,   174,   124,   162,   134,   168,    78,
     135,    10,   173,   181,   124,     9
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    97,    98,    98,    99,    99,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     101,   102,   102,   103,   103,   104,   104,   105,   106,   106,
     107,   107,   107,   108,   108,   108,   108,   109,   109,   109,
     110,   110,   111,   112,   113,   114,   115,   116,   117,   117,
     118,   118,   119,   120,   121,   122,   123,   124,   124,   125,
     125,   125,   126,   126,   126,   127,   127,   127,   128,   129,
     130,   131,   132,   132,   133,   133,   134,   134,   135,   135,
     136,   136,   136,   136,   136,   136,   136,   137,   137,   137,
     137,   138,   139,   139,   139,   139,   139,   139,   139,   139,
     139,   139,   139,   139,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   141,   141,   142,   143,   143,   144,   145,
     146,   146,   147,   147,   148,   149,   149,   150,   150,   151,
     152,   152,   153,   153,   154,   154,   155,   155,   155,   155,
     155,   156,   157,   158,   159,   159,   160,   161,   161,   162,
     162,   163,   163,   164,   165,   165,   166,   166,   167,   168,
     168,   168,   168,   169,   170,   170,   171,   171,   172,   172,
     172,   173,   173,   173,   173,   173,   173,   174,   174,   175,
     175,   176,   177,   178,   179,   179,   180,   181,   181,   182,
     182,   182,   183,   184,   184,   185,   186,   186,   187,   187,
     188,   189,   189,   189,   189,   189,   189,   189,   190,   190,
     190,   190,   191,   191,   192,   193,   194,   195
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     6,     3,     2,     2,     2,     3,     2,     1,
       1,     1,     4,     1,     1,     4,     4,     1,     1,     1,
       3,     1,     1,     6,    11,     7,     3,     3,     1,     0,
       2,     1,     3,     2,     2,     2,     2,     2,     0,     3,
       6,     3,     3,     3,     1,     1,     2,     3,     3,     3,
       5,     2,     1,     0,     2,     0,     2,     0,     4,     0,
       1,     3,     3,     4,     4,     4,     3,     1,     1,     2,
       2,     2,     2,     3,     2,     2,     2,     2,     2,     2,
       2,     3,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     5,     2,     1,     1,     6,
       8,     8,     1,     0,     4,     1,     0,     2,     1,     4,
       1,     0,     2,     1,     6,     3,     4,     3,     4,     3,
       0,     7,     7,     7,     2,     1,     4,     1,     0,     1,
       0,     2,     1,     8,     2,     1,     7,     4,     9,     2,
       2,     2,     0,     2,     2,     0,     2,     0,     4,     4,
       0,     4,     4,     4,     4,     2,     0,     3,     1,     3,
       2,     1,     1,     1,     2,     1,     3,     3,     1,     1,
       1,     2,     2,     1,     3,     3,     1,     0,     2,     1,
       4,     2,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 267 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1793 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 271 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1801 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 275 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1809 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 279 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1817 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 283 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1825 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 287 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1833 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 292 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1841 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 296 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1849 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 301 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1857 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 305 "parser.y" /* yacc.c:1646  */
    {
   m_pModule->pObjectList->add((yyvsp[0].pObject));
}
#line 1865 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 314 "parser.y" /* yacc.c:1646  */
    {
   m_pModule = new MP_MODULE;
   m_pModule->pszName = (yyvsp[0].pszString);
}
#line 1874 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 322 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[-3].pszString);
}
#line 1882 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 326 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[-5].pszString);
}
#line 1890 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 333 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-2].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 1902 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 341 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-1].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 1914 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 352 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pOID) = (yyvsp[0].pOID);
}
#line 1922 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 356 "parser.y" /* yacc.c:1646  */
    {
   MP_SUBID *subid;

   subid = new MP_SUBID;
   subid->dwValue = (yyvsp[0].number).value.nInt32;
   subid->pszName = NULL;
   subid->bResolved = TRUE;
   (yyval.pOID) = new ObjectArray<MP_SUBID>(16, 16, true);
   (yyval.pOID)->add(subid);
}
#line 1937 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 370 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pOID) = (yyvsp[-1].pOID);
}
#line 1945 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 377 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pOID)->add((yyvsp[0].pSubId));
}
#line 1953 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 381 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pOID) = new ObjectArray<MP_SUBID>(16, 16, true);
   (yyval.pOID)->add((yyvsp[0].pSubId));
}
#line 1962 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 389 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSubId) = new MP_SUBID;
   (yyval.pSubId)->dwValue = (yyvsp[0].number).value.nInt32;
   (yyval.pSubId)->pszName = NULL;
   (yyval.pSubId)->bResolved = TRUE;
}
#line 1973 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 396 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSubId) = new MP_SUBID;
   (yyval.pSubId)->dwValue = 0;
   (yyval.pSubId)->pszName = (yyvsp[0].pszString);
   (yyval.pSubId)->bResolved = FALSE;
}
#line 1984 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 403 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSubId) = new MP_SUBID;
   (yyval.pSubId)->dwValue = (yyvsp[-1].number).value.nInt32;
   (yyval.pSubId)->pszName = (yyvsp[-3].pszString);
   (yyval.pSubId)->bResolved = TRUE;
}
#line 1995 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 414 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2003 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 419 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-3].pszString));
}
#line 2011 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 432 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (char *)malloc(strlen((yyvsp[-2].pszString)) + strlen((yyvsp[0].pszString)) + 2);
   sprintf((yyval.pszString), "%s.%s", (yyvsp[-2].pszString), (yyvsp[0].pszString));
   free((yyvsp[-2].pszString));
   free((yyvsp[0].pszString));
}
#line 2022 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 439 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 2030 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 446 "parser.y" /* yacc.c:1646  */
    {
   (yyval.number) = (yyvsp[0].number);
}
#line 2038 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 457 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-5].pszString);
   (yyval.pObject)->iStatus = (yyvsp[-3].nInteger);
   (yyval.pObject)->pszDescription = (yyvsp[-2].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 2052 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 479 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-10].pszString);
   (yyval.pObject)->iSyntax = (yyvsp[-8].pSyntax)->nSyntax;
   (yyval.pObject)->pszDataType = (yyvsp[-8].pSyntax)->pszStr;
   (yyvsp[-8].pSyntax)->pszStr = NULL;
   delete (yyvsp[-8].pSyntax);
   (yyval.pObject)->iAccess = (yyvsp[-6].nInteger);
   (yyval.pObject)->iStatus = (yyvsp[-5].nInteger);
   (yyval.pObject)->pszDescription = (yyvsp[-4].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 2071 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 503 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->iSyntax = MIB_TYPE_MODID;
   (yyval.pObject)->pszName = (yyvsp[-6].pszString);
   (yyval.pObject)->pszDescription = (yyvsp[-2].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 2085 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 516 "parser.y" /* yacc.c:1646  */
    {
	delete m_pModule->pImportList;
   m_pModule->pImportList = (yyvsp[-1].pImportList);
}
#line 2094 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 524 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2102 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 542 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-1].pszString));
   safe_free((yyvsp[0].pszString));
}
#line 2111 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 550 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[-1].pszString);
}
#line 2119 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 557 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2127 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 564 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2135 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 571 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2143 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 578 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 2151 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 582 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = NULL;
}
#line 2159 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 597 "parser.y" /* yacc.c:1646  */
    {
}
#line 2166 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 609 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2174 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 616 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2182 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 623 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2190 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 630 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = (yyvsp[0].pSyntax);
}
#line 2198 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 637 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = (yyvsp[0].pSyntax);
}
#line 2206 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 641 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = new MP_SYNTAX;
   (yyval.pSyntax)->nSyntax = MIB_TYPE_OTHER;
}
#line 2215 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 649 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2223 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 657 "parser.y" /* yacc.c:1646  */
    {
	delete (yyvsp[0].pSyntax);
}
#line 2231 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 665 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2239 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 673 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_TYPEDEF;
   if ((yyvsp[0].pSyntax) != NULL)
   {
      (yyval.pObject)->pszName = (yyvsp[0].pSyntax)->pszStr;
      (yyval.pObject)->iSyntax = (yyvsp[0].pSyntax)->nSyntax;
      (yyval.pObject)->pszDescription = (yyvsp[0].pSyntax)->pszDescription;
      (yyvsp[0].pSyntax)->pszStr = NULL;
      (yyvsp[0].pSyntax)->pszDescription = NULL;
      delete (yyvsp[0].pSyntax);
   }
}
#line 2257 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 687 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_TEXTUAL_CONVENTION;
   (yyval.pObject)->pszName = (yyvsp[-2].pszString);
   if ((yyvsp[0].pSyntax) != NULL)
   {
      (yyval.pObject)->iSyntax = (yyvsp[0].pSyntax)->nSyntax;
      (yyval.pObject)->pszDataType = (yyvsp[0].pSyntax)->pszStr;
      (yyval.pObject)->pszDescription = (yyvsp[0].pSyntax)->pszDescription;
      (yyvsp[0].pSyntax)->pszStr = NULL;
      (yyvsp[0].pSyntax)->pszDescription = NULL;
      delete (yyvsp[0].pSyntax);
   }
}
#line 2276 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 702 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_TYPEDEF;
   (yyval.pObject)->pszName = (yyvsp[-2].pszString);
   if ((yyvsp[0].pSyntax) != NULL)
   {
      (yyval.pObject)->iSyntax = (yyvsp[0].pSyntax)->nSyntax;
      (yyval.pObject)->pszDataType = (yyvsp[0].pSyntax)->pszStr;
      (yyval.pObject)->pszDescription = (yyvsp[0].pSyntax)->pszDescription;
      (yyvsp[0].pSyntax)->pszStr = NULL;
      (yyvsp[0].pSyntax)->pszDescription = NULL;
      delete (yyvsp[0].pSyntax);
   }
}
#line 2295 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 717 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_SEQUENCE;
   (yyval.pObject)->pszName = (yyvsp[-3].pszString);
}
#line 2305 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 723 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_SEQUENCE;
   (yyval.pObject)->pszName = (yyvsp[-3].pszString);
}
#line 2315 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 729 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_CHOICE;
   (yyval.pObject)->pszName = (yyvsp[-3].pszString);
}
#line 2325 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 735 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_VALUE;
   (yyval.pObject)->pszName = (yyvsp[-2].pszString);
}
#line 2335 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 744 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = (yyvsp[0].pSyntax);
}
#line 2343 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 748 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = (yyvsp[0].pSyntax);
}
#line 2351 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 752 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = (yyvsp[-1].pSyntax);
}
#line 2359 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 756 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = (yyvsp[-1].pSyntax);
}
#line 2367 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 763 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = new MP_SYNTAX;
   (yyval.pSyntax)->nSyntax = -1;
   (yyval.pSyntax)->pszStr = (yyvsp[0].pszString);
}
#line 2377 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 772 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_INTEGER);
}
#line 2385 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 776 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_INTEGER);
}
#line 2393 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 780 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_INTEGER32);
}
#line 2401 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 784 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_UNSIGNED32);
}
#line 2409 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 788 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_COUNTER);
}
#line 2417 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 792 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_COUNTER32);
}
#line 2425 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 796 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_COUNTER64);
}
#line 2433 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 800 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_GAUGE32);
}
#line 2441 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 804 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_TIMETICKS);
}
#line 2449 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 808 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_SEQUENCE);
   delete (yyvsp[0].pSyntax);
}
#line 2458 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 813 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_OBJID);
}
#line 2466 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 817 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_OCTETSTR);
}
#line 2474 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 824 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_IPADDR);
   (yyval.pSyntax)->pszStr = strdup("IpAddress");
}
#line 2483 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 829 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_OPAQUE);
   (yyval.pSyntax)->pszStr = strdup("Opaque");
}
#line 2492 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 834 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_INTEGER32);
   (yyval.pSyntax)->pszStr = strdup("Integer32");
}
#line 2501 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 839 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_UNSIGNED32);
   (yyval.pSyntax)->pszStr = strdup("Unsigned32");
}
#line 2510 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 844 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_TIMETICKS);
   (yyval.pSyntax)->pszStr = strdup("TimeTicks");
}
#line 2519 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 849 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_COUNTER);
   (yyval.pSyntax)->pszStr = strdup("Counter");
}
#line 2528 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 854 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_COUNTER32);
   (yyval.pSyntax)->pszStr = strdup("Counter32");
}
#line 2537 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 859 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_COUNTER64);
   (yyval.pSyntax)->pszStr = strdup("Counter64");
}
#line 2546 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 864 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = create_std_syntax(MIB_TYPE_GAUGE32);
   (yyval.pSyntax)->pszStr = strdup("Gauge32");
}
#line 2555 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 872 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[0].pSyntax);
}
#line 2563 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 876 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[0].pSyntax);
}
#line 2571 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 883 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_MACRO;
   (yyval.pObject)->pszName = (yyvsp[-4].pszString);
}
#line 2581 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 906 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pSyntax) = (yyvsp[0].pSyntax);
   (yyval.pSyntax)->pszDescription = (yyvsp[-2].pszString);
}
#line 2590 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 920 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-7].pszString);
   (yyval.pObject)->iSyntax = MIB_TYPE_NOTIFTYPE;
   (yyval.pObject)->iAccess = (yyvsp[-4].nInteger);
   (yyval.pObject)->iStatus = (yyvsp[-3].nInteger);
   (yyval.pObject)->pszDescription = (yyvsp[-2].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 2606 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 121:
#line 937 "parser.y" /* yacc.c:1646  */
    {
   MP_SUBID *pSubId;

   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-7].pszString);
   (yyval.pObject)->iSyntax = MIB_TYPE_TRAPTYPE;
   (yyval.pObject)->pszDescription = (yyvsp[-2].pszString);

   pSubId = new MP_SUBID;
   pSubId->pszName = (yyvsp[-4].pszString);
   (yyval.pObject)->pOID->add(pSubId);

   pSubId = new MP_SUBID;
   pSubId->pszName = (char *)malloc(strlen((yyvsp[-4].pszString)) + 3);
   sprintf(pSubId->pszName, "%s#0", (yyvsp[-4].pszString));
   pSubId->bResolved = TRUE;
   (yyval.pObject)->pOID->add(pSubId);

   for(int i = 0; i < (yyvsp[0].pOID)->size(); i++)
      (yyval.pObject)->pOID->add((yyvsp[0].pOID)->get(i));
   (yyvsp[0].pOID)->setOwner(false);
   delete (yyvsp[0].pOID);
}
#line 2635 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 124:
#line 970 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2643 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 987 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2651 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 1008 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-4].pszString));
   delete (yyvsp[-3].pSyntax);
   safe_free((yyvsp[0].pszString));
}
#line 2661 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 1014 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-1].pszString));
   safe_free((yyvsp[0].pszString));
}
#line 2670 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 136:
#line 1022 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2678 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 1027 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2686 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 1041 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-6].pszString);
   (yyval.pObject)->iStatus = (yyvsp[-3].nInteger);
   (yyval.pObject)->iSyntax = MIB_TYPE_OBJGROUP;
   (yyval.pObject)->pszDescription = (yyvsp[-2].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 2701 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 142:
#line 1060 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-6].pszString);
   (yyval.pObject)->iStatus = (yyvsp[-3].nInteger);
   (yyval.pObject)->pszDescription = (yyvsp[-2].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
}
#line 2715 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 1078 "parser.y" /* yacc.c:1646  */
    {
   free((yyvsp[-6].pszString));
   safe_free((yyvsp[-3].pszString));
   delete (yyvsp[0].pOID);
}
#line 2725 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 1098 "parser.y" /* yacc.c:1646  */
    {
   free((yyvsp[0].pszString));
}
#line 2733 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 153:
#line 1122 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-6].pszString));
   delete (yyvsp[-5].pSyntax);
   safe_free((yyvsp[0].pszString));
}
#line 2743 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 156:
#line 1136 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-5].pszString));
   delete (yyvsp[-2].pList);
}
#line 2752 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 157:
#line 1141 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-3].pszString));
   delete (yyvsp[-1].pList);
}
#line 2761 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 158:
#line 1155 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pObject) = new MP_OBJECT;
   (yyval.pObject)->iType = MIBC_OBJECT;
   (yyval.pObject)->pszName = (yyvsp[-8].pszString);
   (yyval.pObject)->iStatus = (yyvsp[-4].nInteger);
   (yyval.pObject)->iSyntax = MIB_TYPE_AGENTCAP;
   (yyval.pObject)->pszDescription = (yyvsp[-3].pszString);
   delete (yyval.pObject)->pOID;
   (yyval.pObject)->pOID = (yyvsp[0].pOID);
   safe_free((yyvsp[-5].pszString));
}
#line 2777 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 159:
#line 1170 "parser.y" /* yacc.c:1646  */
    {
   (yyval.nInteger) = AccessFromText((yyvsp[0].pszString));
   free((yyvsp[0].pszString));
}
#line 2786 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 160:
#line 1175 "parser.y" /* yacc.c:1646  */
    {
   (yyval.nInteger) = AccessFromText((yyvsp[0].pszString));
   free((yyvsp[0].pszString));
}
#line 2795 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 161:
#line 1180 "parser.y" /* yacc.c:1646  */
    {
   (yyval.nInteger) = AccessFromText((yyvsp[0].pszString));
   free((yyvsp[0].pszString));
}
#line 2804 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 162:
#line 1185 "parser.y" /* yacc.c:1646  */
    {
   (yyval.nInteger) = 0;
}
#line 2812 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 163:
#line 1192 "parser.y" /* yacc.c:1646  */
    {
   static const char *pStatusText[] = { "mandatory", "optional", "obsolete", "deprecated", "current", NULL };
   int i;

   for(i = 0; pStatusText[i] != NULL; i++)
      if (!stricmp(pStatusText[i], (yyvsp[0].pszString)))
      {
         (yyval.nInteger) = i + 1;
         break;
      }
   if (pStatusText[i] == NULL)
   {
      char szBuffer[256];

      sprintf(szBuffer, "Invalid STATUS value \"%s\"", (yyvsp[0].pszString));
      mperror(szBuffer);
   }
   free((yyvsp[0].pszString));
}
#line 2836 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 164:
#line 1215 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2844 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 166:
#line 1223 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2852 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 168:
#line 1231 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2860 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 169:
#line 1235 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-1].pszString));
}
#line 2868 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 173:
#line 1245 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-1].pszString));
}
#line 2876 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 175:
#line 1250 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[0].pOID);
}
#line 2884 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 179:
#line 1263 "parser.y" /* yacc.c:1646  */
    {
   delete (yyvsp[-1].pList);
}
#line 2892 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 181:
#line 1271 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2900 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 182:
#line 1278 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 2908 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 183:
#line 1285 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 2916 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 184:
#line 1292 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pImportList)->add((yyvsp[0].pImports));
}
#line 2924 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 185:
#line 1296 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pImportList) = new ObjectArray<MP_IMPORT_MODULE>(16, 16, true);
   (yyval.pImportList)->add((yyvsp[0].pImports));
}
#line 2933 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 186:
#line 1304 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pImports) = new MP_IMPORT_MODULE;
   (yyval.pImports)->pszName = (yyvsp[0].pszString);
   delete (yyval.pImports)->pSymbols;
   (yyval.pImports)->pSymbols = (yyvsp[-2].pList);
}
#line 2944 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 187:
#line 1314 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pList)->add((yyvsp[0].pszString));
}
#line 2952 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 188:
#line 1318 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pList) = new Array(16, 16, true);
   (yyval.pList)->add((yyvsp[0].pszString));
}
#line 2961 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 189:
#line 1326 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 2969 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 190:
#line 1330 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 2977 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 191:
#line 1334 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 2985 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 192:
#line 1341 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-1].pszString));
   delete (yyvsp[0].pSyntax);
}
#line 2994 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 200:
#line 1368 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[-2].pszString));
}
#line 3002 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 211:
#line 1388 "parser.y" /* yacc.c:1646  */
    {
   safe_free((yyvsp[0].pszString));
}
#line 3010 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 212:
#line 1395 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 3018 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 213:
#line 1399 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 3026 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 214:
#line 1406 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 3034 "parser.tab.cpp" /* yacc.c:1646  */
    break;

  case 215:
#line 1413 "parser.y" /* yacc.c:1646  */
    {
   (yyval.pszString) = (yyvsp[0].pszString);
}
#line 3042 "parser.tab.cpp" /* yacc.c:1646  */
    break;


#line 3046 "parser.tab.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1426 "parser.y" /* yacc.c:1906  */


MP_MODULE *ParseMIB(const TCHAR *fileName)
{
   m_pModule = NULL;
   mpin = _tfopen(fileName, _T("r"));
   if (mpin != NULL)
   {
#ifdef UNICODE
	  WideCharToMultiByte(CP_ACP, WC_DEFAULTCHAR |WC_COMPOSITECHECK, fileName, -1, s_currentFilename, MAX_PATH, NULL, NULL);
#else
	  s_currentFilename = fileName;
#endif
      g_nCurrLine = 1;
      InitStateStack();
      /*mpdebug=1;*/
      ResetScanner();
      mpparse();
      fclose(mpin);
   }
   else
   {
#ifdef UNICODE
      char *name = MBStringFromWideString(fileName);
      Error(ERR_CANNOT_OPEN_FILE, name, strerror(errno));
      free(name);
#else
      Error(ERR_CANNOT_OPEN_FILE, fileName, strerror(errno));
#endif
      return NULL;
   }
   return m_pModule;
}

extern "C" int mpwrap()
{
	return 1;
}

int mperror(const char *pszMsg)
{
   Error(ERR_PARSER_ERROR, s_currentFilename, pszMsg, g_nCurrLine);
   return 0;
}
