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

#ifndef YY_YY_YACC_SQL_HPP_INCLUDED
# define YY_YY_YACC_SQL_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMICOLON = 258,               /* SEMICOLON  */
    CREATE = 259,                  /* CREATE  */
    DROP = 260,                    /* DROP  */
    TABLE = 261,                   /* TABLE  */
    TABLES = 262,                  /* TABLES  */
    INDEX = 263,                   /* INDEX  */
    UNIQUE = 264,                  /* UNIQUE  */
    CALC = 265,                    /* CALC  */
    SELECT = 266,                  /* SELECT  */
    SHOW = 267,                    /* SHOW  */
    SYNC = 268,                    /* SYNC  */
    INSERT = 269,                  /* INSERT  */
    DELETE = 270,                  /* DELETE  */
    UPDATE = 271,                  /* UPDATE  */
    LBRACE = 272,                  /* LBRACE  */
    AGGR_MAX = 273,                /* AGGR_MAX  */
    AGGR_MIN = 274,                /* AGGR_MIN  */
    AGGR_SUM = 275,                /* AGGR_SUM  */
    AGGR_AVG = 276,                /* AGGR_AVG  */
    AGGR_COUNT = 277,              /* AGGR_COUNT  */
    LENGTH = 278,                  /* LENGTH  */
    ROUND = 279,                   /* ROUND  */
    DATE_FORMAT = 280,             /* DATE_FORMAT  */
    ASC = 281,                     /* ASC  */
    DESC = 282,                    /* DESC  */
    RBRACE = 283,                  /* RBRACE  */
    INNER = 284,                   /* INNER  */
    JOIN = 285,                    /* JOIN  */
    COMMA = 286,                   /* COMMA  */
    TRX_BEGIN = 287,               /* TRX_BEGIN  */
    TRX_COMMIT = 288,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 289,            /* TRX_ROLLBACK  */
    INT_T = 290,                   /* INT_T  */
    ORDER = 291,                   /* ORDER  */
    GROUP = 292,                   /* GROUP  */
    BY = 293,                      /* BY  */
    STRING_T = 294,                /* STRING_T  */
    TEXT_T = 295,                  /* TEXT_T  */
    FLOAT_T = 296,                 /* FLOAT_T  */
    DATE_T = 297,                  /* DATE_T  */
    HELP = 298,                    /* HELP  */
    EXIT = 299,                    /* EXIT  */
    DOT = 300,                     /* DOT  */
    INTO = 301,                    /* INTO  */
    VALUES = 302,                  /* VALUES  */
    FROM = 303,                    /* FROM  */
    WHERE = 304,                   /* WHERE  */
    AND = 305,                     /* AND  */
    OR = 306,                      /* OR  */
    SET = 307,                     /* SET  */
    ON = 308,                      /* ON  */
    LOAD = 309,                    /* LOAD  */
    DATA = 310,                    /* DATA  */
    INFILE = 311,                  /* INFILE  */
    EXPLAIN = 312,                 /* EXPLAIN  */
    EQ = 313,                      /* EQ  */
    LT = 314,                      /* LT  */
    GT = 315,                      /* GT  */
    LE = 316,                      /* LE  */
    GE = 317,                      /* GE  */
    NE = 318,                      /* NE  */
    NOT = 319,                     /* NOT  */
    IS = 320,                      /* IS  */
    AS = 321,                      /* AS  */
    LIKE = 322,                    /* LIKE  */
    EXISTS = 323,                  /* EXISTS  */
    IN = 324,                      /* IN  */
    DEFAULT = 325,                 /* DEFAULT  */
    HAVING = 326,                  /* HAVING  */
    NULL_T = 327,                  /* NULL_T  */
    NUMBER = 328,                  /* NUMBER  */
    FLOAT = 329,                   /* FLOAT  */
    ID = 330,                      /* ID  */
    SSS = 331,                     /* SSS  */
    DATE = 332,                    /* DATE  */
    UMINUS = 333                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 140 "yacc_sql.y"

  ParsedSqlNode *                   sql_node;
  PExpr *                           pexpr;
  PUnaryExpr *                      unary_pexpr;
  PArithmeticExpr *                 arith_pexpr;
  PConditionExpr *                  cond_pexpr;
  PFuncExpr *                       func_pexpr;
  PSubQueryExpr *                   subquery_pexpr;
  PListExpr *                       list_pexpr;
  Value *                           value;
  enum  CompOp                      comp;
  enum  AggrFuncType                aggr_func_type;
  Expression *                      expression;
  RelAttrSqlNode *                  rel_attr;
  AttrInfoSqlNode *                 attr_info;
  AssignmentSqlNode *               assignment;
  FromSqlNode *                     from_node;
  std::vector<PExpr *> *            pexpr_list;
  std::vector<PConditionExpr *> *   cond_pexpr_list;
  std::vector<AttrInfoSqlNode> *    attr_infos;
  std::vector<char *> *             string_list;
  std::vector<std::string> *        std_string_list;
  std::vector<Expression *> *       expression_list;
  std::vector<Value> *              value_list;
  std::vector<Relation> *           relation_list;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  std::vector<AssignmentSqlNode> *  assignment_list;
  std::vector<OrderBy> *            orderby_list;
  char *                            string;
  int                               number;
  float                             floats;

#line 175 "yacc_sql.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner);


#endif /* !YY_YY_YACC_SQL_HPP_INCLUDED  */
