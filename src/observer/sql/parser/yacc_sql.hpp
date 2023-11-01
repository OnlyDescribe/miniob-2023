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
#define YY_YY_YACC_SQL_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
enum yytokentype
{
  YYEMPTY = -2,
  YYEOF = 0,          /* "end of file"  */
  YYerror = 256,      /* error  */
  YYUNDEF = 257,      /* "invalid token"  */
  SEMICOLON = 258,    /* SEMICOLON  */
  CREATE = 259,       /* CREATE  */
  DROP = 260,         /* DROP  */
  TABLE = 261,        /* TABLE  */
  TABLES = 262,       /* TABLES  */
  VIEW = 263,         /* VIEW  */
  INDEX = 264,        /* INDEX  */
  UNIQUE = 265,       /* UNIQUE  */
  CALC = 266,         /* CALC  */
  SELECT = 267,       /* SELECT  */
  SHOW = 268,         /* SHOW  */
  SYNC = 269,         /* SYNC  */
  INSERT = 270,       /* INSERT  */
  DELETE = 271,       /* DELETE  */
  UPDATE = 272,       /* UPDATE  */
  LBRACE = 273,       /* LBRACE  */
  AGGR_MAX = 274,     /* AGGR_MAX  */
  AGGR_MIN = 275,     /* AGGR_MIN  */
  AGGR_SUM = 276,     /* AGGR_SUM  */
  AGGR_AVG = 277,     /* AGGR_AVG  */
  AGGR_COUNT = 278,   /* AGGR_COUNT  */
  LENGTH = 279,       /* LENGTH  */
  ROUND = 280,        /* ROUND  */
  DATE_FORMAT = 281,  /* DATE_FORMAT  */
  ASC = 282,          /* ASC  */
  DESC = 283,         /* DESC  */
  RBRACE = 284,       /* RBRACE  */
  INNER = 285,        /* INNER  */
  JOIN = 286,         /* JOIN  */
  COMMA = 287,        /* COMMA  */
  TRX_BEGIN = 288,    /* TRX_BEGIN  */
  TRX_COMMIT = 289,   /* TRX_COMMIT  */
  TRX_ROLLBACK = 290, /* TRX_ROLLBACK  */
  INT_T = 291,        /* INT_T  */
  ORDER = 292,        /* ORDER  */
  GROUP = 293,        /* GROUP  */
  BY = 294,           /* BY  */
  STRING_T = 295,     /* STRING_T  */
  TEXT_T = 296,       /* TEXT_T  */
  FLOAT_T = 297,      /* FLOAT_T  */
  DATE_T = 298,       /* DATE_T  */
  HELP = 299,         /* HELP  */
  EXIT = 300,         /* EXIT  */
  DOT = 301,          /* DOT  */
  INTO = 302,         /* INTO  */
  VALUES = 303,       /* VALUES  */
  FROM = 304,         /* FROM  */
  WHERE = 305,        /* WHERE  */
  AND = 306,          /* AND  */
  OR = 307,           /* OR  */
  SET = 308,          /* SET  */
  ON = 309,           /* ON  */
  LOAD = 310,         /* LOAD  */
  DATA = 311,         /* DATA  */
  INFILE = 312,       /* INFILE  */
  EXPLAIN = 313,      /* EXPLAIN  */
  EQ = 314,           /* EQ  */
  LT = 315,           /* LT  */
  GT = 316,           /* GT  */
  LE = 317,           /* LE  */
  GE = 318,           /* GE  */
  NE = 319,           /* NE  */
  NOT = 320,          /* NOT  */
  IS = 321,           /* IS  */
  AS = 322,           /* AS  */
  LIKE = 323,         /* LIKE  */
  EXISTS = 324,       /* EXISTS  */
  IN = 325,           /* IN  */
  DEFAULT = 326,      /* DEFAULT  */
  HAVING = 327,       /* HAVING  */
  NULL_T = 328,       /* NULL_T  */
  NUMBER = 329,       /* NUMBER  */
  FLOAT = 330,        /* FLOAT  */
  ID = 331,           /* ID  */
  SSS = 332,          /* SSS  */
  DATE = 333,         /* DATE  */
  UMINUS = 334        /* UMINUS  */
};
typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 141 "yacc_sql.y"

  ParsedSqlNode *sql_node;
  PExpr *pexpr;
  PUnaryExpr *unary_pexpr;
  PArithmeticExpr *arith_pexpr;
  PConditionExpr *cond_pexpr;
  PFuncExpr *func_pexpr;
  PSubQueryExpr *subquery_pexpr;
  PListExpr *list_pexpr;
  Value *value;
  enum CompOp comp;
  enum AggrFuncType aggr_func_type;
  Expression *expression;
  RelAttrSqlNode *rel_attr;
  AttrInfoSqlNode *attr_info;
  AssignmentSqlNode *assignment;
  FromSqlNode *from_node;
  std::vector<PExpr *> *pexpr_list;
  std::vector<PConditionExpr *> *cond_pexpr_list;
  std::vector<AttrInfoSqlNode> *attr_infos;
  std::vector<char *> *string_list;
  std::vector<std::string> *std_string_list;
  std::vector<Expression *> *expression_list;
  std::vector<Value> *value_list;
  std::vector<Relation> *relation_list;
  std::vector<RelAttrSqlNode> *rel_attr_list;
  std::vector<AssignmentSqlNode> *assignment_list;
  std::vector<OrderBy> *orderby_list;
  char *string;
  int number;
  float floats;

#line 176 "yacc_sql.hpp"
};
typedef union YYSTYPE YYSTYPE;
#define YYSTYPE_IS_TRIVIAL 1
#define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if !defined YYLTYPE && !defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
#endif

int yyparse(const char *sql_string, ParsedSqlResult *sql_result, void *scanner);

#endif /* !YY_YY_YACC_SQL_HPP_INCLUDED  */
