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
    DESC = 267,                    /* DESC  */
    SHOW = 268,                    /* SHOW  */
    SYNC = 269,                    /* SYNC  */
    INSERT = 270,                  /* INSERT  */
    DELETE = 271,                  /* DELETE  */
    UPDATE = 272,                  /* UPDATE  */
    LBRACE = 273,                  /* LBRACE  */
    AGGR_MAX = 274,                /* AGGR_MAX  */
    AGGR_MIN = 275,                /* AGGR_MIN  */
    AGGR_SUM = 276,                /* AGGR_SUM  */
    AGGR_AVG = 277,                /* AGGR_AVG  */
    AGGR_COUNT = 278,              /* AGGR_COUNT  */
    LENGTH = 279,                  /* LENGTH  */
    ROUND = 280,                   /* ROUND  */
    DATE_FORMAT = 281,             /* DATE_FORMAT  */
    ASC = 282,                     /* ASC  */
    RBRACE = 283,                  /* RBRACE  */
    INNER = 284,                   /* INNER  */
    JOIN = 285,                    /* JOIN  */
    COMMA = 286,                   /* COMMA  */
    TRX_BEGIN = 287,               /* TRX_BEGIN  */
    TRX_COMMIT = 288,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 289,            /* TRX_ROLLBACK  */
    INT_T = 290,                   /* INT_T  */
    ORDER = 291,                   /* ORDER  */
    BY = 292,                      /* BY  */
    STRING_T = 293,                /* STRING_T  */
    TEXT_T = 294,                  /* TEXT_T  */
    FLOAT_T = 295,                 /* FLOAT_T  */
    DATE_T = 296,                  /* DATE_T  */
    HELP = 297,                    /* HELP  */
    EXIT = 298,                    /* EXIT  */
    DOT = 299,                     /* DOT  */
    INTO = 300,                    /* INTO  */
    VALUES = 301,                  /* VALUES  */
    FROM = 302,                    /* FROM  */
    WHERE = 303,                   /* WHERE  */
    AND = 304,                     /* AND  */
    SET = 305,                     /* SET  */
    ON = 306,                      /* ON  */
    LOAD = 307,                    /* LOAD  */
    DATA = 308,                    /* DATA  */
    INFILE = 309,                  /* INFILE  */
    EXPLAIN = 310,                 /* EXPLAIN  */
    EQ = 311,                      /* EQ  */
    LT = 312,                      /* LT  */
    GT = 313,                      /* GT  */
    LE = 314,                      /* LE  */
    GE = 315,                      /* GE  */
    NE = 316,                      /* NE  */
    NOT = 317,                     /* NOT  */
    IS = 318,                      /* IS  */
    DEFAULT = 319,                 /* DEFAULT  */
    NULL_T = 320,                  /* NULL_T  */
    LIKE_CONDITION = 321,          /* LIKE_CONDITION  */
    NUMBER = 322,                  /* NUMBER  */
    FLOAT = 323,                   /* FLOAT  */
    ID = 324,                      /* ID  */
    SSS = 325,                     /* SSS  */
    DATE = 326,                    /* DATE  */
    OR = 327,                      /* OR  */
    UMINUS = 328                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 123 "yacc_sql.y"

  ParsedSqlNode *                   sql_node;
  ConditionSqlNode *                condition;
  Value *                           value;
  enum CompOp                       comp;
  enum AggrFuncType                 aggr_func_type;
  RelAttrSqlNode *                  rel_attr;
  JoinSqlNode *                     joins;
  std::vector<AttrInfoSqlNode> *    attr_infos;
  std::vector<OrderBy> *            orderbys;
  AttrInfoSqlNode *                 attr_info;
  Expression *                      expression;
  PExpr *                           pexpr;
  PFuncExpr *                       func_pexpr;
  AssignmentSqlNode *               assignment;
  std::vector<Expression *> *       expression_list;
  std::vector<Value> *              value_list;
  std::vector<ConditionSqlNode> *   condition_list;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  std::vector<std::string> *        relation_list;
  WhereSqlNode *                    where_node;
  char *                            string;
  std::vector<char *> *             string_list;
  std::vector<std::string> *        std_string_list;
  std::vector<AssignmentSqlNode> *  assignment_list;
  int                               number;
  float                             floats;
  std::vector<std::string>*         field_or_star_list;

#line 167 "yacc_sql.hpp"

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
