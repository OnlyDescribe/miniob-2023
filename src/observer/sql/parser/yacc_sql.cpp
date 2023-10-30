/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"
#include "sql/expr/expression.h"

using namespace std;

string token_name(const char *sql_string, YYLTYPE *llocp)
{
  return string(sql_string + llocp->first_column, llocp->last_column - llocp->first_column + 1);
}

int yyerror(YYLTYPE *llocp, const char *sql_string, ParsedSqlResult *sql_result, yyscan_t scanner, const char *msg, enum SqlCommandFlag flag = SCF_ERROR)
{
  std::unique_ptr<ParsedSqlNode> error_sql_node = std::make_unique<ParsedSqlNode>(flag);
  error_sql_node->error.error_msg = msg;
  error_sql_node->error.line = llocp->first_line;
  error_sql_node->error.column = llocp->first_column;
  sql_result->add_sql_node(std::move(error_sql_node));
  return 0;
}

ArithmeticExpr *create_arithmetic_expression(ArithmeticExpr::Type type,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ArithmeticExpr *expr = new ArithmeticExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

bool IsAttributesVailid(const std::vector<PExpr *> &select_attr)
{
  int relattr_cnt = 0;
  for (const PExpr *node : select_attr) {
    if (node->type == PExpType::UNARY && node->uexp->attr.attribute.aggr_type == AggrFuncType::INVALID) {
      relattr_cnt++;
    }
  }
  return relattr_cnt == 0 || (relattr_cnt == static_cast<int>(attributes.size()));
}


#line 126 "yacc_sql.cpp"

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

#include "yacc_sql.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_INDEX = 8,                      /* INDEX  */
  YYSYMBOL_UNIQUE = 9,                     /* UNIQUE  */
  YYSYMBOL_CALC = 10,                      /* CALC  */
  YYSYMBOL_SELECT = 11,                    /* SELECT  */
  YYSYMBOL_DESC = 12,                      /* DESC  */
  YYSYMBOL_SHOW = 13,                      /* SHOW  */
  YYSYMBOL_SYNC = 14,                      /* SYNC  */
  YYSYMBOL_INSERT = 15,                    /* INSERT  */
  YYSYMBOL_DELETE = 16,                    /* DELETE  */
  YYSYMBOL_UPDATE = 17,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 18,                    /* LBRACE  */
  YYSYMBOL_AGGR_MAX = 19,                  /* AGGR_MAX  */
  YYSYMBOL_AGGR_MIN = 20,                  /* AGGR_MIN  */
  YYSYMBOL_AGGR_SUM = 21,                  /* AGGR_SUM  */
  YYSYMBOL_AGGR_AVG = 22,                  /* AGGR_AVG  */
  YYSYMBOL_AGGR_COUNT = 23,                /* AGGR_COUNT  */
  YYSYMBOL_LENGTH = 24,                    /* LENGTH  */
  YYSYMBOL_ROUND = 25,                     /* ROUND  */
  YYSYMBOL_DATE_FORMAT = 26,               /* DATE_FORMAT  */
  YYSYMBOL_ASC = 27,                       /* ASC  */
  YYSYMBOL_RBRACE = 28,                    /* RBRACE  */
  YYSYMBOL_INNER = 29,                     /* INNER  */
  YYSYMBOL_JOIN = 30,                      /* JOIN  */
  YYSYMBOL_COMMA = 31,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 32,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 33,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 34,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 35,                     /* INT_T  */
  YYSYMBOL_ORDER = 36,                     /* ORDER  */
  YYSYMBOL_GROUP = 37,                     /* GROUP  */
  YYSYMBOL_BY = 38,                        /* BY  */
  YYSYMBOL_STRING_T = 39,                  /* STRING_T  */
  YYSYMBOL_TEXT_T = 40,                    /* TEXT_T  */
  YYSYMBOL_FLOAT_T = 41,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 42,                    /* DATE_T  */
  YYSYMBOL_HELP = 43,                      /* HELP  */
  YYSYMBOL_EXIT = 44,                      /* EXIT  */
  YYSYMBOL_DOT = 45,                       /* DOT  */
  YYSYMBOL_INTO = 46,                      /* INTO  */
  YYSYMBOL_VALUES = 47,                    /* VALUES  */
  YYSYMBOL_FROM = 48,                      /* FROM  */
  YYSYMBOL_WHERE = 49,                     /* WHERE  */
  YYSYMBOL_AND = 50,                       /* AND  */
  YYSYMBOL_SET = 51,                       /* SET  */
  YYSYMBOL_ON = 52,                        /* ON  */
  YYSYMBOL_LOAD = 53,                      /* LOAD  */
  YYSYMBOL_DATA = 54,                      /* DATA  */
  YYSYMBOL_INFILE = 55,                    /* INFILE  */
  YYSYMBOL_EXPLAIN = 56,                   /* EXPLAIN  */
  YYSYMBOL_EQ = 57,                        /* EQ  */
  YYSYMBOL_LT = 58,                        /* LT  */
  YYSYMBOL_GT = 59,                        /* GT  */
  YYSYMBOL_LE = 60,                        /* LE  */
  YYSYMBOL_GE = 61,                        /* GE  */
  YYSYMBOL_NE = 62,                        /* NE  */
  YYSYMBOL_NOT = 63,                       /* NOT  */
  YYSYMBOL_IS = 64,                        /* IS  */
  YYSYMBOL_AS = 65,                        /* AS  */
  YYSYMBOL_LIKE = 66,                      /* LIKE  */
  YYSYMBOL_EXISTS = 67,                    /* EXISTS  */
  YYSYMBOL_IN = 68,                        /* IN  */
  YYSYMBOL_DEFAULT = 69,                   /* DEFAULT  */
  YYSYMBOL_HAVING = 70,                    /* HAVING  */
  YYSYMBOL_NULL_T = 71,                    /* NULL_T  */
  YYSYMBOL_NUMBER = 72,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 73,                     /* FLOAT  */
  YYSYMBOL_ID = 74,                        /* ID  */
  YYSYMBOL_SSS = 75,                       /* SSS  */
  YYSYMBOL_DATE = 76,                      /* DATE  */
  YYSYMBOL_OR = 77,                        /* OR  */
  YYSYMBOL_78_ = 78,                       /* '+'  */
  YYSYMBOL_79_ = 79,                       /* '-'  */
  YYSYMBOL_80_ = 80,                       /* '*'  */
  YYSYMBOL_81_ = 81,                       /* '/'  */
  YYSYMBOL_UMINUS = 82,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 83,                  /* $accept  */
  YYSYMBOL_commands = 84,                  /* commands  */
  YYSYMBOL_command_wrapper = 85,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 86,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 87,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 88,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 89,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 90,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 91,             /* rollback_stmt  */
  YYSYMBOL_drop_table_stmt = 92,           /* drop_table_stmt  */
  YYSYMBOL_show_tables_stmt = 93,          /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 94,           /* desc_table_stmt  */
  YYSYMBOL_load_data_stmt = 95,            /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 96,              /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 97,         /* set_variable_stmt  */
  YYSYMBOL_create_index_stmt = 98,         /* create_index_stmt  */
  YYSYMBOL_drop_index_stmt = 99,           /* drop_index_stmt  */
  YYSYMBOL_create_table_stmt = 100,        /* create_table_stmt  */
  YYSYMBOL_insert_stmt = 101,              /* insert_stmt  */
  YYSYMBOL_delete_stmt = 102,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 103,              /* update_stmt  */
  YYSYMBOL_select_stmt = 104,              /* select_stmt  */
  YYSYMBOL_calc_stmt = 105,                /* calc_stmt  */
  YYSYMBOL_assignment_list = 106,          /* assignment_list  */
  YYSYMBOL_select_attr = 107,              /* select_attr  */
  YYSYMBOL_select_attr_list = 108,         /* select_attr_list  */
  YYSYMBOL_select_from = 109,              /* select_from  */
  YYSYMBOL_from_list = 110,                /* from_list  */
  YYSYMBOL_inner_join = 111,               /* inner_join  */
  YYSYMBOL_where = 112,                    /* where  */
  YYSYMBOL_group_by = 113,                 /* group_by  */
  YYSYMBOL_group_by_list = 114,            /* group_by_list  */
  YYSYMBOL_having = 115,                   /* having  */
  YYSYMBOL_order_by = 116,                 /* order_by  */
  YYSYMBOL_order_condtions = 117,          /* order_condtions  */
  YYSYMBOL_id_list = 118,                  /* id_list  */
  YYSYMBOL_attr_def_list = 119,            /* attr_def_list  */
  YYSYMBOL_attr_def = 120,                 /* attr_def  */
  YYSYMBOL_number = 121,                   /* number  */
  YYSYMBOL_type = 122,                     /* type  */
  YYSYMBOL_value_list = 123,               /* value_list  */
  YYSYMBOL_value = 124,                    /* value  */
  YYSYMBOL_aggr_func_type = 125,           /* aggr_func_type  */
  YYSYMBOL_field_or_star = 126,            /* field_or_star  */
  YYSYMBOL_field_or_star_list = 127,       /* field_or_star_list  */
  YYSYMBOL_rel_attr = 128,                 /* rel_attr  */
  YYSYMBOL_expression_list = 129,          /* expression_list  */
  YYSYMBOL_expression = 130,               /* expression  */
  YYSYMBOL_comp_op = 131,                  /* comp_op  */
  YYSYMBOL_unary_pexpr = 132,              /* unary_pexpr  */
  YYSYMBOL_arith_pexpr = 133,              /* arith_pexpr  */
  YYSYMBOL_cond_pexpr = 134,               /* cond_pexpr  */
  YYSYMBOL_func_pexpr = 135,               /* func_pexpr  */
  YYSYMBOL_pexpr = 136,                    /* pexpr  */
  YYSYMBOL_opt_semicolon = 137             /* opt_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




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

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
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
typedef yytype_int16 yy_state_t;

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
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
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

#if 1

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
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  87
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   600

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  168
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  332

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   333


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    80,    78,     2,    79,     2,    81,     2,     2,
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
      75,    76,    77,    82
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   243,   243,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   274,   280,   285,   291,   297,   303,   309,
     316,   322,   330,   343,   351,   362,   384,   409,   419,   438,
     453,   466,   495,   519,   543,   571,   584,   587,   607,   616,
     628,   643,   646,   654,   665,   679,   691,   705,   721,   724,
     735,   748,   761,   773,   787,   804,   807,   815,   818,   824,
     827,   839,   856,   859,   871,   888,   891,   898,   901,   914,
     927,   943,   946,   958,   970,   987,   990,  1004,  1007,  1020,
    1029,  1038,  1047,  1056,  1071,  1086,  1101,  1118,  1121,  1122,
    1123,  1124,  1125,  1131,  1134,  1145,  1149,  1153,  1161,  1165,
    1177,  1178,  1179,  1180,  1181,  1184,  1187,  1193,  1196,  1203,
    1214,  1219,  1225,  1230,  1237,  1248,  1253,  1265,  1268,  1271,
    1274,  1277,  1281,  1284,  1292,  1293,  1294,  1295,  1296,  1297,
    1298,  1299,  1304,  1311,  1320,  1324,  1328,  1332,  1336,  1343,
    1347,  1351,  1356,  1364,  1372,  1376,  1380,  1384,  1391,  1397,
    1404,  1412,  1423,  1430,  1437,  1441,  1448,  1457,  1458
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "INDEX", "UNIQUE", "CALC", "SELECT", "DESC",
  "SHOW", "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "AGGR_MAX",
  "AGGR_MIN", "AGGR_SUM", "AGGR_AVG", "AGGR_COUNT", "LENGTH", "ROUND",
  "DATE_FORMAT", "ASC", "RBRACE", "INNER", "JOIN", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "ORDER", "GROUP", "BY",
  "STRING_T", "TEXT_T", "FLOAT_T", "DATE_T", "HELP", "EXIT", "DOT", "INTO",
  "VALUES", "FROM", "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE",
  "EXPLAIN", "EQ", "LT", "GT", "LE", "GE", "NE", "NOT", "IS", "AS", "LIKE",
  "EXISTS", "IN", "DEFAULT", "HAVING", "NULL_T", "NUMBER", "FLOAT", "ID",
  "SSS", "DATE", "OR", "'+'", "'-'", "'*'", "'/'", "UMINUS", "$accept",
  "commands", "command_wrapper", "exit_stmt", "help_stmt", "sync_stmt",
  "begin_stmt", "commit_stmt", "rollback_stmt", "drop_table_stmt",
  "show_tables_stmt", "desc_table_stmt", "load_data_stmt", "explain_stmt",
  "set_variable_stmt", "create_index_stmt", "drop_index_stmt",
  "create_table_stmt", "insert_stmt", "delete_stmt", "update_stmt",
  "select_stmt", "calc_stmt", "assignment_list", "select_attr",
  "select_attr_list", "select_from", "from_list", "inner_join", "where",
  "group_by", "group_by_list", "having", "order_by", "order_condtions",
  "id_list", "attr_def_list", "attr_def", "number", "type", "value_list",
  "value", "aggr_func_type", "field_or_star", "field_or_star_list",
  "rel_attr", "expression_list", "expression", "comp_op", "unary_pexpr",
  "arith_pexpr", "cond_pexpr", "func_pexpr", "pexpr", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-291)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-77)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     367,   167,    33,    -1,   119,   -68,    15,  -291,   -28,    19,
      18,  -291,  -291,  -291,  -291,  -291,    40,    65,   367,   121,
     120,  -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,
    -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,
    -291,    57,    85,   159,   104,   109,    -1,  -291,  -291,  -291,
    -291,  -291,    -1,  -291,  -291,    69,   119,  -291,  -291,  -291,
    -291,  -291,   169,   178,   179,   118,   119,   162,   119,  -291,
     160,  -291,   204,  -291,  -291,  -291,  -291,  -291,   371,  -291,
    -291,   149,   154,   184,   172,   181,  -291,  -291,  -291,  -291,
     219,   186,   165,  -291,   189,     8,  -291,    -1,    -1,    -1,
      -1,    -1,   190,   119,   119,   119,   119,   457,   -22,   532,
     168,    84,   119,   119,  -291,  -291,  -291,  -291,  -291,  -291,
     -53,     2,   170,  -291,   119,   212,   119,   119,   119,   119,
     119,  -291,   119,   198,   197,   187,   129,   188,   191,   192,
     207,   199,  -291,  -291,    54,    54,  -291,  -291,  -291,   227,
     153,   432,   457,  -291,  -291,   -19,   197,   -11,  -291,  -291,
      84,   232,   396,   328,  -291,   119,   193,  -291,   212,   457,
    -291,   482,   507,   507,   532,   532,   457,   244,   119,  -291,
     215,  -291,   228,   185,   245,   257,   205,  -291,  -291,  -291,
     119,   119,   248,   208,   119,   209,    62,   197,   197,   255,
      84,  -291,  -291,   220,   212,  -291,   457,  -291,  -291,   129,
      20,   457,   129,   275,  -291,  -291,  -291,  -291,  -291,    91,
     191,   269,   224,   281,   252,   289,   226,     3,    11,    62,
    -291,  -291,  -291,   263,   233,  -291,   212,  -291,   288,   290,
     249,   250,   253,   254,  -291,   245,  -291,   295,   260,  -291,
    -291,   -44,   261,    62,  -291,  -291,   262,   119,   291,  -291,
     129,   300,   264,   197,  -291,  -291,   309,  -291,  -291,  -291,
     266,   313,   295,   268,   292,    62,    62,  -291,    31,   117,
     305,  -291,   288,  -291,   297,  -291,   103,   295,  -291,   317,
     292,    62,  -291,  -291,   282,   284,  -291,     5,  -291,   129,
     293,   294,  -291,  -291,  -291,    62,  -291,    87,   329,   124,
     290,  -291,  -291,  -291,   285,  -291,  -291,   330,   330,     5,
    -291,  -291,   329,  -291,  -291,   134,  -291,   330,   330,  -291,
    -291,  -291
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    25,     0,     0,
       0,    26,    27,    28,    24,    23,     0,     0,     0,     0,
     167,    22,    21,    14,    15,    16,    17,     9,    10,    11,
      18,    19,    20,    12,    13,     8,     5,     7,     6,     4,
       3,     0,     0,     0,     0,     0,     0,   108,   105,   106,
     107,   109,     0,   133,    45,   125,     0,   110,   111,   112,
     113,   114,     0,     0,     0,     0,     0,   122,     0,   120,
       0,   142,     0,   143,   165,   163,   162,   166,    51,    31,
      30,     0,     0,     0,     0,     0,    33,     1,   168,     2,
       0,     0,     0,    29,     0,     0,   132,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   156,     0,   148,
       0,   117,     0,     0,   134,   135,   136,   137,   138,   139,
       0,     0,     0,   140,     0,    51,     0,     0,     0,     0,
       0,    48,     0,     0,    67,     0,     0,     0,     0,     0,
       0,     0,   131,   126,   127,   128,   129,   130,   164,     0,
       0,     0,   157,   123,   121,    58,    67,     0,   116,   115,
     117,     0,    51,   150,   141,     0,     0,   152,    51,   154,
      49,   149,   144,   145,   146,   147,   151,     0,     0,    40,
       0,    34,     0,     0,    87,     0,     0,    37,   158,   159,
       0,     0,     0,     0,     0,     0,    58,    55,    67,    69,
     117,   118,   124,     0,    51,    52,   155,   153,    50,     0,
     162,     0,     0,     0,    98,    99,   100,   101,   102,    93,
       0,     0,     0,     0,     0,     0,     0,    58,   162,    58,
      56,    44,    43,     0,    75,   119,    51,    53,   103,    46,
       0,     0,     0,     0,    94,    87,    38,    85,     0,   160,
     161,    65,     0,    58,    59,    57,     0,     0,    77,    54,
       0,     0,     0,    67,    32,    97,     0,    96,    95,    88,
       0,     0,    85,     0,    65,    58,    58,    60,    72,   162,
       0,    42,   103,    39,     0,    41,    89,    85,    35,     0,
      65,    58,    62,    61,     0,     0,    70,     0,   104,     0,
       0,     0,    90,    86,    36,    58,    63,    72,    72,    81,
      46,    92,    91,    64,     0,    73,    71,    81,    81,     0,
      78,    47,    72,    79,    80,    81,    74,    81,    81,    82,
      83,    84
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -291,  -291,   344,  -291,  -291,  -291,  -291,  -291,  -291,  -291,
    -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,  -291,
    -291,  -291,  -291,    53,  -291,  -124,  -291,  -192,  -122,  -147,
    -291,  -151,  -291,  -291,  -148,  -256,   128,   155,  -291,  -291,
      92,    -3,  -291,   236,  -155,  -290,   279,     7,  -291,  -291,
    -291,  -175,  -291,    -2,  -291
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,   263,    70,   131,   156,   197,   198,   179,
     234,   296,   258,   281,   320,   271,   221,   184,   266,   219,
     261,    71,    72,   160,   161,    73,    54,    55,   132,    74,
      75,    76,    77,   211,    89
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      53,   170,    78,   210,   230,   201,    79,   309,   194,   199,
     192,   -66,   193,   164,   -66,   165,   289,    46,    81,   228,
     -68,   273,    80,   -68,    57,    58,    59,    60,    61,   325,
     274,   303,   192,   194,   193,   254,   142,   255,   205,    44,
     -66,    45,   -66,    53,   208,   235,   195,   -66,   -66,    53,
     231,   232,   153,    95,   102,   196,   -68,   -68,   154,    96,
     -66,   277,   294,   158,   107,   166,   109,    82,   252,   159,
      47,    48,    49,   167,    50,    51,   295,   253,    52,    67,
     237,   -66,   279,   292,   293,    69,    98,    99,   100,   101,
     -68,   192,    83,   193,    53,    53,    53,    53,    53,   306,
      97,   149,   150,   151,   152,   144,   145,   146,   147,   241,
     162,   163,   259,   313,    84,   157,   285,   -76,   294,    85,
     -76,    87,   169,    88,   171,   172,   173,   174,   175,   275,
     176,    90,   314,   181,   100,   101,   317,    56,    57,    58,
      59,    60,    61,    62,    63,    64,   327,    98,    99,   100,
     101,   318,   291,   -76,   242,   319,   315,   316,   158,    91,
     243,   328,   244,   206,   159,   319,   300,    92,   305,   323,
     324,   326,   301,    41,   302,    42,    43,   329,    93,   330,
     331,   189,    65,    94,   190,   106,    66,   103,   224,   225,
      47,    48,    49,    67,    50,    51,   104,   105,    68,    69,
      47,    48,    49,   113,    50,    51,   238,   108,   110,   239,
     114,   115,   116,   117,   118,   119,   120,   121,   148,   123,
     214,   124,   111,   133,   215,   216,   217,   218,   134,   136,
     126,   127,   128,   129,   130,   135,   137,   138,   139,   140,
     113,   141,   155,   112,   168,   177,   178,   114,   115,   116,
     117,   118,   119,   120,   121,   188,   123,   282,   124,   186,
     202,   180,   209,   182,   207,   183,   185,   126,   127,   128,
     129,   130,   212,   187,   213,   222,   220,   113,   226,   223,
     249,   240,   227,   229,   114,   115,   116,   117,   118,   119,
     120,   121,   233,   123,   236,   124,   310,   246,   247,   248,
     251,   256,   113,   257,   126,   127,   128,   129,   130,   114,
     115,   116,   117,   118,   119,   120,   121,   250,   123,   260,
     124,   262,   265,   264,   267,   268,   270,   280,   283,   126,
     127,   128,   129,   130,   272,   276,   278,   286,   284,   113,
     287,   288,   290,   297,   194,   304,   114,   115,   116,   117,
     118,   119,   120,   121,   299,   123,   307,   124,   308,   322,
     294,   319,    86,   321,   311,   312,   126,   127,   128,   129,
     130,     1,     2,   269,   298,   245,   143,     3,     4,     5,
       6,     7,     8,     9,    10,   114,   115,   116,   117,   118,
     119,   120,   121,   200,   123,     0,   124,     0,     0,    11,
      12,    13,   112,     0,     0,     0,   127,   128,   129,   130,
      14,    15,     0,     0,     0,     0,     0,     0,    16,     0,
      17,   113,     0,    18,     0,     0,     0,   112,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,     0,   124,
       0,     0,     0,     0,     0,   125,   113,     0,   126,   127,
     128,   129,   130,   114,   115,   116,   117,   118,   119,   120,
     121,   203,   123,   191,   124,     0,     0,     0,     0,     0,
     204,     0,     0,   126,   127,   128,   129,   130,     0,     0,
       0,     0,   113,     0,     0,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,   121,     0,   123,     0,
     124,     0,     0,     0,     0,     0,     0,   113,     0,   126,
     127,   128,   129,   130,   114,   115,   116,   117,   118,   119,
     120,   121,     0,   123,     0,   124,     0,     0,     0,     0,
       0,     0,   113,     0,   126,   127,   128,   129,   130,   114,
     115,   116,   117,   118,   119,   120,   121,     0,   123,     0,
     124,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     127,   128,   129,   130,   114,   115,   116,   117,   118,   119,
     120,   121,     0,   123,     0,   124,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   129,   130,   114,
     115,   116,   117,   118,   119,   120,   121,     0,   123,     0,
     124
};

static const yytype_int16 yycheck[] =
{
       3,   125,     4,   178,   196,   160,    74,   297,    52,   156,
      29,     0,    31,    66,     3,    68,   272,    18,    46,   194,
       0,    65,     7,     3,    19,    20,    21,    22,    23,   319,
      74,   287,    29,    52,    31,   227,    28,   229,   162,     6,
      29,     8,    31,    46,   168,   200,    65,    36,    37,    52,
     197,   198,    74,    46,    56,    74,    36,    37,    80,    52,
      49,   253,    31,    74,    66,    63,    68,    48,    65,    80,
      71,    72,    73,    71,    75,    76,    45,    74,    79,    74,
     204,    70,   257,   275,   276,    80,    78,    79,    80,    81,
      70,    29,    74,    31,    97,    98,    99,   100,   101,   291,
      31,   103,   104,   105,   106,    98,    99,   100,   101,    18,
     112,   113,   236,   305,    74,    31,   263,     0,    31,    54,
       3,     0,   124,     3,   126,   127,   128,   129,   130,   251,
     132,    74,    45,   136,    80,    81,    12,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    12,    78,    79,    80,
      81,    27,   274,    36,    63,    31,   307,   308,    74,    74,
      69,    27,    71,   165,    80,    31,    63,     8,   290,   317,
     318,   322,    69,     6,    71,     8,     9,   325,    74,   327,
     328,    28,    63,    74,    31,    67,    67,    18,   190,   191,
      71,    72,    73,    74,    75,    76,    18,    18,    79,    80,
      71,    72,    73,    50,    75,    76,   209,    45,    48,   212,
      57,    58,    59,    60,    61,    62,    63,    64,    28,    66,
      35,    68,    18,    74,    39,    40,    41,    42,    74,    57,
      77,    78,    79,    80,    81,    51,    55,    18,    52,    74,
      50,    52,    74,    31,    74,    47,    49,    57,    58,    59,
      60,    61,    62,    63,    64,    28,    66,   260,    68,    52,
      28,    74,    18,    75,    71,    74,    74,    77,    78,    79,
      80,    81,    57,    74,    46,    18,    31,    50,    30,    74,
      28,     6,    74,    74,    57,    58,    59,    60,    61,    62,
      63,    64,    37,    66,    74,    68,   299,    28,    74,    18,
      74,    38,    50,    70,    77,    78,    79,    80,    81,    57,
      58,    59,    60,    61,    62,    63,    64,    28,    66,    31,
      68,    31,    72,    74,    71,    71,    31,    36,    28,    77,
      78,    79,    80,    81,    74,    74,    74,    28,    74,    50,
      74,    28,    74,    38,    52,    28,    57,    58,    59,    60,
      61,    62,    63,    64,    57,    66,    74,    68,    74,    74,
      31,    31,    18,   310,    71,    71,    77,    78,    79,    80,
      81,     4,     5,   245,   282,   220,    97,    10,    11,    12,
      13,    14,    15,    16,    17,    57,    58,    59,    60,    61,
      62,    63,    64,   157,    66,    -1,    68,    -1,    -1,    32,
      33,    34,    31,    -1,    -1,    -1,    78,    79,    80,    81,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,
      53,    50,    -1,    56,    -1,    -1,    -1,    31,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    -1,    68,
      -1,    -1,    -1,    -1,    -1,    74,    50,    -1,    77,    78,
      79,    80,    81,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    31,    68,    -1,    -1,    -1,    -1,    -1,
      74,    -1,    -1,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,    -1,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    -1,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,    77,
      78,    79,    80,    81,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    -1,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    -1,    77,    78,    79,    80,    81,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    -1,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    79,    80,    81,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    -1,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    -1,
      68
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,    10,    11,    12,    13,    14,    15,    16,
      17,    32,    33,    34,    43,    44,    51,    53,    56,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,     6,     8,     9,     6,     8,    18,    71,    72,    73,
      75,    76,    79,   124,   129,   130,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    63,    67,    74,    79,    80,
     107,   124,   125,   128,   132,   133,   134,   135,   136,    74,
       7,    46,    48,    74,    74,    54,    85,     0,     3,   137,
      74,    74,     8,    74,    74,   130,   130,    31,    78,    79,
      80,    81,   136,    18,    18,    18,    67,   136,    45,   136,
      48,    18,    31,    50,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    68,    74,    77,    78,    79,    80,
      81,   108,   131,    74,    74,    51,    57,    55,    18,    52,
      74,    52,    28,   129,   130,   130,   130,   130,    28,   136,
     136,   136,   136,    74,    80,    74,   109,    31,    74,    80,
     126,   127,   136,   136,    66,    68,    63,    71,    74,   136,
     108,   136,   136,   136,   136,   136,   136,    47,    49,   112,
      74,   124,    75,    74,   120,    74,    52,    74,    28,    28,
      31,    31,    29,    31,    52,    65,    74,   110,   111,   112,
     126,   127,    28,    65,    74,   108,   136,    71,   108,    18,
     134,   136,    57,    46,    35,    39,    40,    41,    42,   122,
      31,   119,    18,    74,   136,   136,    30,    74,   134,    74,
     110,   112,   112,    37,   113,   127,    74,   108,   124,   124,
       6,    18,    63,    69,    71,   120,    28,    74,    18,    28,
      28,    74,    65,    74,   110,   110,    38,    70,   115,   108,
      31,   123,    31,   106,    74,    72,   121,    71,    71,   119,
      31,   118,    74,    65,    74,   111,    74,   110,    74,   134,
      36,   116,   124,    28,    74,   112,    28,    74,    28,   118,
      74,   111,   110,   110,    31,    45,   114,    38,   123,    57,
      63,    69,    71,   118,    28,   111,   110,    74,    74,   128,
     124,    71,    71,   110,    45,   114,   114,    12,    27,    31,
     117,   106,    74,   117,   117,   128,   114,    12,    27,   117,
     117,   117
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    84,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    98,    99,   100,   101,
     102,   103,   104,   104,   104,   105,   106,   106,   107,   107,
     107,   108,   108,   108,   108,   109,   109,   109,   110,   110,
     110,   110,   110,   110,   110,   111,   111,   112,   112,   113,
     113,   113,   114,   114,   114,   115,   115,   116,   116,   116,
     116,   117,   117,   117,   117,   118,   118,   119,   119,   120,
     120,   120,   120,   120,   120,   120,   120,   121,   122,   122,
     122,   122,   122,   123,   123,   124,   124,   124,   124,   124,
     125,   125,   125,   125,   125,   126,   126,   127,   127,   127,
     128,   128,   128,   128,   128,   129,   129,   130,   130,   130,
     130,   130,   130,   130,   131,   131,   131,   131,   131,   131,
     131,   131,   132,   132,   133,   133,   133,   133,   133,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   135,   135,
     135,   135,   136,   136,   136,   136,   136,   137,   137
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     2,     7,     2,     4,     9,    10,     5,     7,     8,
       4,     8,     8,     6,     6,     2,     0,     5,     2,     3,
       4,     0,     3,     4,     5,     2,     3,     4,     0,     3,
       4,     5,     5,     6,     7,     0,     2,     0,     2,     0,
       4,     6,     0,     3,     5,     0,     2,     0,     4,     5,
       5,     0,     3,     4,     4,     0,     3,     0,     3,     5,
       6,     7,     7,     2,     3,     4,     4,     1,     1,     1,
       1,     1,     1,     0,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     3,
       1,     3,     1,     3,     4,     1,     3,     3,     3,     3,
       3,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     3,     3,     3,     3,     2,     3,
       3,     3,     3,     4,     3,     4,     2,     3,     4,     4,
       6,     6,     1,     1,     3,     1,     1,     0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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
        yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


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


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, sql_string, sql_result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, sql_string, sql_result, scanner);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
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
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), sql_string, sql_result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, sql_string, sql_result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
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
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
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
# endif
#endif

#ifndef yytnamerr
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
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
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
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
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
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
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


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
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
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
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
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
  *++yylsp = yylloc;

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

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* commands: command_wrapper opt_semicolon  */
#line 244 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 1946 "yacc_sql.cpp"
    break;

  case 23: /* exit_stmt: EXIT  */
#line 274 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 1955 "yacc_sql.cpp"
    break;

  case 24: /* help_stmt: HELP  */
#line 280 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 1963 "yacc_sql.cpp"
    break;

  case 25: /* sync_stmt: SYNC  */
#line 285 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 1971 "yacc_sql.cpp"
    break;

  case 26: /* begin_stmt: TRX_BEGIN  */
#line 291 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 1979 "yacc_sql.cpp"
    break;

  case 27: /* commit_stmt: TRX_COMMIT  */
#line 297 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 1987 "yacc_sql.cpp"
    break;

  case 28: /* rollback_stmt: TRX_ROLLBACK  */
#line 303 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 1995 "yacc_sql.cpp"
    break;

  case 29: /* drop_table_stmt: DROP TABLE ID  */
#line 309 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2005 "yacc_sql.cpp"
    break;

  case 30: /* show_tables_stmt: SHOW TABLES  */
#line 316 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2013 "yacc_sql.cpp"
    break;

  case 31: /* desc_table_stmt: DESC ID  */
#line 322 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2023 "yacc_sql.cpp"
    break;

  case 32: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 331 "yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
#line 2037 "yacc_sql.cpp"
    break;

  case 33: /* explain_stmt: EXPLAIN command_wrapper  */
#line 344 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 2046 "yacc_sql.cpp"
    break;

  case 34: /* set_variable_stmt: SET ID EQ value  */
#line 352 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
#line 2058 "yacc_sql.cpp"
    break;

  case 35: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE ID id_list RBRACE  */
#line 363 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-6].string);
      create_index.relation_name = (yyvsp[-4].string);
      create_index.is_unique = false;
      
      std::vector<std::string> *attribute_names = (yyvsp[-1].std_string_list);

      if (attribute_names != nullptr) {
        create_index.attribute_name.swap(*attribute_names);
      }

      create_index.attribute_name.emplace_back((yyvsp[-2].string));
      std::reverse(create_index.attribute_name.begin(), create_index.attribute_name.end());

      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
      free((yyvsp[-1].std_string_list));
    }
#line 2084 "yacc_sql.cpp"
    break;

  case 36: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE ID id_list RBRACE  */
#line 385 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-6].string);
      create_index.relation_name = (yyvsp[-4].string);
      create_index.is_unique = true;
      
      std::vector<std::string> *attribute_names = (yyvsp[-1].std_string_list);

      if (attribute_names != nullptr) {
        create_index.attribute_name.swap(*attribute_names);
      }

      create_index.attribute_name.emplace_back((yyvsp[-2].string));
      std::reverse(create_index.attribute_name.begin(), create_index.attribute_name.end());

      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
      free((yyvsp[-1].std_string_list));
    }
#line 2110 "yacc_sql.cpp"
    break;

  case 37: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 410 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2122 "yacc_sql.cpp"
    break;

  case 38: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE  */
#line 420 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-4].string);
      free((yyvsp[-4].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-1].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-2].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-2].attr_info);
    }
#line 2142 "yacc_sql.cpp"
    break;

  case 39: /* insert_stmt: INSERT INTO ID VALUES LBRACE value value_list RBRACE  */
#line 439 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-5].string);
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.sql_node)->insertion.values.swap(*(yyvsp[-1].value_list));
      }
      (yyval.sql_node)->insertion.values.emplace_back(*(yyvsp[-2].value));
      std::reverse((yyval.sql_node)->insertion.values.begin(), (yyval.sql_node)->insertion.values.end());
      delete (yyvsp[-2].value);
      free((yyvsp[-5].string));
    }
#line 2158 "yacc_sql.cpp"
    break;

  case 40: /* delete_stmt: DELETE FROM ID where  */
#line 454 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].string);
      if ((yyvsp[0].cond_pexpr) != nullptr) {
        (yyval.sql_node)->deletion.conditions.swap((yyvsp[0].cond_pexpr));
        delete (yyvsp[0].cond_pexpr);
      }
      free((yyvsp[-1].string));
    }
#line 2172 "yacc_sql.cpp"
    break;

  case 41: /* update_stmt: UPDATE ID SET ID EQ value assignment_list where  */
#line 467 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-6].string);

      if ((yyvsp[-1].assignment_list) != nullptr) {
        (yyval.sql_node)->update.assignments.swap(*(yyvsp[-1].assignment_list));
        delete (yyvsp[-1].assignment_list);
      }
      if ((yyvsp[0].cond_pexpr) != nullptr) {
        (yyval.sql_node)->update.conditions.swap((yyvsp[0].cond_pexpr));
        delete (yyvsp[0].cond_pexpr);
      }

      AssignmentSqlNode node = {
        .attribute_name = std::string{(yyvsp[-4].string)},
        .value = *(yyvsp[-2].value)
      };

      (yyval.sql_node)->update.assignments.push_back(std::move(node));
      std::reverse((yyval.sql_node)->update.assignments.begin(), (yyval.sql_node)->update.assignments.end());

      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
      free((yyvsp[-2].value));
    }
#line 2202 "yacc_sql.cpp"
    break;

  case 42: /* select_stmt: SELECT select_attr FROM select_from where group_by having order_by  */
#line 496 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-6].pexpr_list) != nullptr) {
        (yyval.sql_node)->selection.attributes.swap(*(yyvsp[-6].pexpr_list));
        delete (yyvsp[-6].pexpr_list);
        if (!IsAttributesVailid((yyval.sql_node)->selection.attributes)) {
          yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }
      // 还需要连接的一个表时relations
      (yyval.sql_node)->selection.relations.push_back((yyvsp[-4].from_node));
      if ((yyvsp[-3].cond_pexpr) != nullptr) {
        (yyval.sql_node)->selection.relations.insert((yyval.sql_node)->selection.relations.end(), (yyvsp[-3].cond_pexpr)->relations.begin(), (yyvsp[-3].cond_pexpr)->relations.end());
        (yyval.sql_node)->selection.join_conds.insert((yyval.sql_node)->selection.join_conds.end(), (yyvsp[-3].cond_pexpr)->join_conds.begin(), (yyvsp[-3].cond_pexpr)->join_conds.end());
        delete (yyvsp[-3].cond_pexpr);
      }
      if ((yyvsp[-2].rel_attr_list) != nullptr) {
        (yyval.sql_node)->selection.conditions = std::move((yyvsp[-2].rel_attr_list)->conditions);
        (yyval.sql_node)->selection.orderbys.swap((yyvsp[-2].rel_attr_list)->orderbys);
        delete (yyvsp[-2].rel_attr_list);
      }
      delete (yyvsp[-4].from_node);
    }
#line 2230 "yacc_sql.cpp"
    break;

  case 43: /* select_stmt: SELECT select_attr FROM ID inner_join where  */
#line 520 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-4].pexpr_list) != nullptr) {
        (yyval.sql_node)->selection.attributes.swap(*(yyvsp[-4].pexpr_list));
        delete (yyvsp[-4].pexpr_list);
        if (!IsAttributesVailid((yyval.sql_node)->selection.attributes)) {
          yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }
      // 还需要连接的一个表时relations
      (yyval.sql_node)->selection.relations.push_back((yyvsp[-2].string));
      if ((yyvsp[-1].cond_pexpr) != nullptr) {
        (yyval.sql_node)->selection.relations.insert((yyval.sql_node)->selection.relations.end(), (yyvsp[-1].cond_pexpr)->relations.begin(), (yyvsp[-1].cond_pexpr)->relations.end());
        (yyval.sql_node)->selection.join_conds.insert((yyval.sql_node)->selection.join_conds.end(), (yyvsp[-1].cond_pexpr)->join_conds.begin(), (yyvsp[-1].cond_pexpr)->join_conds.end());
        delete (yyvsp[-1].cond_pexpr);
      }
      if ((yyvsp[0].cond_pexpr) != nullptr) {
        (yyval.sql_node)->selection.conditions = std::move((yyvsp[0].cond_pexpr)->conditions);
        (yyval.sql_node)->selection.orderbys.swap((yyvsp[0].cond_pexpr)->orderbys);
        delete (yyvsp[0].cond_pexpr);
      }
      free((yyvsp[-2].string));
    }
#line 2258 "yacc_sql.cpp"
    break;

  case 44: /* select_stmt: SELECT select_attr FROM ID from_list where  */
#line 544 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-4].pexpr_list) != nullptr) {
        (yyval.sql_node)->selection.attributes.swap(*(yyvsp[-4].pexpr_list));
        delete (yyvsp[-4].pexpr_list);
        if (!IsAttributesVailid((yyval.sql_node)->selection.attributes)) {
          yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }
      if ((yyvsp[-1].from_node) != nullptr) {
        (yyval.sql_node)->selection.relations.swap(*(yyvsp[-1].from_node));
        delete (yyvsp[-1].from_node);
      }
      (yyval.sql_node)->selection.relations.push_back((yyvsp[-2].string));
      std::reverse((yyval.sql_node)->selection.relations.begin(), (yyval.sql_node)->selection.relations.end());

      // TODO(oldcb): 直接传进来, 不用一个个移动了
      if ((yyvsp[0].cond_pexpr) != nullptr) {
        (yyval.sql_node)->selection.conditions = std::move((yyvsp[0].cond_pexpr)->conditions);
        (yyval.sql_node)->selection.orderbys.swap((yyvsp[0].cond_pexpr)->orderbys);
        delete (yyvsp[0].cond_pexpr);
      }
      free((yyvsp[-2].string));
    }
#line 2287 "yacc_sql.cpp"
    break;

  case 45: /* calc_stmt: CALC expression_list  */
#line 572 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2298 "yacc_sql.cpp"
    break;

  case 46: /* assignment_list: %empty  */
#line 584 "yacc_sql.y"
    {
      (yyval.assignment_list) = nullptr;
    }
#line 2306 "yacc_sql.cpp"
    break;

  case 47: /* assignment_list: COMMA ID EQ value assignment_list  */
#line 588 "yacc_sql.y"
    {
      if ((yyvsp[0].assignment_list) != nullptr) {
        (yyval.assignment_list) = (yyvsp[0].assignment_list);
      } else {
        (yyval.assignment_list) = new std::vector<AssignmentSqlNode>;
      }

      AssignmentSqlNode node = {
        .attribute_name = std::string{(yyvsp[-3].string)},
        .value = *(yyvsp[-1].value)
      };

      (yyval.assignment_list)->push_back(std::move(node));
      free((yyvsp[-3].string));
      delete (yyvsp[-1].value);
    }
#line 2327 "yacc_sql.cpp"
    break;

  case 48: /* select_attr: pexpr select_attr_list  */
#line 607 "yacc_sql.y"
                           {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      (yyval.pexpr_list)->push_back((yyvsp[-1].pexpr));
      std::reverse((yyval.pexpr_list)->begin(), (yyval.pexpr_list)->end());
    }
#line 2341 "yacc_sql.cpp"
    break;

  case 49: /* select_attr: pexpr ID select_attr_list  */
#line 616 "yacc_sql.y"
                                {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      PExpr * pexpr = (yyvsp[-2].pexpr);
      pexpr->alias = (yyvsp[-1].string);
      (yyval.pexpr_list)->push_back(pexpr);
      std::reverse((yyval.pexpr_list)->begin(), (yyval.pexpr_list)->end());
      free((yyvsp[-1].string));
    }
#line 2358 "yacc_sql.cpp"
    break;

  case 50: /* select_attr: pexpr AS ID select_attr_list  */
#line 628 "yacc_sql.y"
                                   {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      PExpr * pexpr = (yyvsp[-3].pexpr);
      pexpr->alias = (yyvsp[-1].string);
      (yyval.pexpr_list)->push_back(pexpr);
      std::reverse((yyval.pexpr_list)->begin(), (yyval.pexpr_list)->end());
      free((yyvsp[-1].string));
    }
#line 2375 "yacc_sql.cpp"
    break;

  case 51: /* select_attr_list: %empty  */
#line 643 "yacc_sql.y"
    {
      (yyval.pexpr_list) = nullptr;
    }
#line 2383 "yacc_sql.cpp"
    break;

  case 52: /* select_attr_list: COMMA pexpr select_attr_list  */
#line 646 "yacc_sql.y"
                                   {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      (yyval.pexpr_list)->push_back((yyvsp[-1].pexpr));
    }
#line 2396 "yacc_sql.cpp"
    break;

  case 53: /* select_attr_list: COMMA pexpr ID select_attr_list  */
#line 654 "yacc_sql.y"
                                      {
      if ((yyvsp[-1].string) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[-1].string);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      PExpr * pexpr = (yyvsp[-2].pexpr);
      pexpr->alias = (yyvsp[-1].string);
      (yyval.pexpr_list)->push_back(pexpr);
      free((yyvsp[-1].string));
    }
#line 2412 "yacc_sql.cpp"
    break;

  case 54: /* select_attr_list: COMMA pexpr AS ID select_attr_list  */
#line 665 "yacc_sql.y"
                                         {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      PExpr * pexpr = (yyvsp[-3].pexpr);
      pexpr->alias = (yyvsp[-1].string);
      (yyval.pexpr_list)->push_back(pexpr);
      free((yyvsp[-1].string));
    }
#line 2428 "yacc_sql.cpp"
    break;

  case 55: /* select_from: ID from_list  */
#line 679 "yacc_sql.y"
                 {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-1].string);
      (yyval.from_node)->relations.push_back(std::move(rel));
      std::reverse((yyval.from_node)->begin(), (yyval.from_node)->end());
      free((yyvsp[-1].string));
    }
#line 2445 "yacc_sql.cpp"
    break;

  case 56: /* select_from: ID ID from_list  */
#line 691 "yacc_sql.y"
                      {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-2].string);
      rel.alias = (yyvsp[-1].string);
      (yyval.from_node)->relations.push_back(std::move(rel));
      std::reverse((yyval.from_node)->begin(), (yyval.from_node)->end());
      free((yyvsp[-2].string));
      free((yyvsp[-1].string));
    }
#line 2464 "yacc_sql.cpp"
    break;

  case 57: /* select_from: ID AS ID from_list  */
#line 705 "yacc_sql.y"
                         {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-3].string);
      rel.alias = (yyvsp[-1].string);
      (yyval.from_node)->relations.push_back(std::move(rel));
      std::reverse((yyval.from_node)->begin(), (yyval.from_node)->end());
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2483 "yacc_sql.cpp"
    break;

  case 58: /* from_list: %empty  */
#line 721 "yacc_sql.y"
    {
      (yyval.from_node) = nullptr;
    }
#line 2491 "yacc_sql.cpp"
    break;

  case 59: /* from_list: COMMA ID from_list  */
#line 724 "yacc_sql.y"
                         {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-1].string);
      (yyval.from_node)->relations.push_back(rel);
      free((yyvsp[-1].string));
    }
#line 2507 "yacc_sql.cpp"
    break;

  case 60: /* from_list: COMMA ID ID from_list  */
#line 735 "yacc_sql.y"
                            {
      if ((yyvsp[-1].string) != nullptr) {
        (yyval.from_node) = (yyvsp[-1].string);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-2].string);
      rel.alias = (yyvsp[-1].string);
      (yyval.from_node)->relations.push_back(rel);
      free((yyvsp[-2].string));
      free((yyvsp[-1].string));
    }
#line 2525 "yacc_sql.cpp"
    break;

  case 61: /* from_list: COMMA ID AS ID from_list  */
#line 748 "yacc_sql.y"
                               {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-3].string);
      rel.alias = (yyvsp[-1].string);
      (yyval.from_node)->relations.push_back(rel);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2543 "yacc_sql.cpp"
    break;

  case 62: /* from_list: INNER JOIN ID inner_join from_list  */
#line 761 "yacc_sql.y"
                                         {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-2].string);
      (yyval.from_node)->relations.push_back(rel);
      (yyval.from_node)->join_conds.push_back((yyvsp[-1].cond_pexpr))
      free((yyvsp[-2].string));
    }
#line 2560 "yacc_sql.cpp"
    break;

  case 63: /* from_list: INNER JOIN ID ID inner_join from_list  */
#line 773 "yacc_sql.y"
                                            {
      if ((yyvsp[-1].cond_pexpr) != nullptr) {
        (yyval.from_node) = (yyvsp[-1].cond_pexpr);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-3].string);
      rel.alias = (yyvsp[-2].string);
      (yyval.from_node)->relations.push_back(rel);
      (yyval.from_node)->join_conds.push_back((yyvsp[-1].cond_pexpr))
      free((yyvsp[-3].string));
      free((yyvsp[-2].string));
    }
#line 2579 "yacc_sql.cpp"
    break;

  case 64: /* from_list: INNER JOIN ID AS ID inner_join from_list  */
#line 787 "yacc_sql.y"
                                               {
      if ((yyvsp[-2].string) != nullptr) {
        (yyval.from_node) = (yyvsp[-2].string);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-4].string);
      rel.alias = (yyvsp[-2].string);
      (yyval.from_node)->relations.push_back(rel);
      (yyval.from_node)->join_conds.push_back((yyvsp[-1].cond_pexpr))
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
    }
#line 2598 "yacc_sql.cpp"
    break;

  case 65: /* inner_join: %empty  */
#line 804 "yacc_sql.y"
    {
      (yyval.cond_pexpr) = nullptr;
    }
#line 2606 "yacc_sql.cpp"
    break;

  case 66: /* inner_join: ON cond_pexpr  */
#line 807 "yacc_sql.y"
                   {
      (yyval.cond_pexpr) = (yyvsp[0].cond_pexpr);
    }
#line 2614 "yacc_sql.cpp"
    break;

  case 67: /* where: %empty  */
#line 815 "yacc_sql.y"
    {
      (yyval.cond_pexpr) = nullptr;
    }
#line 2622 "yacc_sql.cpp"
    break;

  case 68: /* where: WHERE cond_pexpr  */
#line 818 "yacc_sql.y"
                       {
      (yyval.cond_pexpr) = (yyvsp[0].cond_pexpr);  
    }
#line 2630 "yacc_sql.cpp"
    break;

  case 69: /* group_by: %empty  */
#line 824 "yacc_sql.y"
    {
      (yyval.rel_attr_list) = nullptr;
    }
#line 2638 "yacc_sql.cpp"
    break;

  case 70: /* group_by: GROUP BY ID group_by_list  */
#line 827 "yacc_sql.y"
                                {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr_list)->push_back(rel_attr);
      std::reverse((yyval.rel_attr_list)->begin(), (yyval.rel_attr_list)->end());
      free((yyvsp[-1].string));
    }
#line 2655 "yacc_sql.cpp"
    break;

  case 71: /* group_by: GROUP BY ID DOT ID group_by_list  */
#line 839 "yacc_sql.y"
                                       {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.relation_name = (yyvsp[-3].string);
      rel_attr.attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr_list)->push_back(rel_attr);
      std::reverse((yyval.rel_attr_list)->begin(), (yyval.rel_attr_list)->end());
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2674 "yacc_sql.cpp"
    break;

  case 72: /* group_by_list: %empty  */
#line 856 "yacc_sql.y"
    { 
      (yyval.rel_attr_list) = nullptr;
    }
#line 2682 "yacc_sql.cpp"
    break;

  case 73: /* group_by_list: COMMA ID group_by_list  */
#line 859 "yacc_sql.y"
                             {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.attribute_name = (yyvsp[-1].string);
      rel_attr.relation_name = (yyvsp[-1].string);
      (yyval.rel_attr_list)->push_back(rerel_attrl);
      free((yyvsp[-1].string));
    }
#line 2699 "yacc_sql.cpp"
    break;

  case 74: /* group_by_list: COMMA ID DOT ID group_by_list  */
#line 871 "yacc_sql.y"
                                    {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.relation_name = (yyvsp[-3].string);
      rel_attr.attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr_list)->push_back(rel_attr);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2717 "yacc_sql.cpp"
    break;

  case 75: /* having: %empty  */
#line 888 "yacc_sql.y"
    {
      (yyval.cond_pexpr) = nullptr;
    }
#line 2725 "yacc_sql.cpp"
    break;

  case 76: /* having: HAVING cond_pexpr  */
#line 891 "yacc_sql.y"
                        {
      (yyval.cond_pexpr) = (yyvsp[0].cond_pexpr);  
    }
#line 2733 "yacc_sql.cpp"
    break;

  case 77: /* order_by: %empty  */
#line 898 "yacc_sql.y"
    {
      (yyval.orderby_list) = nullptr;
    }
#line 2741 "yacc_sql.cpp"
    break;

  case 78: /* order_by: ORDER BY rel_attr order_condtions  */
#line 901 "yacc_sql.y"
                                        {
      if ((yyvsp[0].orderby_list) != nullptr) {
        (yyval.orderby_list) = (yyvsp[0].orderby_list);
      } else {
        (yyval.orderby_list) = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *(yyvsp[-1].rel_attr);
      order.sort_type = SortType::ASC;
      (yyval.orderby_list)->push_back(order);
      std::reverse((yyval.orderby_list)->begin(), (yyval.orderby_list)->end());
      delete((yyvsp[-1].rel_attr));
    }
#line 2759 "yacc_sql.cpp"
    break;

  case 79: /* order_by: ORDER BY rel_attr DESC order_condtions  */
#line 914 "yacc_sql.y"
                                             {
      if ((yyvsp[0].orderby_list) != nullptr) {
        (yyval.orderby_list) = (yyvsp[0].orderby_list);
      } else {
        (yyval.orderby_list) = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *(yyvsp[-2].rel_attr);
      order.sort_type = SortType::DESC;
      (yyval.orderby_list)->push_back(order);
      std::reverse((yyval.orderby_list)->begin(), (yyval.orderby_list)->end());
      delete((yyvsp[-2].rel_attr));
    }
#line 2777 "yacc_sql.cpp"
    break;

  case 80: /* order_by: ORDER BY rel_attr ASC order_condtions  */
#line 927 "yacc_sql.y"
                                            {
      if ((yyvsp[0].orderby_list) != nullptr) {
        (yyval.orderby_list) = (yyvsp[0].orderby_list);
      } else {
        (yyval.orderby_list) = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *(yyvsp[-2].rel_attr);
      order.sort_type = SortType::ASC;
      (yyval.orderby_list)->push_back(order);
      std::reverse((yyval.orderby_list)->begin(), (yyval.orderby_list)->end());
      delete((yyvsp[-2].rel_attr));
    }
#line 2795 "yacc_sql.cpp"
    break;

  case 81: /* order_condtions: %empty  */
#line 943 "yacc_sql.y"
    {
      (yyval.orderby_list) = nullptr;
    }
#line 2803 "yacc_sql.cpp"
    break;

  case 82: /* order_condtions: COMMA rel_attr order_condtions  */
#line 946 "yacc_sql.y"
                                     {
      if ((yyvsp[0].orderby_list) != nullptr) {
        (yyval.orderby_list) = (yyvsp[0].orderby_list);
      } else {
        (yyval.orderby_list) = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *(yyvsp[-1].rel_attr);
      order.sort_type = SortType::ASC;
      (yyval.orderby_list)->push_back(order);
      delete((yyvsp[-1].rel_attr));
    }
#line 2820 "yacc_sql.cpp"
    break;

  case 83: /* order_condtions: COMMA rel_attr DESC order_condtions  */
#line 958 "yacc_sql.y"
                                          {
      if ((yyvsp[0].orderby_list) != nullptr) {
        (yyval.orderby_list) = (yyvsp[0].orderby_list);
      } else {
        (yyval.orderby_list) = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *(yyvsp[-2].rel_attr);
      order.sort_type = SortType::DESC;
      (yyval.orderby_list)->push_back(order);
      delete((yyvsp[-2].rel_attr));
    }
#line 2837 "yacc_sql.cpp"
    break;

  case 84: /* order_condtions: COMMA rel_attr ASC order_condtions  */
#line 970 "yacc_sql.y"
                                         {
      if ((yyvsp[0].orderby_list) != nullptr) {
        (yyval.orderby_list) = (yyvsp[0].orderby_list);
      } else {
        (yyval.orderby_list) = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *(yyvsp[-2].rel_attr);
      order.sort_type = SortType::ASC;
      (yyval.orderby_list)->push_back(order);
      delete((yyvsp[-2].rel_attr));
    }
#line 2854 "yacc_sql.cpp"
    break;

  case 85: /* id_list: %empty  */
#line 987 "yacc_sql.y"
    {
      (yyval.std_string_list) = nullptr;
    }
#line 2862 "yacc_sql.cpp"
    break;

  case 86: /* id_list: COMMA ID id_list  */
#line 991 "yacc_sql.y"
    {
      if ((yyvsp[0].std_string_list) != nullptr) {
        (yyval.std_string_list) = (yyvsp[0].std_string_list);
      } else {
        (yyval.std_string_list) = new std::vector<std::string>;
      }
      (yyval.std_string_list)->emplace_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 2876 "yacc_sql.cpp"
    break;

  case 87: /* attr_def_list: %empty  */
#line 1004 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
#line 2884 "yacc_sql.cpp"
    break;

  case 88: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 1008 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
#line 2898 "yacc_sql.cpp"
    break;

  case 89: /* attr_def: ID type LBRACE number RBRACE  */
#line 1021 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].string);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      (yyval.attr_info)->is_not_null = false;
      free((yyvsp[-4].string));
    }
#line 2911 "yacc_sql.cpp"
    break;

  case 90: /* attr_def: ID type LBRACE number RBRACE NULL_T  */
#line 1030 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].string);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->is_not_null = false;
      free((yyvsp[-5].string));
    }
#line 2924 "yacc_sql.cpp"
    break;

  case 91: /* attr_def: ID type LBRACE number RBRACE DEFAULT NULL_T  */
#line 1039 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-5].number);
      (yyval.attr_info)->name = (yyvsp[-6].string);
      (yyval.attr_info)->length = (yyvsp[-3].number);
      (yyval.attr_info)->is_not_null = false;
      free((yyvsp[-6].string));
    }
#line 2937 "yacc_sql.cpp"
    break;

  case 92: /* attr_def: ID type LBRACE number RBRACE NOT NULL_T  */
#line 1048 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-5].number);
      (yyval.attr_info)->name = (yyvsp[-6].string);
      (yyval.attr_info)->length = (yyvsp[-3].number);
      (yyval.attr_info)->is_not_null = true;
      free((yyvsp[-6].string));
    }
#line 2950 "yacc_sql.cpp"
    break;

  case 93: /* attr_def: ID type  */
#line 1057 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].string);
      (yyval.attr_info)->is_not_null = false;
      if((yyval.attr_info)->type == AttrType::TEXTS)
      {
        (yyval.attr_info)->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        (yyval.attr_info)->length = 4;
      }
      free((yyvsp[-1].string));
    }
#line 2969 "yacc_sql.cpp"
    break;

  case 94: /* attr_def: ID type NULL_T  */
#line 1072 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].string);
      (yyval.attr_info)->is_not_null = false;
      if((yyval.attr_info)->type == AttrType::TEXTS)
      {
        (yyval.attr_info)->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        (yyval.attr_info)->length = 4;
      }
      free((yyvsp[-2].string));
    }
#line 2988 "yacc_sql.cpp"
    break;

  case 95: /* attr_def: ID type DEFAULT NULL_T  */
#line 1087 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-2].number);
      (yyval.attr_info)->name = (yyvsp[-3].string);
      (yyval.attr_info)->is_not_null = false;
      if((yyval.attr_info)->type == AttrType::TEXTS)
      {
        (yyval.attr_info)->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        (yyval.attr_info)->length = 4;
      }
      free((yyvsp[-3].string));
    }
#line 3007 "yacc_sql.cpp"
    break;

  case 96: /* attr_def: ID type NOT NULL_T  */
#line 1102 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-2].number);
      (yyval.attr_info)->name = (yyvsp[-3].string);
      (yyval.attr_info)->is_not_null = true;
      if((yyval.attr_info)->type == AttrType::TEXTS)
      {
        (yyval.attr_info)->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        (yyval.attr_info)->length = 4;
      }
      free((yyvsp[-3].string));
    }
#line 3026 "yacc_sql.cpp"
    break;

  case 97: /* number: NUMBER  */
#line 1118 "yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 3032 "yacc_sql.cpp"
    break;

  case 98: /* type: INT_T  */
#line 1121 "yacc_sql.y"
               { (yyval.number)=AttrType::INTS; }
#line 3038 "yacc_sql.cpp"
    break;

  case 99: /* type: STRING_T  */
#line 1122 "yacc_sql.y"
               { (yyval.number)=AttrType::CHARS; }
#line 3044 "yacc_sql.cpp"
    break;

  case 100: /* type: TEXT_T  */
#line 1123 "yacc_sql.y"
               { (yyval.number)=AttrType::TEXTS; }
#line 3050 "yacc_sql.cpp"
    break;

  case 101: /* type: FLOAT_T  */
#line 1124 "yacc_sql.y"
               { (yyval.number)=AttrType::FLOATS; }
#line 3056 "yacc_sql.cpp"
    break;

  case 102: /* type: DATE_T  */
#line 1125 "yacc_sql.y"
               { (yyval.number)=AttrType::DATES; }
#line 3062 "yacc_sql.cpp"
    break;

  case 103: /* value_list: %empty  */
#line 1131 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
#line 3070 "yacc_sql.cpp"
    break;

  case 104: /* value_list: COMMA value value_list  */
#line 1134 "yacc_sql.y"
                              { 
      if ((yyvsp[0].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[0].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(*(yyvsp[-1].value));
      delete (yyvsp[-1].value);
    }
#line 3084 "yacc_sql.cpp"
    break;

  case 105: /* value: NUMBER  */
#line 1145 "yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 3093 "yacc_sql.cpp"
    break;

  case 106: /* value: FLOAT  */
#line 1149 "yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 3102 "yacc_sql.cpp"
    break;

  case 107: /* value: SSS  */
#line 1153 "yacc_sql.y"
         {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      if(strlen(tmp) > 65535){
        yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid text", SCF_INVALID);
      }
      (yyval.value) = new Value(tmp);
      free(tmp);
    }
#line 3115 "yacc_sql.cpp"
    break;

  case 108: /* value: NULL_T  */
#line 1161 "yacc_sql.y"
            {
      (yyval.value) = new Value(AttrType::NULLS);
      (yyloc) = (yylsp[0]);
    }
#line 3124 "yacc_sql.cpp"
    break;

  case 109: /* value: DATE  */
#line 1165 "yacc_sql.y"
          {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value(tmp, DATES);
      free(tmp);
      if(!(yyval.value)->check_date()){
        yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid date", SCF_INVALID);
      }
    }
#line 3137 "yacc_sql.cpp"
    break;

  case 110: /* aggr_func_type: AGGR_MAX  */
#line 1177 "yacc_sql.y"
             { (yyval.aggr_func_type) = AggrFuncType::MAX; }
#line 3143 "yacc_sql.cpp"
    break;

  case 111: /* aggr_func_type: AGGR_MIN  */
#line 1178 "yacc_sql.y"
               { (yyval.aggr_func_type) = AggrFuncType::MIN; }
#line 3149 "yacc_sql.cpp"
    break;

  case 112: /* aggr_func_type: AGGR_SUM  */
#line 1179 "yacc_sql.y"
               { (yyval.aggr_func_type) = AggrFuncType::SUM; }
#line 3155 "yacc_sql.cpp"
    break;

  case 113: /* aggr_func_type: AGGR_AVG  */
#line 1180 "yacc_sql.y"
               { (yyval.aggr_func_type) = AggrFuncType::AVG; }
#line 3161 "yacc_sql.cpp"
    break;

  case 114: /* aggr_func_type: AGGR_COUNT  */
#line 1181 "yacc_sql.y"
                 { (yyval.aggr_func_type) = AggrFuncType::COUNT; }
#line 3167 "yacc_sql.cpp"
    break;

  case 115: /* field_or_star: '*'  */
#line 1184 "yacc_sql.y"
      {
    (yyval.string) = "*";
  }
#line 3175 "yacc_sql.cpp"
    break;

  case 116: /* field_or_star: ID  */
#line 1187 "yacc_sql.y"
       {
    (yyval.string) = (yyvsp[0].string);
  }
#line 3183 "yacc_sql.cpp"
    break;

  case 117: /* field_or_star_list: %empty  */
#line 1193 "yacc_sql.y"
    {
      (yyval.std_string_list) = nullptr;
    }
#line 3191 "yacc_sql.cpp"
    break;

  case 118: /* field_or_star_list: field_or_star field_or_star_list  */
#line 1196 "yacc_sql.y"
                                       {
      (yyval.std_string_list) = new std::vector<std::string>;
      (yyval.std_string_list)->emplace_back((yyvsp[-1].string));
      if ((yyvsp[0].std_string_list) != nullptr) {
        (yyval.std_string_list)->insert((yyval.std_string_list)->end(), (yyvsp[0].std_string_list)->begin(), (yyvsp[0].std_string_list)->end());
      }
    }
#line 3203 "yacc_sql.cpp"
    break;

  case 119: /* field_or_star_list: COMMA field_or_star field_or_star_list  */
#line 1203 "yacc_sql.y"
                                             {
      if ((yyvsp[0].std_string_list) != nullptr) {
        (yyval.std_string_list) = (yyvsp[0].std_string_list);
      } else {
        (yyval.std_string_list) = new std::vector<std::string>;
      }
      (yyval.std_string_list)->emplace_back((yyvsp[-1].string));
    }
#line 3216 "yacc_sql.cpp"
    break;

  case 120: /* rel_attr: '*'  */
#line 1214 "yacc_sql.y"
        {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = "";
      (yyval.rel_attr)->attribute_name = "*";
    }
#line 3226 "yacc_sql.cpp"
    break;

  case 121: /* rel_attr: ID DOT '*'  */
#line 1219 "yacc_sql.y"
                 {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = "*";
      free((yyvsp[-2].string));
    }
#line 3237 "yacc_sql.cpp"
    break;

  case 122: /* rel_attr: ID  */
#line 1225 "yacc_sql.y"
         {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 3247 "yacc_sql.cpp"
    break;

  case 123: /* rel_attr: ID DOT ID  */
#line 1230 "yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 3259 "yacc_sql.cpp"
    break;

  case 124: /* rel_attr: aggr_func_type LBRACE field_or_star_list RBRACE  */
#line 1237 "yacc_sql.y"
                                                      {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->aggr_type = (yyvsp[-3].aggr_func_type);
      if ((yyvsp[-1].std_string_list) != nullptr) {
        (yyval.rel_attr)->aggregates = *(yyvsp[-1].std_string_list);
        delete (yyvsp[-1].std_string_list);
      }
    }
#line 3272 "yacc_sql.cpp"
    break;

  case 125: /* expression_list: expression  */
#line 1249 "yacc_sql.y"
    {
      (yyval.expression_list) = new std::vector<Expression*>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 3281 "yacc_sql.cpp"
    break;

  case 126: /* expression_list: expression COMMA expression_list  */
#line 1254 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 3294 "yacc_sql.cpp"
    break;

  case 127: /* expression: expression '+' expression  */
#line 1265 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3302 "yacc_sql.cpp"
    break;

  case 128: /* expression: expression '-' expression  */
#line 1268 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3310 "yacc_sql.cpp"
    break;

  case 129: /* expression: expression '*' expression  */
#line 1271 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3318 "yacc_sql.cpp"
    break;

  case 130: /* expression: expression '/' expression  */
#line 1274 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3326 "yacc_sql.cpp"
    break;

  case 131: /* expression: LBRACE expression RBRACE  */
#line 1277 "yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 3335 "yacc_sql.cpp"
    break;

  case 132: /* expression: '-' expression  */
#line 1281 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 3343 "yacc_sql.cpp"
    break;

  case 133: /* expression: value  */
#line 1284 "yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 3353 "yacc_sql.cpp"
    break;

  case 134: /* comp_op: EQ  */
#line 1292 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 3359 "yacc_sql.cpp"
    break;

  case 135: /* comp_op: LT  */
#line 1293 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 3365 "yacc_sql.cpp"
    break;

  case 136: /* comp_op: GT  */
#line 1294 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 3371 "yacc_sql.cpp"
    break;

  case 137: /* comp_op: LE  */
#line 1295 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 3377 "yacc_sql.cpp"
    break;

  case 138: /* comp_op: GE  */
#line 1296 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 3383 "yacc_sql.cpp"
    break;

  case 139: /* comp_op: NE  */
#line 1297 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 3389 "yacc_sql.cpp"
    break;

  case 140: /* comp_op: LIKE  */
#line 1298 "yacc_sql.y"
           { (yyval.comp) = LIKE; }
#line 3395 "yacc_sql.cpp"
    break;

  case 141: /* comp_op: NOT LIKE  */
#line 1299 "yacc_sql.y"
               { (yyval.comp) = NOT_LIKE; }
#line 3401 "yacc_sql.cpp"
    break;

  case 142: /* unary_pexpr: value  */
#line 1304 "yacc_sql.y"
        {
    PUnaryExpr *unary_pexpr = new PUnaryExpr;
    unary_pexpr->is_attr = false;
    unary_pexpr->value = *(yyvsp[0].value);
    (yyval.unary_pexpr) = unary_pexpr;
    delete (yyvsp[0].value);
  }
#line 3413 "yacc_sql.cpp"
    break;

  case 143: /* unary_pexpr: rel_attr  */
#line 1311 "yacc_sql.y"
             {
    PUnaryExpr *unary_pexpr = new PUnaryExpr;
    unary_pexpr->is_attr = true;
    unary_pexpr->attr = *(yyvsp[0].rel_attr);
    (yyval.unary_pexpr) = unary_pexpr;
    delete (yyvsp[0].rel_attr);
  }
#line 3425 "yacc_sql.cpp"
    break;

  case 144: /* arith_pexpr: pexpr '+' pexpr  */
#line 1320 "yacc_sql.y"
                    {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::ADD, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
        (yyval.arith_pexpr) = arith_pexpr;
    }
#line 3434 "yacc_sql.cpp"
    break;

  case 145: /* arith_pexpr: pexpr '-' pexpr  */
#line 1324 "yacc_sql.y"
                      {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::SUB, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
        (yyval.arith_pexpr) = arith_pexpr;
    }
#line 3443 "yacc_sql.cpp"
    break;

  case 146: /* arith_pexpr: pexpr '*' pexpr  */
#line 1328 "yacc_sql.y"
                      {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::MUL, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
        (yyval.arith_pexpr) = arith_pexpr;
    }
#line 3452 "yacc_sql.cpp"
    break;

  case 147: /* arith_pexpr: pexpr '/' pexpr  */
#line 1332 "yacc_sql.y"
                      {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::DIV, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
        (yyval.arith_pexpr) = arith_pexpr;
    }
#line 3461 "yacc_sql.cpp"
    break;

  case 148: /* arith_pexpr: '-' pexpr  */
#line 1336 "yacc_sql.y"
                             {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::NEGATIVE, (yyvsp[0].pexpr), nullptr);
        (yyval.arith_pexpr) = arith_pexpr;
    }
#line 3470 "yacc_sql.cpp"
    break;

  case 149: /* cond_pexpr: pexpr OR pexpr  */
#line 1343 "yacc_sql.y"
                  {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::OR, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3479 "yacc_sql.cpp"
    break;

  case 150: /* cond_pexpr: pexpr AND pexpr  */
#line 1347 "yacc_sql.y"
                     {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::AND, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3488 "yacc_sql.cpp"
    break;

  case 151: /* cond_pexpr: pexpr comp_op pexpr  */
#line 1352 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(comp_op, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3497 "yacc_sql.cpp"
    break;

  case 152: /* cond_pexpr: pexpr IS NULL_T  */
#line 1357 "yacc_sql.y"
    {
      PUnaryExpr *unary_pexpr = new PUnaryExpr;
      unary_pexpr->is_attr = false;
      unary_pexpr->value = Value(AttrType::NULLS);
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IS_NULL, (yyvsp[-2].pexpr), unary_pexpr);
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3509 "yacc_sql.cpp"
    break;

  case 153: /* cond_pexpr: pexpr IS NOT NULL_T  */
#line 1365 "yacc_sql.y"
    {
      PUnaryExpr *unary_pexpr = new PUnaryExpr;
      unary_pexpr->is_attr = false;
      unary_pexpr->value = Value(AttrType::NULLS);
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IS_NOT_NULL, (yyvsp[-3].pexpr), unary_pexpr);
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3521 "yacc_sql.cpp"
    break;

  case 154: /* cond_pexpr: pexpr IN pexpr  */
#line 1372 "yacc_sql.y"
                    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IN, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3530 "yacc_sql.cpp"
    break;

  case 155: /* cond_pexpr: pexpr NOT IN pexpr  */
#line 1376 "yacc_sql.y"
                        {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_IN, (yyvsp[-3].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3539 "yacc_sql.cpp"
    break;

  case 156: /* cond_pexpr: EXISTS pexpr  */
#line 1380 "yacc_sql.y"
                  {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::EXISTS, nullptr, (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3548 "yacc_sql.cpp"
    break;

  case 157: /* cond_pexpr: NOT EXISTS pexpr  */
#line 1384 "yacc_sql.y"
                      {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_EXISTS, nullptr, (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3557 "yacc_sql.cpp"
    break;

  case 158: /* func_pexpr: LENGTH LBRACE pexpr RBRACE  */
#line 1391 "yacc_sql.y"
                               {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::LENGTH;
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3568 "yacc_sql.cpp"
    break;

  case 159: /* func_pexpr: ROUND LBRACE pexpr RBRACE  */
#line 1398 "yacc_sql.y"
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::ROUND;
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3579 "yacc_sql.cpp"
    break;

  case 160: /* func_pexpr: ROUND LBRACE pexpr COMMA pexpr RBRACE  */
#line 1405 "yacc_sql.y"
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::ROUND;
      func_pexpr->params.push_back((yyvsp[-3].pexpr));
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3591 "yacc_sql.cpp"
    break;

  case 161: /* func_pexpr: DATE_FORMAT LBRACE pexpr COMMA pexpr RBRACE  */
#line 1413 "yacc_sql.y"
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::DATE_FORMAT;
      func_pexpr->params.push_back((yyvsp[-3].pexpr));
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3603 "yacc_sql.cpp"
    break;

  case 162: /* pexpr: cond_pexpr  */
#line 1423 "yacc_sql.y"
               {
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::COMPARISON;
      pexpr->cexp = (yyvsp[0].cond_pexpr);
      pexpr->name = token_name(sql_string, &(yyloc));
      (yyval.pexpr) = pexpr;
    }
#line 3615 "yacc_sql.cpp"
    break;

  case 163: /* pexpr: arith_pexpr  */
#line 1430 "yacc_sql.y"
                  {      
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::ARITHMETIC;
      pexpr->aexp = (yyvsp[0].arith_pexpr);
      pexpr->name = token_name(sql_string, &(yyloc));
      (yyval.pexpr) = pexpr;
    }
#line 3627 "yacc_sql.cpp"
    break;

  case 164: /* pexpr: LBRACE pexpr RBRACE  */
#line 1437 "yacc_sql.y"
                          {
      (yyval.pexpr) = (yyvsp[-1].pexpr);
      (yyval.pexpr)->name(token_name(sql_string, &(yyloc)));
    }
#line 3636 "yacc_sql.cpp"
    break;

  case 165: /* pexpr: unary_pexpr  */
#line 1441 "yacc_sql.y"
                  {
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::UNARY;
      pexpr->uexp = (yyvsp[0].unary_pexpr);
      pexpr->name = token_name(sql_string, &(yyloc));
      (yyval.pexpr) = pexpr;
    }
#line 3648 "yacc_sql.cpp"
    break;

  case 166: /* pexpr: func_pexpr  */
#line 1448 "yacc_sql.y"
                 {        
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::FUNC;
        pexpr->fexp = (yyvsp[0].func_pexpr);
        pexpr->name = token_name(sql_string, &(yyloc));
        (yyval.pexpr) = pexpr;
    }
#line 3660 "yacc_sql.cpp"
    break;


#line 3664 "yacc_sql.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, sql_string, sql_result, scanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
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
                      yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
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
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, sql_string, sql_result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1460 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, ParsedSqlResult *sql_result) {
  yyscan_t scanner;
  yylex_init(&scanner);
  scan_string(s, scanner);
  int result = yyparse(s, sql_result, scanner);
  yylex_destroy(scanner);
  return result;
}
