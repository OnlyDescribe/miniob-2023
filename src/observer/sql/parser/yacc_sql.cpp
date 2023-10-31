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
    if (node->type == PExpType::UNARY && node->uexp->attr.aggr_type == AggrFuncType::INVALID) {
      relattr_cnt++;
    }
  }
  return relattr_cnt == 0 || (relattr_cnt == static_cast<int>(select_attr.size()));
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
  YYSYMBOL_SHOW = 12,                      /* SHOW  */
  YYSYMBOL_SYNC = 13,                      /* SYNC  */
  YYSYMBOL_INSERT = 14,                    /* INSERT  */
  YYSYMBOL_DELETE = 15,                    /* DELETE  */
  YYSYMBOL_UPDATE = 16,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 17,                    /* LBRACE  */
  YYSYMBOL_AGGR_MAX = 18,                  /* AGGR_MAX  */
  YYSYMBOL_AGGR_MIN = 19,                  /* AGGR_MIN  */
  YYSYMBOL_AGGR_SUM = 20,                  /* AGGR_SUM  */
  YYSYMBOL_AGGR_AVG = 21,                  /* AGGR_AVG  */
  YYSYMBOL_AGGR_COUNT = 22,                /* AGGR_COUNT  */
  YYSYMBOL_LENGTH = 23,                    /* LENGTH  */
  YYSYMBOL_ROUND = 24,                     /* ROUND  */
  YYSYMBOL_DATE_FORMAT = 25,               /* DATE_FORMAT  */
  YYSYMBOL_ASC = 26,                       /* ASC  */
  YYSYMBOL_DESC = 27,                      /* DESC  */
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
  YYSYMBOL_82_ = 82,                       /* '%'  */
  YYSYMBOL_UMINUS = 83,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 84,                  /* $accept  */
  YYSYMBOL_commands = 85,                  /* commands  */
  YYSYMBOL_command_wrapper = 86,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 87,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 88,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 89,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 90,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 91,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 92,             /* rollback_stmt  */
  YYSYMBOL_drop_table_stmt = 93,           /* drop_table_stmt  */
  YYSYMBOL_show_tables_stmt = 94,          /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 95,           /* desc_table_stmt  */
  YYSYMBOL_load_data_stmt = 96,            /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 97,              /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 98,         /* set_variable_stmt  */
  YYSYMBOL_create_index_stmt = 99,         /* create_index_stmt  */
  YYSYMBOL_drop_index_stmt = 100,          /* drop_index_stmt  */
  YYSYMBOL_create_table_stmt = 101,        /* create_table_stmt  */
  YYSYMBOL_insert_stmt = 102,              /* insert_stmt  */
  YYSYMBOL_delete_stmt = 103,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 104,              /* update_stmt  */
  YYSYMBOL_select_stmt = 105,              /* select_stmt  */
  YYSYMBOL_calc_stmt = 106,                /* calc_stmt  */
  YYSYMBOL_assignment_list = 107,          /* assignment_list  */
  YYSYMBOL_select_attr = 108,              /* select_attr  */
  YYSYMBOL_select_attr_list = 109,         /* select_attr_list  */
  YYSYMBOL_select_from = 110,              /* select_from  */
  YYSYMBOL_from_list = 111,                /* from_list  */
  YYSYMBOL_inner_join = 112,               /* inner_join  */
  YYSYMBOL_where = 113,                    /* where  */
  YYSYMBOL_group_by = 114,                 /* group_by  */
  YYSYMBOL_group_by_list = 115,            /* group_by_list  */
  YYSYMBOL_having = 116,                   /* having  */
  YYSYMBOL_order_by = 117,                 /* order_by  */
  YYSYMBOL_order_condtions = 118,          /* order_condtions  */
  YYSYMBOL_subquery_pexpr = 119,           /* subquery_pexpr  */
  YYSYMBOL_id_list = 120,                  /* id_list  */
  YYSYMBOL_attr_def_list = 121,            /* attr_def_list  */
  YYSYMBOL_attr_def = 122,                 /* attr_def  */
  YYSYMBOL_number = 123,                   /* number  */
  YYSYMBOL_type = 124,                     /* type  */
  YYSYMBOL_value_list = 125,               /* value_list  */
  YYSYMBOL_value = 126,                    /* value  */
  YYSYMBOL_aggr_func_type = 127,           /* aggr_func_type  */
  YYSYMBOL_field_or_star = 128,            /* field_or_star  */
  YYSYMBOL_field_or_star_list = 129,       /* field_or_star_list  */
  YYSYMBOL_rel_attr = 130,                 /* rel_attr  */
  YYSYMBOL_expression_list = 131,          /* expression_list  */
  YYSYMBOL_expression = 132,               /* expression  */
  YYSYMBOL_unary_pexpr = 133,              /* unary_pexpr  */
  YYSYMBOL_cond_pexpr = 134,               /* cond_pexpr  */
  YYSYMBOL_arith_pexpr = 135,              /* arith_pexpr  */
  YYSYMBOL_func_pexpr = 136,               /* func_pexpr  */
  YYSYMBOL_pexpr = 137,                    /* pexpr  */
  YYSYMBOL_opt_semicolon = 138             /* opt_semicolon  */
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
#define YYFINAL  88
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   605

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  167
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  345

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
       2,     2,     2,     2,     2,     2,     2,    82,     2,     2,
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
      75,    76,    77,    83
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   246,   246,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   277,   283,   288,   294,   300,   306,   312,
     319,   325,   333,   346,   354,   365,   387,   412,   422,   441,
     456,   466,   493,   530,   543,   546,   566,   575,   587,   602,
     605,   613,   624,   638,   651,   666,   683,   686,   697,   710,
     723,   735,   749,   766,   769,   777,   780,   786,   789,   801,
     818,   821,   833,   850,   853,   860,   863,   876,   889,   905,
     908,   920,   932,   947,   986,   989,  1003,  1006,  1019,  1028,
    1037,  1046,  1055,  1070,  1085,  1100,  1117,  1120,  1121,  1122,
    1123,  1124,  1130,  1133,  1144,  1148,  1152,  1160,  1164,  1176,
    1177,  1178,  1179,  1180,  1183,  1186,  1192,  1195,  1202,  1213,
    1218,  1224,  1229,  1236,  1247,  1252,  1264,  1267,  1270,  1273,
    1276,  1280,  1283,  1292,  1299,  1308,  1312,  1316,  1321,  1326,
    1331,  1336,  1341,  1346,  1351,  1356,  1367,  1378,  1382,  1386,
    1390,  1397,  1401,  1405,  1409,  1413,  1419,  1425,  1432,  1440,
    1451,  1458,  1465,  1469,  1476,  1483,  1492,  1493
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
  "DROP", "TABLE", "TABLES", "INDEX", "UNIQUE", "CALC", "SELECT", "SHOW",
  "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "AGGR_MAX", "AGGR_MIN",
  "AGGR_SUM", "AGGR_AVG", "AGGR_COUNT", "LENGTH", "ROUND", "DATE_FORMAT",
  "ASC", "DESC", "RBRACE", "INNER", "JOIN", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "ORDER", "GROUP", "BY",
  "STRING_T", "TEXT_T", "FLOAT_T", "DATE_T", "HELP", "EXIT", "DOT", "INTO",
  "VALUES", "FROM", "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE",
  "EXPLAIN", "EQ", "LT", "GT", "LE", "GE", "NE", "NOT", "IS", "AS", "LIKE",
  "EXISTS", "IN", "DEFAULT", "HAVING", "NULL_T", "NUMBER", "FLOAT", "ID",
  "SSS", "DATE", "OR", "'+'", "'-'", "'*'", "'/'", "'%'", "UMINUS",
  "$accept", "commands", "command_wrapper", "exit_stmt", "help_stmt",
  "sync_stmt", "begin_stmt", "commit_stmt", "rollback_stmt",
  "drop_table_stmt", "show_tables_stmt", "desc_table_stmt",
  "load_data_stmt", "explain_stmt", "set_variable_stmt",
  "create_index_stmt", "drop_index_stmt", "create_table_stmt",
  "insert_stmt", "delete_stmt", "update_stmt", "select_stmt", "calc_stmt",
  "assignment_list", "select_attr", "select_attr_list", "select_from",
  "from_list", "inner_join", "where", "group_by", "group_by_list",
  "having", "order_by", "order_condtions", "subquery_pexpr", "id_list",
  "attr_def_list", "attr_def", "number", "type", "value_list", "value",
  "aggr_func_type", "field_or_star", "field_or_star_list", "rel_attr",
  "expression_list", "expression", "unary_pexpr", "cond_pexpr",
  "arith_pexpr", "func_pexpr", "pexpr", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-300)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-75)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     395,   203,    94,     5,   155,    21,  -300,    -4,     4,   -28,
      13,  -300,  -300,  -300,  -300,  -300,    19,    18,   395,   137,
     118,  -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,
    -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,
    -300,    57,    93,   173,   112,   119,     5,  -300,  -300,  -300,
    -300,  -300,     5,  -300,  -300,   104,   128,  -300,  -300,  -300,
    -300,  -300,   151,   189,   196,   148,   155,   174,   155,  -300,
     177,  -300,  -300,   204,  -300,  -300,  -300,  -300,  -300,   399,
    -300,   158,   169,   197,  -300,   200,   213,  -300,  -300,  -300,
    -300,   242,   232,   212,  -300,   235,   -11,  -300,     5,     5,
       5,     5,     5,   155,   217,   155,   155,   155,   155,   524,
     -55,  -300,   214,    58,   155,   155,   155,   155,   155,   155,
     155,   155,   121,    -6,   215,   155,   155,   259,   155,   155,
     155,   155,   155,  -300,   244,   243,   219,   190,   224,   226,
     227,   250,   229,  -300,  -300,   -65,   -65,  -300,  -300,   257,
    -300,   254,   192,   460,   524,  -300,  -300,   131,   243,   -33,
    -300,  -300,    58,   278,   424,   355,   524,   160,   160,   160,
     160,   524,   155,   155,   238,  -300,   259,   524,   291,  -300,
     510,    30,    30,  -300,  -300,   293,   155,  -300,   262,  -300,
     275,   124,   292,   307,   251,  -300,   214,  -300,  -300,   155,
     155,   296,   253,   255,   163,  -300,   299,    58,  -300,  -300,
     273,   259,  -300,   524,   291,  -300,  -300,   190,     3,   485,
     190,   322,  -300,  -300,  -300,  -300,  -300,    -8,   226,   325,
     281,   337,   243,   280,   317,   282,   159,   163,  -300,   324,
     294,  -300,   259,  -300,   332,   334,   308,   301,   295,   297,
    -300,   292,  -300,   353,   312,   299,  -300,  -300,   -41,   313,
     163,  -300,  -300,   314,   155,   354,  -300,   190,   361,   318,
     243,  -300,  -300,   363,  -300,  -300,  -300,   319,   373,   353,
     294,   155,   328,   351,   163,   163,  -300,   -19,   133,   366,
    -300,   332,  -300,   367,  -300,   -18,   353,  -300,   392,   354,
       1,   351,   163,  -300,  -300,   352,   357,  -300,   136,  -300,
     190,   369,   370,  -300,  -300,  -300,   397,   163,  -300,    17,
     401,    48,   334,  -300,  -300,  -300,  -300,   368,  -300,  -300,
     406,   406,   136,  -300,  -300,   401,  -300,  -300,    59,  -300,
     406,   406,  -300,  -300,  -300
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,    25,     0,     0,     0,
       0,    26,    27,    28,    24,    23,     0,     0,     0,     0,
     166,    22,    21,    14,    15,    16,    17,     9,    10,    11,
      18,    19,    20,    12,    13,     8,     5,     7,     6,     4,
       3,     0,     0,     0,     0,     0,     0,   107,   104,   105,
     106,   108,     0,   132,    43,   124,     0,   109,   110,   111,
     112,   113,     0,     0,     0,     0,     0,   121,     0,   119,
       0,   165,   133,     0,   134,   163,   160,   161,   164,    49,
      30,     0,     0,     0,    31,     0,     0,    33,     1,   167,
       2,     0,     0,     0,    29,     0,     0,   131,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   149,
       0,   155,     0,   116,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    49,     0,     0,
       0,     0,     0,    46,     0,    65,     0,     0,     0,     0,
       0,     0,     0,   130,   125,   126,   127,   128,   129,     0,
     162,     0,     0,     0,   150,   122,   120,    56,    65,     0,
     115,   114,   116,     0,    49,   136,   137,   139,   140,   141,
     142,   138,     0,     0,     0,   145,    49,   143,   147,    47,
     135,   151,   152,   153,   154,     0,     0,    40,     0,    34,
       0,     0,    86,     0,     0,    37,     0,   156,   157,     0,
       0,     0,     0,     0,    56,    53,    67,   116,   117,   123,
       0,    49,    50,   144,   148,   146,    48,     0,   160,     0,
       0,     0,    97,    98,    99,   100,   101,    92,     0,     0,
       0,     0,    65,     0,     0,     0,    56,    56,    54,     0,
      73,   118,    49,    51,   102,    44,     0,     0,     0,     0,
      93,    86,    38,    84,     0,    67,   158,   159,    63,     0,
      56,    57,    55,     0,     0,    75,    52,     0,     0,     0,
      65,    32,    96,     0,    95,    94,    87,     0,     0,    84,
      73,     0,     0,    63,    56,    56,    58,    70,   160,     0,
      42,   102,    39,     0,    41,    88,    84,    35,     0,    75,
     160,    63,    56,    60,    59,     0,     0,    68,     0,   103,
       0,     0,     0,    89,    85,    36,     0,    56,    61,    70,
      70,    79,    44,    91,    90,    83,    62,     0,    71,    69,
      79,    79,     0,    76,    45,    70,    78,    77,    79,    72,
      79,    79,    80,    82,    81
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -300,  -300,   425,  -300,  -300,  -300,  -300,  -300,  -300,  -300,
    -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,  -300,
    -300,  -300,  -300,   122,   342,  -120,   256,  -177,  -278,  -150,
     195,  -299,   167,   154,   -94,  -300,  -261,   218,   240,  -300,
    -300,   175,    -3,  -300,   311,  -149,  -298,   356,    42,  -300,
    -172,  -300,  -300,    -2,  -300
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,   270,    70,   133,   158,   205,   284,   187,
     240,   307,   265,   290,   333,    71,   278,   229,   192,   273,
     227,   268,    72,    73,   162,   163,    74,    54,    55,    75,
      76,    77,    78,   219,    90
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      53,   -64,    79,   -66,   -64,   302,   -66,   179,   206,   247,
     321,   281,   305,   208,   218,   101,   102,   143,   298,   155,
     328,   329,    46,   317,   282,   156,   306,   238,    80,   -64,
     -64,   -66,   -64,   283,   338,   314,   339,   -64,   -64,   -66,
     -66,   160,    81,    53,   212,   311,    83,   161,   305,    53,
     -64,   312,    82,   313,   104,   248,   216,   174,   241,   261,
     262,   249,   327,   250,   109,   175,   111,    99,   100,   101,
     102,   -64,    86,   -66,   330,   331,    47,    48,    49,   332,
      50,    51,   255,   286,    52,   340,   341,    84,    96,   159,
     332,   243,   288,    85,    97,    53,    53,    53,    53,    53,
      44,    79,    45,   151,   152,   153,   154,   303,   304,   300,
     131,   132,   164,   165,   166,   167,   168,   169,   170,   171,
     294,    89,   266,   177,   178,   318,   180,   181,   182,   183,
     184,    91,   160,   -74,   189,    98,   -74,    88,   161,   103,
     326,   145,   146,   147,   148,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    57,    58,    59,    60,    61,   222,
     201,   -74,   202,   223,   224,   225,   226,    92,   105,   -74,
     213,   214,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    93,    99,   100,   101,   102,    94,   172,   201,   173,
     202,    65,   201,    95,   202,    66,   203,   233,   234,    47,
      48,    49,    67,    50,    51,   204,   106,    68,    69,    41,
      67,    42,    43,   107,   244,   108,    69,   245,    65,   110,
     198,   113,    66,   199,   259,   112,    47,    48,    49,    67,
      50,    51,   134,   260,    68,    69,   336,   337,   129,   130,
     131,   132,   115,   135,   342,   150,   343,   344,   136,   116,
     117,   118,   119,   120,   121,   122,   123,   137,   125,   139,
     126,    47,    48,    49,   291,    50,    51,   115,   138,   128,
     129,   130,   131,   132,   116,   117,   118,   119,   120,   121,
     122,   123,   197,   125,   140,   126,   141,   142,   157,   176,
     114,   185,   186,   188,   128,   129,   130,   131,   132,   190,
     191,   193,   194,   195,   115,   196,   209,   322,   256,   215,
     217,   116,   117,   118,   119,   120,   121,   122,   123,   220,
     125,   221,   126,   228,   230,   231,   235,   236,   246,   237,
     115,   128,   129,   130,   131,   132,   239,   116,   117,   118,
     119,   120,   121,   122,   123,   257,   125,   242,   126,   117,
     118,   119,   120,   252,   254,   253,   258,   128,   129,   130,
     131,   132,   263,   267,   264,   269,   274,   115,   275,   129,
     130,   131,   132,   272,   116,   117,   118,   119,   120,   121,
     122,   123,   271,   125,   277,   126,   279,   285,   287,   292,
     289,   295,   293,   296,   128,   129,   130,   131,   132,     1,
       2,   297,   301,   281,   308,     3,     4,     5,     6,     7,
       8,     9,   116,   117,   118,   119,   120,   121,   122,   123,
     315,   125,    10,   126,   310,   325,   319,    11,    12,    13,
     114,   320,   305,   129,   130,   131,   132,   332,    14,    15,
     323,   324,   335,    87,   334,   149,    16,   299,    17,   115,
     280,    18,   232,   316,   144,   114,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   309,   126,   251,   276,
     207,     0,     0,   127,   115,     0,   128,   129,   130,   131,
     132,   116,   117,   118,   119,   120,   121,   122,   123,   210,
     125,   200,   126,     0,     0,     0,     0,     0,   211,     0,
       0,   128,   129,   130,   131,   132,     0,     0,     0,     0,
     115,     0,     0,     0,     0,     0,     0,   116,   117,   118,
     119,   120,   121,   122,   123,     0,   125,     0,   126,     0,
       0,     0,     0,     0,     0,   115,     0,   128,   129,   130,
     131,   132,   116,   117,   118,   119,   120,   121,   122,   123,
       0,   125,     0,   126,     0,     0,     0,     0,     0,     0,
     115,     0,   128,   129,   130,   131,   132,   116,   117,   118,
     119,   120,   121,   122,   123,     0,   125,     0,   126,     0,
       0,     0,   117,   118,   119,   120,     0,   122,   129,   130,
     131,   132,   126,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   129,   130,   131,   132
};

static const yytype_int16 yycheck[] =
{
       3,     0,     4,     0,     3,   283,     3,   127,   158,    17,
     308,    52,    31,   162,   186,    80,    81,    28,   279,    74,
     319,   320,    17,   301,    65,    80,    45,   204,     7,    28,
      29,    28,    31,    74,   332,   296,   335,    36,    37,    36,
      37,    74,    46,    46,   164,    63,    74,    80,    31,    52,
      49,    69,    48,    71,    56,    63,   176,    63,   207,   236,
     237,    69,    45,    71,    66,    71,    68,    78,    79,    80,
      81,    70,    54,    70,    26,    27,    71,    72,    73,    31,
      75,    76,   232,   260,    79,    26,    27,    74,    46,    31,
      31,   211,   264,    74,    52,    98,    99,   100,   101,   102,
       6,   103,     8,   105,   106,   107,   108,   284,   285,   281,
      80,    81,   114,   115,   116,   117,   118,   119,   120,   121,
     270,     3,   242,   125,   126,   302,   128,   129,   130,   131,
     132,    74,    74,     0,   137,    31,     3,     0,    80,    11,
     317,    99,   100,   101,   102,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    18,    19,    20,    21,    22,    35,
      29,    28,    31,    39,    40,    41,    42,    74,    17,    36,
     172,   173,    17,    18,    19,    20,    21,    22,    23,    24,
      25,     8,    78,    79,    80,    81,    74,    66,    29,    68,
      31,    63,    29,    74,    31,    67,    65,   199,   200,    71,
      72,    73,    74,    75,    76,    74,    17,    79,    80,     6,
      74,     8,     9,    17,   217,    67,    80,   220,    63,    45,
      28,    17,    67,    31,    65,    48,    71,    72,    73,    74,
      75,    76,    74,    74,    79,    80,   330,   331,    78,    79,
      80,    81,    50,    74,   338,    28,   340,   341,    51,    57,
      58,    59,    60,    61,    62,    63,    64,    57,    66,    17,
      68,    71,    72,    73,   267,    75,    76,    50,    55,    77,
      78,    79,    80,    81,    57,    58,    59,    60,    61,    62,
      63,    64,    28,    66,    52,    68,    74,    52,    74,    74,
      31,    47,    49,    74,    77,    78,    79,    80,    81,    75,
      74,    74,    52,    74,    50,    48,    28,   310,    28,    71,
      17,    57,    58,    59,    60,    61,    62,    63,    64,    57,
      66,    46,    68,    31,    17,    74,    30,    74,     6,    74,
      50,    77,    78,    79,    80,    81,    37,    57,    58,    59,
      60,    61,    62,    63,    64,    28,    66,    74,    68,    58,
      59,    60,    61,    28,    17,    74,    74,    77,    78,    79,
      80,    81,    38,    31,    70,    31,    71,    50,    71,    78,
      79,    80,    81,    72,    57,    58,    59,    60,    61,    62,
      63,    64,    74,    66,    31,    68,    74,    74,    74,    28,
      36,    28,    74,    74,    77,    78,    79,    80,    81,     4,
       5,    28,    74,    52,    38,    10,    11,    12,    13,    14,
      15,    16,    57,    58,    59,    60,    61,    62,    63,    64,
      28,    66,    27,    68,    57,    28,    74,    32,    33,    34,
      31,    74,    31,    78,    79,    80,    81,    31,    43,    44,
      71,    71,    74,    18,   322,   103,    51,   280,    53,    50,
     255,    56,   196,   299,    98,    31,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,   291,    68,   228,   251,
     159,    -1,    -1,    74,    50,    -1,    77,    78,    79,    80,
      81,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    31,    68,    -1,    -1,    -1,    -1,    -1,    74,    -1,
      -1,    77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    -1,    68,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    -1,    77,    78,    79,
      80,    81,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    -1,    77,    78,    79,    80,    81,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    -1,    68,    -1,
      -1,    -1,    58,    59,    60,    61,    -1,    63,    78,    79,
      80,    81,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    78,    79,    80,    81
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,    10,    11,    12,    13,    14,    15,    16,
      27,    32,    33,    34,    43,    44,    51,    53,    56,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     6,     8,     9,     6,     8,    17,    71,    72,    73,
      75,    76,    79,   126,   131,   132,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    63,    67,    74,    79,    80,
     108,   119,   126,   127,   130,   133,   134,   135,   136,   137,
       7,    46,    48,    74,    74,    74,    54,    86,     0,     3,
     138,    74,    74,     8,    74,    74,   132,   132,    31,    78,
      79,    80,    81,    11,   137,    17,    17,    17,    67,   137,
      45,   137,    48,    17,    31,    50,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    68,    74,    77,    78,
      79,    80,    81,   109,    74,    74,    51,    57,    55,    17,
      52,    74,    52,    28,   131,   132,   132,   132,   132,   108,
      28,   137,   137,   137,   137,    74,    80,    74,   110,    31,
      74,    80,   128,   129,   137,   137,   137,   137,   137,   137,
     137,   137,    66,    68,    63,    71,    74,   137,   137,   109,
     137,   137,   137,   137,   137,    47,    49,   113,    74,   126,
      75,    74,   122,    74,    52,    74,    48,    28,    28,    31,
      31,    29,    31,    65,    74,   111,   113,   128,   129,    28,
      65,    74,   109,   137,   137,    71,   109,    17,   134,   137,
      57,    46,    35,    39,    40,    41,    42,   124,    31,   121,
      17,    74,   110,   137,   137,    30,    74,    74,   111,    37,
     114,   129,    74,   109,   126,   126,     6,    17,    63,    69,
      71,   122,    28,    74,    17,   113,    28,    28,    74,    65,
      74,   111,   111,    38,    70,   116,   109,    31,   125,    31,
     107,    74,    72,   123,    71,    71,   121,    31,   120,    74,
     114,    52,    65,    74,   112,    74,   111,    74,   134,    36,
     117,   126,    28,    74,   113,    28,    74,    28,   120,   116,
     134,    74,   112,   111,   111,    31,    45,   115,    38,   125,
      57,    63,    69,    71,   120,    28,   117,   112,   111,    74,
      74,   130,   126,    71,    71,    28,   111,    45,   115,   115,
      26,    27,    31,   118,   107,    74,   118,   118,   130,   115,
      26,    27,   118,   118,   118
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    85,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   107,   108,   108,   108,   109,
     109,   109,   109,   110,   110,   110,   111,   111,   111,   111,
     111,   111,   111,   112,   112,   113,   113,   114,   114,   114,
     115,   115,   115,   116,   116,   117,   117,   117,   117,   118,
     118,   118,   118,   119,   120,   120,   121,   121,   122,   122,
     122,   122,   122,   122,   122,   122,   123,   124,   124,   124,
     124,   124,   125,   125,   126,   126,   126,   126,   126,   127,
     127,   127,   127,   127,   128,   128,   129,   129,   129,   130,
     130,   130,   130,   130,   131,   131,   132,   132,   132,   132,
     132,   132,   132,   133,   133,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   135,   135,   135,   135,   135,   136,   136,   136,   136,
     137,   137,   137,   137,   137,   137,   138,   138
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     2,     7,     2,     4,     9,    10,     5,     7,     8,
       4,     8,     8,     2,     0,     5,     2,     3,     4,     0,
       3,     4,     5,     2,     3,     4,     0,     3,     4,     5,
       5,     6,     7,     0,     2,     0,     2,     0,     4,     6,
       0,     3,     5,     0,     2,     0,     4,     5,     5,     0,
       3,     4,     4,    10,     0,     3,     0,     3,     5,     6,
       7,     7,     2,     3,     4,     4,     1,     1,     1,     1,
       1,     1,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     2,     3,     1,
       3,     1,     3,     4,     1,     3,     3,     3,     3,     3,
       3,     2,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     3,     4,     3,     4,     2,
       3,     3,     3,     3,     3,     2,     4,     4,     6,     6,
       1,     1,     3,     1,     1,     1,     0,     1
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
#line 247 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 1951 "yacc_sql.cpp"
    break;

  case 23: /* exit_stmt: EXIT  */
#line 277 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 1960 "yacc_sql.cpp"
    break;

  case 24: /* help_stmt: HELP  */
#line 283 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 1968 "yacc_sql.cpp"
    break;

  case 25: /* sync_stmt: SYNC  */
#line 288 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 1976 "yacc_sql.cpp"
    break;

  case 26: /* begin_stmt: TRX_BEGIN  */
#line 294 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 1984 "yacc_sql.cpp"
    break;

  case 27: /* commit_stmt: TRX_COMMIT  */
#line 300 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 1992 "yacc_sql.cpp"
    break;

  case 28: /* rollback_stmt: TRX_ROLLBACK  */
#line 306 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 2000 "yacc_sql.cpp"
    break;

  case 29: /* drop_table_stmt: DROP TABLE ID  */
#line 312 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2010 "yacc_sql.cpp"
    break;

  case 30: /* show_tables_stmt: SHOW TABLES  */
#line 319 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2018 "yacc_sql.cpp"
    break;

  case 31: /* desc_table_stmt: DESC ID  */
#line 325 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2028 "yacc_sql.cpp"
    break;

  case 32: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 334 "yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
#line 2042 "yacc_sql.cpp"
    break;

  case 33: /* explain_stmt: EXPLAIN command_wrapper  */
#line 347 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 2051 "yacc_sql.cpp"
    break;

  case 34: /* set_variable_stmt: SET ID EQ value  */
#line 355 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
#line 2063 "yacc_sql.cpp"
    break;

  case 35: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE ID id_list RBRACE  */
#line 366 "yacc_sql.y"
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
#line 2089 "yacc_sql.cpp"
    break;

  case 36: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE ID id_list RBRACE  */
#line 388 "yacc_sql.y"
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
#line 2115 "yacc_sql.cpp"
    break;

  case 37: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 413 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2127 "yacc_sql.cpp"
    break;

  case 38: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE  */
#line 423 "yacc_sql.y"
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
#line 2147 "yacc_sql.cpp"
    break;

  case 39: /* insert_stmt: INSERT INTO ID VALUES LBRACE value value_list RBRACE  */
#line 442 "yacc_sql.y"
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
#line 2163 "yacc_sql.cpp"
    break;

  case 40: /* delete_stmt: DELETE FROM ID where  */
#line 457 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].string);
      (yyval.sql_node)->deletion.conditions = (yyvsp[0].cond_pexpr);
      free((yyvsp[-1].string));
    }
#line 2174 "yacc_sql.cpp"
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

      (yyval.sql_node)->update.conditions = (yyvsp[0].cond_pexpr);
      
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
#line 494 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);

      if ((yyvsp[-6].pexpr_list) != nullptr) {
        (yyval.sql_node)->selection.attributes.swap(*(yyvsp[-6].pexpr_list));
        delete (yyvsp[-6].pexpr_list);
        if (!IsAttributesVailid((yyval.sql_node)->selection.attributes)) {
          yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }

      if((yyvsp[-4].from_node) != nullptr)
      {
        FromSqlNode* from_node = (yyvsp[-4].from_node);
        (yyval.sql_node)->selection.relations.swap(from_node->relations);
        (yyval.sql_node)->selection.join_conds.swap(from_node->join_conds);
        delete (yyvsp[-4].from_node);
      }

      (yyval.sql_node)->selection.conditions = (yyvsp[-3].cond_pexpr);

      if ((yyvsp[-2].rel_attr_list) != nullptr) {
        (yyval.sql_node)->selection.groupbys = *(yyvsp[-2].rel_attr_list);
        delete (yyvsp[-2].rel_attr_list);
      }

      (yyval.sql_node)->selection.havings = (yyvsp[-1].cond_pexpr);

      if ((yyvsp[0].orderby_list) != nullptr) {
        (yyval.sql_node)->selection.orderbys = *(yyvsp[0].orderby_list);
        delete (yyvsp[0].orderby_list);
      }
    }
#line 2240 "yacc_sql.cpp"
    break;

  case 43: /* calc_stmt: CALC expression_list  */
#line 531 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2251 "yacc_sql.cpp"
    break;

  case 44: /* assignment_list: %empty  */
#line 543 "yacc_sql.y"
    {
      (yyval.assignment_list) = nullptr;
    }
#line 2259 "yacc_sql.cpp"
    break;

  case 45: /* assignment_list: COMMA ID EQ value assignment_list  */
#line 547 "yacc_sql.y"
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
#line 2280 "yacc_sql.cpp"
    break;

  case 46: /* select_attr: pexpr select_attr_list  */
#line 566 "yacc_sql.y"
                           {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      (yyval.pexpr_list)->push_back((yyvsp[-1].pexpr));
      // std::reverse($$->begin(), $$->end());
    }
#line 2294 "yacc_sql.cpp"
    break;

  case 47: /* select_attr: pexpr ID select_attr_list  */
#line 575 "yacc_sql.y"
                                {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      PExpr * pexpr = (yyvsp[-2].pexpr);
      pexpr->alias = (yyvsp[-1].string);
      (yyval.pexpr_list)->push_back(pexpr);
      // std::reverse($$->begin(), $$->end());
      free((yyvsp[-1].string));
    }
#line 2311 "yacc_sql.cpp"
    break;

  case 48: /* select_attr: pexpr AS ID select_attr_list  */
#line 587 "yacc_sql.y"
                                   {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      PExpr * pexpr = (yyvsp[-3].pexpr);
      pexpr->alias = (yyvsp[-1].string);
      (yyval.pexpr_list)->push_back(pexpr);
      // std::reverse($$->begin(), $$->end());
      free((yyvsp[-1].string));
    }
#line 2328 "yacc_sql.cpp"
    break;

  case 49: /* select_attr_list: %empty  */
#line 602 "yacc_sql.y"
    {
      (yyval.pexpr_list) = nullptr;
    }
#line 2336 "yacc_sql.cpp"
    break;

  case 50: /* select_attr_list: COMMA pexpr select_attr_list  */
#line 605 "yacc_sql.y"
                                   {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      (yyval.pexpr_list)->push_back((yyvsp[-1].pexpr));
    }
#line 2349 "yacc_sql.cpp"
    break;

  case 51: /* select_attr_list: COMMA pexpr ID select_attr_list  */
#line 613 "yacc_sql.y"
                                      {
      if ((yyvsp[0].pexpr_list) != nullptr) {
        (yyval.pexpr_list) = (yyvsp[0].pexpr_list);
      } else {
        (yyval.pexpr_list) = new std::vector<PExpr *>;
      }
      PExpr * pexpr = (yyvsp[-2].pexpr);
      pexpr->alias = (yyvsp[-1].string);
      (yyval.pexpr_list)->push_back(pexpr);
      free((yyvsp[-1].string));
    }
#line 2365 "yacc_sql.cpp"
    break;

  case 52: /* select_attr_list: COMMA pexpr AS ID select_attr_list  */
#line 624 "yacc_sql.y"
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
#line 2381 "yacc_sql.cpp"
    break;

  case 53: /* select_from: ID from_list  */
#line 638 "yacc_sql.y"
                 {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-1].string);
      (yyval.from_node)->relations.push_back(std::move(rel));
      std::reverse((yyval.from_node)->relations.begin(), (yyval.from_node)->relations.end());
      std::reverse((yyval.from_node)->join_conds.begin(), (yyval.from_node)->join_conds.end());
      free((yyvsp[-1].string));
    }
#line 2399 "yacc_sql.cpp"
    break;

  case 54: /* select_from: ID ID from_list  */
#line 651 "yacc_sql.y"
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
      std::reverse((yyval.from_node)->relations.begin(), (yyval.from_node)->relations.end());
      std::reverse((yyval.from_node)->join_conds.begin(), (yyval.from_node)->join_conds.end());
      free((yyvsp[-2].string));
      free((yyvsp[-1].string));
    }
#line 2419 "yacc_sql.cpp"
    break;

  case 55: /* select_from: ID AS ID from_list  */
#line 666 "yacc_sql.y"
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
      std::reverse((yyval.from_node)->relations.begin(), (yyval.from_node)->relations.end());
      std::reverse((yyval.from_node)->join_conds.begin(), (yyval.from_node)->join_conds.end());
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2439 "yacc_sql.cpp"
    break;

  case 56: /* from_list: %empty  */
#line 683 "yacc_sql.y"
    {
      (yyval.from_node) = nullptr;
    }
#line 2447 "yacc_sql.cpp"
    break;

  case 57: /* from_list: COMMA ID from_list  */
#line 686 "yacc_sql.y"
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
#line 2463 "yacc_sql.cpp"
    break;

  case 58: /* from_list: COMMA ID ID from_list  */
#line 697 "yacc_sql.y"
                            {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
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
#line 2481 "yacc_sql.cpp"
    break;

  case 59: /* from_list: COMMA ID AS ID from_list  */
#line 710 "yacc_sql.y"
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
#line 2499 "yacc_sql.cpp"
    break;

  case 60: /* from_list: INNER JOIN ID inner_join from_list  */
#line 723 "yacc_sql.y"
                                         {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-2].string);
      (yyval.from_node)->relations.push_back(rel);
      (yyval.from_node)->join_conds.push_back((yyvsp[-1].cond_pexpr));
      free((yyvsp[-2].string));
    }
#line 2516 "yacc_sql.cpp"
    break;

  case 61: /* from_list: INNER JOIN ID ID inner_join from_list  */
#line 735 "yacc_sql.y"
                                            {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-3].string);
      rel.alias = (yyvsp[-2].string);
      (yyval.from_node)->relations.push_back(rel);
      (yyval.from_node)->join_conds.push_back((yyvsp[-1].cond_pexpr));
      free((yyvsp[-3].string));
      free((yyvsp[-2].string));
    }
#line 2535 "yacc_sql.cpp"
    break;

  case 62: /* from_list: INNER JOIN ID AS ID inner_join from_list  */
#line 749 "yacc_sql.y"
                                               {
      if ((yyvsp[0].from_node) != nullptr) {
        (yyval.from_node) = (yyvsp[0].from_node);
      } else {
        (yyval.from_node) = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = (yyvsp[-4].string);
      rel.alias = (yyvsp[-2].string);
      (yyval.from_node)->relations.push_back(rel);
      (yyval.from_node)->join_conds.push_back((yyvsp[-1].cond_pexpr));
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
    }
#line 2554 "yacc_sql.cpp"
    break;

  case 63: /* inner_join: %empty  */
#line 766 "yacc_sql.y"
    {
      (yyval.cond_pexpr) = nullptr;
    }
#line 2562 "yacc_sql.cpp"
    break;

  case 64: /* inner_join: ON cond_pexpr  */
#line 769 "yacc_sql.y"
                   {
      (yyval.cond_pexpr) = (yyvsp[0].cond_pexpr);
    }
#line 2570 "yacc_sql.cpp"
    break;

  case 65: /* where: %empty  */
#line 777 "yacc_sql.y"
    {
      (yyval.cond_pexpr) = nullptr;
    }
#line 2578 "yacc_sql.cpp"
    break;

  case 66: /* where: WHERE cond_pexpr  */
#line 780 "yacc_sql.y"
                       {
      (yyval.cond_pexpr) = (yyvsp[0].cond_pexpr);  
    }
#line 2586 "yacc_sql.cpp"
    break;

  case 67: /* group_by: %empty  */
#line 786 "yacc_sql.y"
    {
      (yyval.rel_attr_list) = nullptr;
    }
#line 2594 "yacc_sql.cpp"
    break;

  case 68: /* group_by: GROUP BY ID group_by_list  */
#line 789 "yacc_sql.y"
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
#line 2611 "yacc_sql.cpp"
    break;

  case 69: /* group_by: GROUP BY ID DOT ID group_by_list  */
#line 801 "yacc_sql.y"
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
#line 2630 "yacc_sql.cpp"
    break;

  case 70: /* group_by_list: %empty  */
#line 818 "yacc_sql.y"
    { 
      (yyval.rel_attr_list) = nullptr;
    }
#line 2638 "yacc_sql.cpp"
    break;

  case 71: /* group_by_list: COMMA ID group_by_list  */
#line 821 "yacc_sql.y"
                             {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.attribute_name = (yyvsp[-1].string);
      rel_attr.relation_name = (yyvsp[-1].string);
      (yyval.rel_attr_list)->push_back(rel_attr);
      free((yyvsp[-1].string));
    }
#line 2655 "yacc_sql.cpp"
    break;

  case 72: /* group_by_list: COMMA ID DOT ID group_by_list  */
#line 833 "yacc_sql.y"
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
#line 2673 "yacc_sql.cpp"
    break;

  case 73: /* having: %empty  */
#line 850 "yacc_sql.y"
    {
      (yyval.cond_pexpr) = nullptr;
    }
#line 2681 "yacc_sql.cpp"
    break;

  case 74: /* having: HAVING cond_pexpr  */
#line 853 "yacc_sql.y"
                        {
      (yyval.cond_pexpr) = (yyvsp[0].cond_pexpr);  
    }
#line 2689 "yacc_sql.cpp"
    break;

  case 75: /* order_by: %empty  */
#line 860 "yacc_sql.y"
    {
      (yyval.orderby_list) = nullptr;
    }
#line 2697 "yacc_sql.cpp"
    break;

  case 76: /* order_by: ORDER BY rel_attr order_condtions  */
#line 863 "yacc_sql.y"
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
#line 2715 "yacc_sql.cpp"
    break;

  case 77: /* order_by: ORDER BY rel_attr DESC order_condtions  */
#line 876 "yacc_sql.y"
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
#line 2733 "yacc_sql.cpp"
    break;

  case 78: /* order_by: ORDER BY rel_attr ASC order_condtions  */
#line 889 "yacc_sql.y"
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
#line 2751 "yacc_sql.cpp"
    break;

  case 79: /* order_condtions: %empty  */
#line 905 "yacc_sql.y"
    {
      (yyval.orderby_list) = nullptr;
    }
#line 2759 "yacc_sql.cpp"
    break;

  case 80: /* order_condtions: COMMA rel_attr order_condtions  */
#line 908 "yacc_sql.y"
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
#line 2776 "yacc_sql.cpp"
    break;

  case 81: /* order_condtions: COMMA rel_attr DESC order_condtions  */
#line 920 "yacc_sql.y"
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
#line 2793 "yacc_sql.cpp"
    break;

  case 82: /* order_condtions: COMMA rel_attr ASC order_condtions  */
#line 932 "yacc_sql.y"
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
#line 2810 "yacc_sql.cpp"
    break;

  case 83: /* subquery_pexpr: LBRACE SELECT select_attr FROM select_from where group_by having order_by RBRACE  */
#line 948 "yacc_sql.y"
    {
      (yyval.subquery_pexpr) = new PSubQueryExpr;
      SelectSqlNode * sub_select = (yyval.subquery_pexpr)->sub_select;
      if ((yyvsp[-7].pexpr_list) != nullptr) {
        sub_select->attributes.swap(*(yyvsp[-7].pexpr_list));
        delete (yyvsp[-7].pexpr_list);
        if (!IsAttributesVailid(sub_select->attributes)) {
          yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }

      if((yyvsp[-5].from_node) != nullptr)
      {
        FromSqlNode* from_node = (yyvsp[-5].from_node);
        sub_select->relations.swap(from_node->relations);
        sub_select->join_conds.swap(from_node->join_conds);
        delete (yyvsp[-5].from_node);
      }

      sub_select->conditions = (yyvsp[-4].cond_pexpr);

      if ((yyvsp[-3].rel_attr_list) != nullptr) {
        sub_select->groupbys = *(yyvsp[-3].rel_attr_list);
        delete (yyvsp[-3].rel_attr_list);
      }

      sub_select->havings = (yyvsp[-2].cond_pexpr);

      if ((yyvsp[-1].orderby_list) != nullptr) {
        sub_select->orderbys = *(yyvsp[-1].orderby_list);
        delete (yyvsp[-1].orderby_list);
      }
    }
#line 2848 "yacc_sql.cpp"
    break;

  case 84: /* id_list: %empty  */
#line 986 "yacc_sql.y"
    {
      (yyval.std_string_list) = nullptr;
    }
#line 2856 "yacc_sql.cpp"
    break;

  case 85: /* id_list: COMMA ID id_list  */
#line 990 "yacc_sql.y"
    {
      if ((yyvsp[0].std_string_list) != nullptr) {
        (yyval.std_string_list) = (yyvsp[0].std_string_list);
      } else {
        (yyval.std_string_list) = new std::vector<std::string>;
      }
      (yyval.std_string_list)->emplace_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 2870 "yacc_sql.cpp"
    break;

  case 86: /* attr_def_list: %empty  */
#line 1003 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
#line 2878 "yacc_sql.cpp"
    break;

  case 87: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 1007 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
#line 2892 "yacc_sql.cpp"
    break;

  case 88: /* attr_def: ID type LBRACE number RBRACE  */
#line 1020 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].string);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      (yyval.attr_info)->is_not_null = false;
      free((yyvsp[-4].string));
    }
#line 2905 "yacc_sql.cpp"
    break;

  case 89: /* attr_def: ID type LBRACE number RBRACE NULL_T  */
#line 1029 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].string);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->is_not_null = false;
      free((yyvsp[-5].string));
    }
#line 2918 "yacc_sql.cpp"
    break;

  case 90: /* attr_def: ID type LBRACE number RBRACE DEFAULT NULL_T  */
#line 1038 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-5].number);
      (yyval.attr_info)->name = (yyvsp[-6].string);
      (yyval.attr_info)->length = (yyvsp[-3].number);
      (yyval.attr_info)->is_not_null = false;
      free((yyvsp[-6].string));
    }
#line 2931 "yacc_sql.cpp"
    break;

  case 91: /* attr_def: ID type LBRACE number RBRACE NOT NULL_T  */
#line 1047 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-5].number);
      (yyval.attr_info)->name = (yyvsp[-6].string);
      (yyval.attr_info)->length = (yyvsp[-3].number);
      (yyval.attr_info)->is_not_null = true;
      free((yyvsp[-6].string));
    }
#line 2944 "yacc_sql.cpp"
    break;

  case 92: /* attr_def: ID type  */
#line 1056 "yacc_sql.y"
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
#line 2963 "yacc_sql.cpp"
    break;

  case 93: /* attr_def: ID type NULL_T  */
#line 1071 "yacc_sql.y"
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
#line 2982 "yacc_sql.cpp"
    break;

  case 94: /* attr_def: ID type DEFAULT NULL_T  */
#line 1086 "yacc_sql.y"
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
#line 3001 "yacc_sql.cpp"
    break;

  case 95: /* attr_def: ID type NOT NULL_T  */
#line 1101 "yacc_sql.y"
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
#line 3020 "yacc_sql.cpp"
    break;

  case 96: /* number: NUMBER  */
#line 1117 "yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 3026 "yacc_sql.cpp"
    break;

  case 97: /* type: INT_T  */
#line 1120 "yacc_sql.y"
               { (yyval.number)=AttrType::INTS; }
#line 3032 "yacc_sql.cpp"
    break;

  case 98: /* type: STRING_T  */
#line 1121 "yacc_sql.y"
               { (yyval.number)=AttrType::CHARS; }
#line 3038 "yacc_sql.cpp"
    break;

  case 99: /* type: TEXT_T  */
#line 1122 "yacc_sql.y"
               { (yyval.number)=AttrType::TEXTS; }
#line 3044 "yacc_sql.cpp"
    break;

  case 100: /* type: FLOAT_T  */
#line 1123 "yacc_sql.y"
               { (yyval.number)=AttrType::FLOATS; }
#line 3050 "yacc_sql.cpp"
    break;

  case 101: /* type: DATE_T  */
#line 1124 "yacc_sql.y"
               { (yyval.number)=AttrType::DATES; }
#line 3056 "yacc_sql.cpp"
    break;

  case 102: /* value_list: %empty  */
#line 1130 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
#line 3064 "yacc_sql.cpp"
    break;

  case 103: /* value_list: COMMA value value_list  */
#line 1133 "yacc_sql.y"
                              { 
      if ((yyvsp[0].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[0].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(*(yyvsp[-1].value));
      delete (yyvsp[-1].value);
    }
#line 3078 "yacc_sql.cpp"
    break;

  case 104: /* value: NUMBER  */
#line 1144 "yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 3087 "yacc_sql.cpp"
    break;

  case 105: /* value: FLOAT  */
#line 1148 "yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 3096 "yacc_sql.cpp"
    break;

  case 106: /* value: SSS  */
#line 1152 "yacc_sql.y"
         {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      if(strlen(tmp) > 65535){
        yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid text", SCF_INVALID);
      }
      (yyval.value) = new Value(tmp);
      free(tmp);
    }
#line 3109 "yacc_sql.cpp"
    break;

  case 107: /* value: NULL_T  */
#line 1160 "yacc_sql.y"
            {
      (yyval.value) = new Value(AttrType::NULLS);
      (yyloc) = (yylsp[0]);
    }
#line 3118 "yacc_sql.cpp"
    break;

  case 108: /* value: DATE  */
#line 1164 "yacc_sql.y"
          {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value(tmp, DATES);
      free(tmp);
      if(!(yyval.value)->check_date()){
        yyerror(&(yyloc), sql_string, sql_result, scanner, "invalid date", SCF_INVALID);
      }
    }
#line 3131 "yacc_sql.cpp"
    break;

  case 109: /* aggr_func_type: AGGR_MAX  */
#line 1176 "yacc_sql.y"
             { (yyval.aggr_func_type) = AggrFuncType::MAX; }
#line 3137 "yacc_sql.cpp"
    break;

  case 110: /* aggr_func_type: AGGR_MIN  */
#line 1177 "yacc_sql.y"
               { (yyval.aggr_func_type) = AggrFuncType::MIN; }
#line 3143 "yacc_sql.cpp"
    break;

  case 111: /* aggr_func_type: AGGR_SUM  */
#line 1178 "yacc_sql.y"
               { (yyval.aggr_func_type) = AggrFuncType::SUM; }
#line 3149 "yacc_sql.cpp"
    break;

  case 112: /* aggr_func_type: AGGR_AVG  */
#line 1179 "yacc_sql.y"
               { (yyval.aggr_func_type) = AggrFuncType::AVG; }
#line 3155 "yacc_sql.cpp"
    break;

  case 113: /* aggr_func_type: AGGR_COUNT  */
#line 1180 "yacc_sql.y"
                 { (yyval.aggr_func_type) = AggrFuncType::COUNT; }
#line 3161 "yacc_sql.cpp"
    break;

  case 114: /* field_or_star: '*'  */
#line 1183 "yacc_sql.y"
      {
    (yyval.string) = "*";
  }
#line 3169 "yacc_sql.cpp"
    break;

  case 115: /* field_or_star: ID  */
#line 1186 "yacc_sql.y"
       {
    (yyval.string) = (yyvsp[0].string);
  }
#line 3177 "yacc_sql.cpp"
    break;

  case 116: /* field_or_star_list: %empty  */
#line 1192 "yacc_sql.y"
    {
      (yyval.std_string_list) = nullptr;
    }
#line 3185 "yacc_sql.cpp"
    break;

  case 117: /* field_or_star_list: field_or_star field_or_star_list  */
#line 1195 "yacc_sql.y"
                                       {
      (yyval.std_string_list) = new std::vector<std::string>;
      (yyval.std_string_list)->emplace_back((yyvsp[-1].string));
      if ((yyvsp[0].std_string_list) != nullptr) {
        (yyval.std_string_list)->insert((yyval.std_string_list)->end(), (yyvsp[0].std_string_list)->begin(), (yyvsp[0].std_string_list)->end());
      }
    }
#line 3197 "yacc_sql.cpp"
    break;

  case 118: /* field_or_star_list: COMMA field_or_star field_or_star_list  */
#line 1202 "yacc_sql.y"
                                             {
      if ((yyvsp[0].std_string_list) != nullptr) {
        (yyval.std_string_list) = (yyvsp[0].std_string_list);
      } else {
        (yyval.std_string_list) = new std::vector<std::string>;
      }
      (yyval.std_string_list)->emplace_back((yyvsp[-1].string));
    }
#line 3210 "yacc_sql.cpp"
    break;

  case 119: /* rel_attr: '*'  */
#line 1213 "yacc_sql.y"
        {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = "";
      (yyval.rel_attr)->attribute_name = "*";
    }
#line 3220 "yacc_sql.cpp"
    break;

  case 120: /* rel_attr: ID DOT '*'  */
#line 1218 "yacc_sql.y"
                 {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = "*";
      free((yyvsp[-2].string));
    }
#line 3231 "yacc_sql.cpp"
    break;

  case 121: /* rel_attr: ID  */
#line 1224 "yacc_sql.y"
         {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 3241 "yacc_sql.cpp"
    break;

  case 122: /* rel_attr: ID DOT ID  */
#line 1229 "yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 3253 "yacc_sql.cpp"
    break;

  case 123: /* rel_attr: aggr_func_type LBRACE field_or_star_list RBRACE  */
#line 1236 "yacc_sql.y"
                                                      {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->aggr_type = (yyvsp[-3].aggr_func_type);
      if ((yyvsp[-1].std_string_list) != nullptr) {
        (yyval.rel_attr)->aggregates = *(yyvsp[-1].std_string_list);
        delete (yyvsp[-1].std_string_list);
      }
    }
#line 3266 "yacc_sql.cpp"
    break;

  case 124: /* expression_list: expression  */
#line 1248 "yacc_sql.y"
    {
      (yyval.expression_list) = new std::vector<Expression*>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 3275 "yacc_sql.cpp"
    break;

  case 125: /* expression_list: expression COMMA expression_list  */
#line 1253 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 3288 "yacc_sql.cpp"
    break;

  case 126: /* expression: expression '+' expression  */
#line 1264 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3296 "yacc_sql.cpp"
    break;

  case 127: /* expression: expression '-' expression  */
#line 1267 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3304 "yacc_sql.cpp"
    break;

  case 128: /* expression: expression '*' expression  */
#line 1270 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3312 "yacc_sql.cpp"
    break;

  case 129: /* expression: expression '/' expression  */
#line 1273 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 3320 "yacc_sql.cpp"
    break;

  case 130: /* expression: LBRACE expression RBRACE  */
#line 1276 "yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 3329 "yacc_sql.cpp"
    break;

  case 131: /* expression: '-' expression  */
#line 1280 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 3337 "yacc_sql.cpp"
    break;

  case 132: /* expression: value  */
#line 1283 "yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 3347 "yacc_sql.cpp"
    break;

  case 133: /* unary_pexpr: value  */
#line 1292 "yacc_sql.y"
        {
    PUnaryExpr *unary_pexpr = new PUnaryExpr;
    unary_pexpr->is_attr = false;
    unary_pexpr->value = *(yyvsp[0].value);
    (yyval.unary_pexpr) = unary_pexpr;
    delete (yyvsp[0].value);
  }
#line 3359 "yacc_sql.cpp"
    break;

  case 134: /* unary_pexpr: rel_attr  */
#line 1299 "yacc_sql.y"
             {
    PUnaryExpr *unary_pexpr = new PUnaryExpr;
    unary_pexpr->is_attr = true;
    unary_pexpr->attr = *(yyvsp[0].rel_attr);
    (yyval.unary_pexpr) = unary_pexpr;
    delete (yyvsp[0].rel_attr);
  }
#line 3371 "yacc_sql.cpp"
    break;

  case 135: /* cond_pexpr: pexpr OR pexpr  */
#line 1308 "yacc_sql.y"
                  {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::OR, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3380 "yacc_sql.cpp"
    break;

  case 136: /* cond_pexpr: pexpr AND pexpr  */
#line 1312 "yacc_sql.y"
                     {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::AND, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3389 "yacc_sql.cpp"
    break;

  case 137: /* cond_pexpr: pexpr EQ pexpr  */
#line 1317 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::EQUAL_TO, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3398 "yacc_sql.cpp"
    break;

  case 138: /* cond_pexpr: pexpr NE pexpr  */
#line 1322 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_EQUAL, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3407 "yacc_sql.cpp"
    break;

  case 139: /* cond_pexpr: pexpr LT pexpr  */
#line 1327 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::LESS_THAN, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3416 "yacc_sql.cpp"
    break;

  case 140: /* cond_pexpr: pexpr GT pexpr  */
#line 1332 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::GREAT_THAN, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3425 "yacc_sql.cpp"
    break;

  case 141: /* cond_pexpr: pexpr LE pexpr  */
#line 1337 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::LESS_EQUAL, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3434 "yacc_sql.cpp"
    break;

  case 142: /* cond_pexpr: pexpr GE pexpr  */
#line 1342 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::GREAT_EQUAL, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3443 "yacc_sql.cpp"
    break;

  case 143: /* cond_pexpr: pexpr LIKE pexpr  */
#line 1347 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::LIKE, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3452 "yacc_sql.cpp"
    break;

  case 144: /* cond_pexpr: pexpr NOT LIKE pexpr  */
#line 1352 "yacc_sql.y"
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_LIKE, (yyvsp[-3].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3461 "yacc_sql.cpp"
    break;

  case 145: /* cond_pexpr: pexpr IS NULL_T  */
#line 1357 "yacc_sql.y"
    {
      PUnaryExpr *unary_pexpr = new PUnaryExpr;
      unary_pexpr->is_attr = false;
      unary_pexpr->value = Value(AttrType::NULLS);
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::UNARY;
      pexpr->uexp = unary_pexpr;
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IS_NULL, (yyvsp[-2].pexpr), pexpr);
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3476 "yacc_sql.cpp"
    break;

  case 146: /* cond_pexpr: pexpr IS NOT NULL_T  */
#line 1368 "yacc_sql.y"
    {
      PUnaryExpr *unary_pexpr = new PUnaryExpr;
      unary_pexpr->is_attr = false;
      unary_pexpr->value = Value(AttrType::NULLS);
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::UNARY;
      pexpr->uexp = unary_pexpr;
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IS_NOT_NULL, (yyvsp[-3].pexpr), pexpr);
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3491 "yacc_sql.cpp"
    break;

  case 147: /* cond_pexpr: pexpr IN pexpr  */
#line 1378 "yacc_sql.y"
                    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IN, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3500 "yacc_sql.cpp"
    break;

  case 148: /* cond_pexpr: pexpr NOT IN pexpr  */
#line 1382 "yacc_sql.y"
                        {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_IN, (yyvsp[-3].pexpr), (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3509 "yacc_sql.cpp"
    break;

  case 149: /* cond_pexpr: EXISTS pexpr  */
#line 1386 "yacc_sql.y"
                  {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::EXISTS, nullptr, (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3518 "yacc_sql.cpp"
    break;

  case 150: /* cond_pexpr: NOT EXISTS pexpr  */
#line 1390 "yacc_sql.y"
                      {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_EXISTS, nullptr, (yyvsp[0].pexpr));
      (yyval.cond_pexpr) = condition_pexpr;
    }
#line 3527 "yacc_sql.cpp"
    break;

  case 151: /* arith_pexpr: pexpr '+' pexpr  */
#line 1397 "yacc_sql.y"
                      {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::ADD, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
          (yyval.arith_pexpr) = arith_pexpr;
      }
#line 3536 "yacc_sql.cpp"
    break;

  case 152: /* arith_pexpr: pexpr '-' pexpr  */
#line 1401 "yacc_sql.y"
                        {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::SUB, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
          (yyval.arith_pexpr) = arith_pexpr;
      }
#line 3545 "yacc_sql.cpp"
    break;

  case 153: /* arith_pexpr: pexpr '*' pexpr  */
#line 1405 "yacc_sql.y"
                        {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::MUL, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
          (yyval.arith_pexpr) = arith_pexpr;
      }
#line 3554 "yacc_sql.cpp"
    break;

  case 154: /* arith_pexpr: pexpr '/' pexpr  */
#line 1409 "yacc_sql.y"
                        {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::DIV, (yyvsp[-2].pexpr), (yyvsp[0].pexpr));
          (yyval.arith_pexpr) = arith_pexpr;
      }
#line 3563 "yacc_sql.cpp"
    break;

  case 155: /* arith_pexpr: '-' pexpr  */
#line 1413 "yacc_sql.y"
                               {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::NEGATIVE, (yyvsp[0].pexpr), nullptr);
          (yyval.arith_pexpr) = arith_pexpr;
      }
#line 3572 "yacc_sql.cpp"
    break;

  case 156: /* func_pexpr: LENGTH LBRACE pexpr RBRACE  */
#line 1419 "yacc_sql.y"
                               {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::LENGTH;
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3583 "yacc_sql.cpp"
    break;

  case 157: /* func_pexpr: ROUND LBRACE pexpr RBRACE  */
#line 1426 "yacc_sql.y"
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::ROUND;
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3594 "yacc_sql.cpp"
    break;

  case 158: /* func_pexpr: ROUND LBRACE pexpr COMMA pexpr RBRACE  */
#line 1433 "yacc_sql.y"
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::ROUND;
      func_pexpr->params.push_back((yyvsp[-3].pexpr));
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3606 "yacc_sql.cpp"
    break;

  case 159: /* func_pexpr: DATE_FORMAT LBRACE pexpr COMMA pexpr RBRACE  */
#line 1441 "yacc_sql.y"
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::DATE_FORMAT;
      func_pexpr->params.push_back((yyvsp[-3].pexpr));
      func_pexpr->params.push_back((yyvsp[-1].pexpr));
      (yyval.func_pexpr) = func_pexpr;
    }
#line 3618 "yacc_sql.cpp"
    break;

  case 160: /* pexpr: cond_pexpr  */
#line 1451 "yacc_sql.y"
               {
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::COMPARISON;
      pexpr->cexp = (yyvsp[0].cond_pexpr);
      pexpr->name = token_name(sql_string, &(yyloc));
      (yyval.pexpr) = pexpr;
    }
#line 3630 "yacc_sql.cpp"
    break;

  case 161: /* pexpr: arith_pexpr  */
#line 1458 "yacc_sql.y"
                  {      
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::ARITHMETIC;
      pexpr->aexp = (yyvsp[0].arith_pexpr);
      pexpr->name = token_name(sql_string, &(yyloc));
      (yyval.pexpr) = pexpr;
    }
#line 3642 "yacc_sql.cpp"
    break;

  case 162: /* pexpr: LBRACE pexpr RBRACE  */
#line 1465 "yacc_sql.y"
                          {
      (yyval.pexpr) = (yyvsp[-1].pexpr);
      (yyval.pexpr)->name = token_name(sql_string, &(yyloc));
    }
#line 3651 "yacc_sql.cpp"
    break;

  case 163: /* pexpr: unary_pexpr  */
#line 1469 "yacc_sql.y"
                  {
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::UNARY;
      pexpr->uexp = (yyvsp[0].unary_pexpr);
      pexpr->name = token_name(sql_string, &(yyloc));
      (yyval.pexpr) = pexpr;
    }
#line 3663 "yacc_sql.cpp"
    break;

  case 164: /* pexpr: func_pexpr  */
#line 1476 "yacc_sql.y"
                 {        
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::FUNC;
        pexpr->fexp = (yyvsp[0].func_pexpr);
        pexpr->name = token_name(sql_string, &(yyloc));
        (yyval.pexpr) = pexpr;
    }
#line 3675 "yacc_sql.cpp"
    break;

  case 165: /* pexpr: subquery_pexpr  */
#line 1483 "yacc_sql.y"
                     { 
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::SUBQUERY;
        pexpr->sexp = (yyvsp[0].subquery_pexpr);
        pexpr->name = token_name(sql_string, &(yyloc));
        (yyval.pexpr) = pexpr;
    }
#line 3687 "yacc_sql.cpp"
    break;


#line 3691 "yacc_sql.cpp"

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

#line 1495 "yacc_sql.y"

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
