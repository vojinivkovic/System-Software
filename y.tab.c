/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "misc/assembly_bison.y"

#include "assembler.hpp"
#include "aux/instructions_functions.hpp"
#include "aux/directive_functions.hpp"
#include "aux/expression_tokens.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>

int yylex(void);
int yywrap();
int yyerror(const char* s);


static uint8_t parseRegister(const char* variable)
{
     if(std::strcmp(variable, "sp") == 0)
     {    
          return 14;
     }
     else if (std::strcmp(variable, "pc") == 0)
     {
          return 15;
     }
     else
     {
          return static_cast<uint8_t>(std::stoul(variable));
     }
}

static uint8_t parseSPRegister(const char* variable)
{
     if(std::strcmp(variable, "status") == 0)
     {
          return 0;
     }
     else if(std::strcmp(variable, "handler") == 0)
     {
          return 1;
     }
     else 
     {
          return 2;
     }
}

static std::uint32_t resolveLiteral(const std::string& literal)
{
     if (literal.empty()) 
     {
          throw std::invalid_argument("Empty literal");
     }

    std::size_t pos = 0;
    bool negative = false;

    if (literal[pos] == '+' || literal[pos] == '-') 
    {
          negative = (literal[pos] == '-');
          pos++;
          if (pos >= literal.size()) 
          {
               throw std::invalid_argument("Literal contains only a sign");
          }
     }

    int base = 10;
    std::size_t digitsPos = pos;

    if (pos + 1 < literal.size() &&
        literal[pos] == '0' &&
        (literal[pos + 1] == 'x' || literal[pos + 1] == 'X'))
    {
        base = 16;
        digitsPos = pos + 2;
    }

    else if (pos + 1 < literal.size() &&
             literal[pos] == '0' &&
             (literal[pos + 1] == 'b' || literal[pos + 1] == 'B'))
    {
        base = 2;
        digitsPos = pos + 2;
    }

    if (digitsPos >= literal.size()) {
        throw std::invalid_argument("Missing digits in literal: " + literal);
    }

    std::uint64_t value;
    if (base == 2) 
    {
        value = std::stoull(literal.substr(digitsPos), nullptr, 2);
    }
    else
    {
          value = std::stoull(literal, nullptr, base);
    }

    if(negative)
    {
          value = static_cast<std::uint64_t>(static_cast<std::uint32_t>(-static_cast<std::int64_t>(value)));
    }

    if (value > std::numeric_limits<std::uint32_t>::max()) 
    {
        throw std::out_of_range("Literal does not fit in 32 bits: " + literal);
    }
    return static_cast<std::uint32_t>(value);
}

std::vector<Argument> args;
std::vector<MacroParameter> params;
std::string instr;
std::string directive;

#line 189 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 123 "misc/assembly_bison.y"

    #include <vector>
    #include "aux/expression_tokens.hpp"
    #include "aux/instructions_functions.hpp"

#line 238 "y.tab.c"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    DIRECTIVE = 258,
    EQU = 259,
    COMMAND = 260,
    GP_REGISTER = 261,
    CS_REGISTER = 262,
    SYMBOL = 263,
    LABEL = 264,
    STRING = 265,
    LITERAL = 266,
    END = 267,
    UMINUS = 268,
    UPLUS = 269
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 129 "misc/assembly_bison.y"

    char* field;
    Argument* argument;
    std::vector<Token>* tokenVector;

#line 270 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

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
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   96

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  24
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  15
/* YYNRULES -- Number of rules.  */
#define YYNRULES  54
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  82

#define YYUNDEFTOK  2
#define YYMAXUTOK   269


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      17,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    21,     2,     2,     2,
      19,    20,     2,    13,    18,    14,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    22,     2,    23,     2,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12,    15,    16
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   147,   147,   148,   149,   150,   153,   154,   155,   159,
     159,   164,   164,   171,   176,   182,   191,   194,   205,   209,
     215,   219,   227,   230,   236,   244,   248,   254,   260,   265,
     270,   277,   281,   287,   295,   298,   306,   318,   323,   328,
     333,   338,   343,   350,   358,   366,   377,   384,   392,   401,
     412,   425,   441,   458,   474
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DIRECTIVE", "EQU", "COMMAND",
  "GP_REGISTER", "CS_REGISTER", "SYMBOL", "LABEL", "STRING", "LITERAL",
  "END", "'+'", "'-'", "UMINUS", "UPLUS", "'\\n'", "','", "'('", "')'",
  "'$'", "'['", "']'", "$accept", "program", "line", "$@1", "$@2",
  "assembly_directive", "equ_directive", "list_of_parameters",
  "expression", "term", "signed_term", "base", "signed_literal", "operand",
  "assembly_command", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    43,    45,   268,   269,    10,    44,    40,
      41,    36,    91,    93
};
# endif

#define YYPACT_NINF (-41)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-12)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -41,     1,   -41,    54,    -5,     8,    22,   -10,   -41,    -8,
     -41,   -41,   -41,   -41,   -41,   -41,    12,    49,    56,   -41,
      59,    60,    61,   -41,   -41,    58,    44,    63,    29,    75,
     -41,   -41,   -41,   -41,    62,    32,    14,    48,   -41,   -41,
     -41,   -11,    76,   -41,   -41,   -41,   -41,   -41,   -41,    36,
      36,    32,    24,   -41,   -41,    65,   -41,   -41,   -41,    50,
     -41,   -41,   -41,   -41,    32,   -41,   -41,    39,    32,    32,
      20,    64,    66,    43,   -41,   -41,   -41,   -41,   -41,   -41,
     -41,   -41
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,    13,     0,    46,     9,     0,     3,     0,
       6,    16,     7,    18,    15,    34,     0,     0,    14,    19,
       0,    42,     0,    38,    37,     0,     0,    47,     0,     0,
       5,     4,    36,    35,     0,     0,     0,     0,    41,    34,
      40,     0,     0,    10,    12,    20,    21,    25,    26,     0,
       0,     0,    17,    22,    28,    42,    53,    50,    52,     0,
      43,    49,    31,    32,     0,    29,    30,     0,     0,     0,
       0,     0,     0,     0,    27,    23,    24,    42,    54,    44,
      45,    33
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -41,   -41,   -41,   -41,   -41,    57,   -41,   -41,   -40,   -20,
     -41,    34,   -17,   -36,    67
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     9,    28,    29,    10,    11,    18,    52,    53,
      54,    65,    19,    27,    12
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      57,     2,    59,    20,     3,     4,     5,    30,    40,    31,
       6,    67,    60,     7,    21,    22,    23,    46,     8,    24,
      55,    56,    23,    32,    73,    24,    77,   -11,    23,    25,
      26,    24,     3,     4,    78,    25,    26,    68,    69,    -8,
      47,    25,    26,    48,    62,    49,    50,    63,    75,    76,
      41,    51,    68,    69,    58,    64,    68,    69,    71,    74,
      33,    72,    13,    81,    14,    15,    38,    16,    17,    39,
      45,    16,    17,    15,    34,    16,    17,    35,    36,    37,
       5,    42,    61,    70,    66,    43,     0,    79,     0,    80,
       0,     0,     0,     0,     0,     0,    44
};

static const yytype_int8 yycheck[] =
{
      36,     0,    13,     8,     3,     4,     5,    17,    25,    17,
       9,    51,    23,    12,     6,     7,     8,    34,    17,    11,
       6,     7,     8,    11,    64,    11,     6,     5,     8,    21,
      22,    11,     3,     4,    70,    21,    22,    13,    14,    17,
       8,    21,    22,    11,     8,    13,    14,    11,    68,    69,
       6,    19,    13,    14,     6,    19,    13,    14,     8,    20,
      11,    11,     8,    20,    10,    11,     8,    13,    14,    11,
       8,    13,    14,    11,    18,    13,    14,    18,    18,    18,
       5,    18,     6,    18,    50,    28,    -1,    23,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    29
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    25,     0,     3,     4,     5,     9,    12,    17,    26,
      29,    30,    38,     8,    10,    11,    13,    14,    31,    36,
       8,     6,     7,     8,    11,    21,    22,    37,    27,    28,
      17,    17,    11,    11,    18,    18,    18,    18,     8,    11,
      36,     6,    18,    29,    38,     8,    36,     8,    11,    13,
      14,    19,    32,    33,    34,     6,     7,    37,     6,    13,
      23,     6,     8,    11,    19,    35,    35,    32,    13,    14,
      18,     8,    11,    32,    20,    33,    33,     6,    37,    23,
      23,    20
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    24,    25,    25,    25,    25,    26,    26,    26,    27,
      26,    28,    26,    29,    29,    29,    29,    30,    31,    31,
      31,    31,    32,    32,    32,    33,    33,    33,    33,    34,
      34,    35,    35,    35,    36,    36,    36,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    38,    38,    38,    38,
      38,    38,    38,    38,    38
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     3,     3,     1,     1,     1,     0,
       3,     0,     3,     1,     2,     2,     1,     4,     1,     1,
       3,     3,     1,     3,     3,     1,     1,     3,     1,     2,
       2,     1,     1,     3,     1,     2,     2,     1,     1,     2,
       2,     2,     1,     3,     5,     5,     1,     2,     2,     4,
       4,     4,     4,     4,     6
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
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
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
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
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
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
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

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
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
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
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
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
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
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
          ++yyp;
          ++yyformat;
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
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

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
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
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
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 5:
#line 150 "misc/assembly_bison.y"
                         { YYACCEPT; }
#line 1499 "y.tab.c"
    break;

  case 8:
#line 155 "misc/assembly_bison.y"
             {
          Assembler::getCurrentSection()->defineSymbol((yyvsp[0].field));
          free((yyvsp[0].field));
      }
#line 1508 "y.tab.c"
    break;

  case 9:
#line 159 "misc/assembly_bison.y"
             {
          Assembler::getCurrentSection()->defineSymbol((yyvsp[0].field));
          free((yyvsp[0].field));              
     }
#line 1517 "y.tab.c"
    break;

  case 11:
#line 164 "misc/assembly_bison.y"
             {
          Assembler::getCurrentSection()->defineSymbol((yyvsp[0].field));
          free((yyvsp[0].field));              
     }
#line 1526 "y.tab.c"
    break;

  case 13:
#line 171 "misc/assembly_bison.y"
                               {
                         directive = (yyvsp[0].field);
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         free((yyvsp[0].field));
                    }
#line 1536 "y.tab.c"
    break;

  case 14:
#line 176 "misc/assembly_bison.y"
                                                  {
                         directive = (yyvsp[-1].field);
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         params.clear();
                         free((yyvsp[-1].field));
                    }
#line 1547 "y.tab.c"
    break;

  case 15:
#line 182 "misc/assembly_bison.y"
                                      {
                         directive = (yyvsp[-1].field);
                         params.push_back(MacroParameter{MacroParameterType::String, (yyvsp[0].field), 0, {}});
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         params.clear();
                         free((yyvsp[-1].field));
                         free((yyvsp[0].field));
                    }
#line 1560 "y.tab.c"
    break;

  case 17:
#line 194 "misc/assembly_bison.y"
                                         {
                         directive = (yyvsp[-3].field);
                         params.push_back(MacroParameter{MacroParameterType::Symbol, (yyvsp[-2].field), 0, {}});
                         params.push_back(MacroParameter{MacroParameterType::Expression, "", 0, std::move(*(yyvsp[0].tokenVector))});
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         params.clear();
                         free((yyvsp[-3].field));
                         free((yyvsp[-2].field));
                         delete (yyvsp[0].tokenVector);
                    }
#line 1575 "y.tab.c"
    break;

  case 18:
#line 205 "misc/assembly_bison.y"
                            {
                         params.push_back(MacroParameter{MacroParameterType::Symbol, (yyvsp[0].field), 0, {}});
                         free((yyvsp[0].field));
                    }
#line 1584 "y.tab.c"
    break;

  case 19:
#line 209 "misc/assembly_bison.y"
                                    {
                         std::uint32_t raw = resolveLiteral((yyvsp[0].field));
                         std::int32_t value = static_cast<std::int32_t>(raw); 
                         free((yyvsp[0].field));
                         params.push_back(MacroParameter{MacroParameterType::Literal, "", value, {}});
                   }
#line 1595 "y.tab.c"
    break;

  case 20:
#line 215 "misc/assembly_bison.y"
                                                   {
                         params.push_back(MacroParameter{MacroParameterType::Symbol, (yyvsp[0].field), 0, {}});
                         free((yyvsp[0].field));
                    }
#line 1604 "y.tab.c"
    break;

  case 21:
#line 219 "misc/assembly_bison.y"
                                                           {
                         std::uint32_t raw = resolveLiteral((yyvsp[0].field));
                         std::int32_t value = static_cast<std::int32_t>(raw); 
                         free((yyvsp[0].field));
                         params.push_back(MacroParameter{MacroParameterType::Literal, "", value, {}});
                   }
#line 1615 "y.tab.c"
    break;

  case 22:
#line 227 "misc/assembly_bison.y"
                  {
               (yyval.tokenVector) = (yyvsp[0].tokenVector);
          }
#line 1623 "y.tab.c"
    break;

  case 23:
#line 230 "misc/assembly_bison.y"
                                 {
               (yyval.tokenVector) = (yyvsp[-2].tokenVector);
               (yyval.tokenVector)->push_back(Token{TokenType::BINPLUS, 0, ""});
               (yyval.tokenVector)->insert((yyval.tokenVector)->end(), (yyvsp[0].tokenVector)->begin(), (yyvsp[0].tokenVector)->end());
               delete (yyvsp[0].tokenVector);
           }
#line 1634 "y.tab.c"
    break;

  case 24:
#line 236 "misc/assembly_bison.y"
                                 {
               (yyval.tokenVector) = (yyvsp[-2].tokenVector);
               (yyval.tokenVector)->push_back(Token{TokenType::BINMINUS, 0, ""});
               (yyval.tokenVector)->insert((yyval.tokenVector)->end(), (yyvsp[0].tokenVector)->begin(), (yyvsp[0].tokenVector)->end());
               delete (yyvsp[0].tokenVector);
           }
#line 1645 "y.tab.c"
    break;

  case 25:
#line 244 "misc/assembly_bison.y"
              {
          (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::SYMBOL, 0, (yyvsp[0].field)}};
          free((yyvsp[0].field));
     }
#line 1654 "y.tab.c"
    break;

  case 26:
#line 248 "misc/assembly_bison.y"
               {
          std::uint32_t raw = resolveLiteral((yyvsp[0].field));
          std::int32_t value = static_cast<std::int32_t>(raw); 
          (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::LITERAL, value, ""}};
          free((yyvsp[0].field));
     }
#line 1665 "y.tab.c"
    break;

  case 27:
#line 254 "misc/assembly_bison.y"
                          {
          (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::LPARENTHESES, 0,  ""}};
          (yyval.tokenVector)->insert((yyval.tokenVector)->end(), (yyvsp[-1].tokenVector)->begin(), (yyvsp[-1].tokenVector)->end());
          (yyval.tokenVector)->push_back(Token{TokenType::RPARENTHESES, 0, ""});
          delete (yyvsp[-1].tokenVector);
     }
#line 1676 "y.tab.c"
    break;

  case 28:
#line 260 "misc/assembly_bison.y"
                   {
          (yyval.tokenVector) = (yyvsp[0].tokenVector);
     }
#line 1684 "y.tab.c"
    break;

  case 29:
#line 265 "misc/assembly_bison.y"
                       {
               (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::UNPLUS, 0, ""}};
               (yyval.tokenVector)->insert((yyval.tokenVector)->end(), (yyvsp[0].tokenVector)->begin(), (yyvsp[0].tokenVector)->end());
               delete (yyvsp[0].tokenVector);
               }
#line 1694 "y.tab.c"
    break;

  case 30:
#line 270 "misc/assembly_bison.y"
                       {
               (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::UNMINUS, 0, ""}};
               (yyval.tokenVector)->insert((yyval.tokenVector)->end(), (yyvsp[0].tokenVector)->begin(), (yyvsp[0].tokenVector)->end());
               delete (yyvsp[0].tokenVector);
               }
#line 1704 "y.tab.c"
    break;

  case 31:
#line 277 "misc/assembly_bison.y"
              {
          (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::SYMBOL, 0, (yyvsp[0].field)}};
          free((yyvsp[0].field));
     }
#line 1713 "y.tab.c"
    break;

  case 32:
#line 281 "misc/assembly_bison.y"
               {
          std::uint32_t raw = resolveLiteral((yyvsp[0].field));
          std::int32_t value = static_cast<std::int32_t>(raw); 
          (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::LITERAL, value, ""}};
          free((yyvsp[0].field));
     }
#line 1724 "y.tab.c"
    break;

  case 33:
#line 287 "misc/assembly_bison.y"
                          {
          (yyval.tokenVector) = new std::vector<Token>{Token{TokenType::LPARENTHESES, 0,  ""}};
          (yyval.tokenVector)->insert((yyval.tokenVector)->end(), (yyvsp[-1].tokenVector)->begin(), (yyvsp[-1].tokenVector)->end());
          (yyval.tokenVector)->push_back(Token{TokenType::RPARENTHESES, 0, ""});
          delete (yyvsp[-1].tokenVector);
     }
#line 1735 "y.tab.c"
    break;

  case 34:
#line 295 "misc/assembly_bison.y"
                         { 
                         (yyval.field) = (yyvsp[0].field);
                    }
#line 1743 "y.tab.c"
    break;

  case 35:
#line 298 "misc/assembly_bison.y"
                             {
                         size_t len = strlen((yyvsp[0].field));
                         (yyval.field) = (char*)malloc((len + 2) * sizeof(char));
                         (yyval.field)[0] = '-';
                         strcpy((yyval.field) + 1, (yyvsp[0].field));

                         free((yyvsp[0].field));
                    }
#line 1756 "y.tab.c"
    break;

  case 36:
#line 306 "misc/assembly_bison.y"
                             {
                         size_t len = strlen((yyvsp[0].field));
                         (yyval.field) = (char*)malloc((len + 2) * sizeof(char));
                         (yyval.field)[0] = '+';
                         strcpy((yyval.field) + 1, (yyvsp[0].field));

                         free((yyvsp[0].field));
                    }
#line 1769 "y.tab.c"
    break;

  case 37:
#line 318 "misc/assembly_bison.y"
                  {
                    (yyval.argument) = new Argument{ArgumentType::OperandLiteral, 
                    AddressingType::MemoryDirect, 0, (yyvsp[0].field)}; 
                    free((yyvsp[0].field));
                }
#line 1779 "y.tab.c"
    break;

  case 38:
#line 323 "misc/assembly_bison.y"
                 {
                    (yyval.argument) = new Argument{ArgumentType::OperandSymbol, 
                    AddressingType::MemoryDirect, 0, (yyvsp[0].field)}; 
                    free((yyvsp[0].field));
               }
#line 1789 "y.tab.c"
    break;

  case 39:
#line 328 "misc/assembly_bison.y"
                      {
                    (yyval.argument) = new Argument{ArgumentType::OperandLiteral, 
                    AddressingType::Immediate, 0, (yyvsp[0].field)}; 
                    free((yyvsp[0].field));
               }
#line 1799 "y.tab.c"
    break;

  case 40:
#line 333 "misc/assembly_bison.y"
                             {
               (yyval.argument) = new Argument{ArgumentType::OperandLiteral, 
                    AddressingType::Immediate, 0, (yyvsp[0].field)}; 
                    free((yyvsp[0].field));
               }
#line 1809 "y.tab.c"
    break;

  case 41:
#line 338 "misc/assembly_bison.y"
                     {
               (yyval.argument) = new Argument{ArgumentType::OperandSymbol, 
                    AddressingType::Immediate, 0, (yyvsp[0].field)}; 
                    free((yyvsp[0].field));
               }
#line 1819 "y.tab.c"
    break;

  case 42:
#line 343 "misc/assembly_bison.y"
                      {
               uint8_t registerNum = parseRegister((yyvsp[0].field));
               
               (yyval.argument) = new Argument{ArgumentType::Register, 
                    AddressingType::RegisterDirect, registerNum, ""}; 
                    free((yyvsp[0].field)); 
               }
#line 1831 "y.tab.c"
    break;

  case 43:
#line 350 "misc/assembly_bison.y"
                              {
               uint8_t registerNum = parseRegister((yyvsp[-1].field));
               
               (yyval.argument) = new Argument{ArgumentType::Register, 
                    AddressingType::RegisterIndirect, registerNum, ""}; 
                    free((yyvsp[-1].field)); 
               }
#line 1843 "y.tab.c"
    break;

  case 44:
#line 358 "misc/assembly_bison.y"
                                         {
               uint8_t registerNum = parseRegister((yyvsp[-3].field));
               
               (yyval.argument) = new Argument{ArgumentType::RegisterAndSymbol, 
                    AddressingType::RegisterIndirect, registerNum, (yyvsp[-1].field)}; 
                    free((yyvsp[-3].field));
                    free((yyvsp[-1].field)); 
               }
#line 1856 "y.tab.c"
    break;

  case 45:
#line 366 "misc/assembly_bison.y"
                                          {
               uint8_t registerNum = parseRegister((yyvsp[-3].field));
               
               (yyval.argument) = new Argument{ArgumentType::RegisterAndLiteral, 
                    AddressingType::RegisterIndirect, registerNum, (yyvsp[-1].field)}; 
                    free((yyvsp[-3].field));
                    free((yyvsp[-1].field)); 
               }
#line 1869 "y.tab.c"
    break;

  case 46:
#line 377 "misc/assembly_bison.y"
                           {
                         args.clear();
                         instr = (yyvsp[0].field);
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free((yyvsp[0].field));
                         
                    }
#line 1881 "y.tab.c"
    break;

  case 47:
#line 384 "misc/assembly_bison.y"
                                   { 
                         instr = (yyvsp[-1].field);
                         args.push_back(*(yyvsp[0].argument));
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free((yyvsp[-1].field));
                         delete (yyvsp[0].argument);
                         args.clear();
                    }
#line 1894 "y.tab.c"
    break;

  case 48:
#line 392 "misc/assembly_bison.y"
                                       {
                         instr = (yyvsp[-1].field);
                         uint8_t registerNum = parseRegister((yyvsp[0].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free((yyvsp[-1].field));
                         free((yyvsp[0].field));
                         args.clear();
                    }
#line 1908 "y.tab.c"
    break;

  case 49:
#line 401 "misc/assembly_bison.y"
                                                   {
                         instr = (yyvsp[-3].field);
                         uint8_t registerNum = parseRegister((yyvsp[0].field));
                         args.push_back(*(yyvsp[-2].argument));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free((yyvsp[-3].field));
                         free((yyvsp[0].field));
                         delete (yyvsp[-2].argument);
                         args.clear();
                    }
#line 1924 "y.tab.c"
    break;

  case 50:
#line 412 "misc/assembly_bison.y"
                                                   {
                         instr = (yyvsp[-3].field);
                         uint8_t registerNum = parseRegister((yyvsp[-2].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         args.push_back(*(yyvsp[0].argument));
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free((yyvsp[-3].field));
                         free((yyvsp[-2].field));
                         delete (yyvsp[0].argument);
                         args.clear();
                    }
#line 1942 "y.tab.c"
    break;

  case 51:
#line 425 "misc/assembly_bison.y"
                                                       {
                         instr = (yyvsp[-3].field);

                         uint8_t registerNum1 = parseRegister((yyvsp[-2].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                        uint8_t registerNum2 = parseRegister((yyvsp[0].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free((yyvsp[-3].field));
                         free((yyvsp[-2].field));
                         free((yyvsp[0].field));
                         args.clear();
                    }
#line 1963 "y.tab.c"
    break;

  case 52:
#line 441 "misc/assembly_bison.y"
                                                       {
                         instr = (yyvsp[-3].field);

                         uint8_t registerNum1 = parseSPRegister((yyvsp[-2].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         uint8_t registerNum2 = parseRegister((yyvsp[0].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free((yyvsp[-3].field));
                         free((yyvsp[-2].field));
                         free((yyvsp[0].field));
                         args.clear();
                    }
#line 1984 "y.tab.c"
    break;

  case 53:
#line 458 "misc/assembly_bison.y"
                                                       {
                         instr = (yyvsp[-3].field);

                         uint8_t registerNum1 = parseRegister((yyvsp[-2].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         uint8_t registerNum2 = parseSPRegister((yyvsp[0].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free((yyvsp[-3].field));
                         free((yyvsp[-2].field));
                         free((yyvsp[0].field));
                         args.clear();
                    }
#line 2005 "y.tab.c"
    break;

  case 54:
#line 474 "misc/assembly_bison.y"
                                                                   {
                         instr = (yyvsp[-5].field);

                         uint8_t registerNum1 = parseRegister((yyvsp[-4].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         uint8_t registerNum2 = parseRegister((yyvsp[-2].field));
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});

                         args.push_back(*(yyvsp[0].argument));
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free((yyvsp[-5].field));
                         free((yyvsp[-4].field));
                         free((yyvsp[-2].field));
                         delete (yyvsp[0].argument);
                         args.clear();
                 }
#line 2028 "y.tab.c"
    break;


#line 2032 "y.tab.c"

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
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

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
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

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


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
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
                  yystos[+*yyssp], yyvsp);
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
#line 494 "misc/assembly_bison.y"

int yyerror(const char* s)
{
    throw std::runtime_error(s);
    return 1;
}
