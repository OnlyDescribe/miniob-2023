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
    RBRACE = 279,                  /* RBRACE  */
    INNER = 280,                   /* INNER  */
    JOIN = 281,                    /* JOIN  */
    COMMA = 282,                   /* COMMA  */
    TRX_BEGIN = 283,               /* TRX_BEGIN  */
    TRX_COMMIT = 284,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 285,            /* TRX_ROLLBACK  */
    INT_T = 286,                   /* INT_T  */
    STRING_T = 287,                /* STRING_T  */
    TEXT_T = 288,                  /* TEXT_T  */
    FLOAT_T = 289,                 /* FLOAT_T  */
    DATE_T = 290,                  /* DATE_T  */
    HELP = 291,                    /* HELP  */
    EXIT = 292,                    /* EXIT  */
    DOT = 293,                     /* DOT  */
    INTO = 294,                    /* INTO  */
    VALUES = 295,                  /* VALUES  */
    FROM = 296,                    /* FROM  */
    WHERE = 297,                   /* WHERE  */
    AND = 298,                     /* AND  */
    SET = 299,                     /* SET  */
    ON = 300,                      /* ON  */
    LOAD = 301,                    /* LOAD  */
    DATA = 302,                    /* DATA  */
    INFILE = 303,                  /* INFILE  */
    EXPLAIN = 304,                 /* EXPLAIN  */
    EQ = 305,                      /* EQ  */
    LT = 306,                      /* LT  */
    GT = 307,                      /* GT  */
    LE = 308,                      /* LE  */
    GE = 309,                      /* GE  */
    NE = 310,                      /* NE  */
    NOT = 311,                     /* NOT  */
    LIKE_CONDITION = 312,          /* LIKE_CONDITION  */
    NUMBER = 313,                  /* NUMBER  */
    FLOAT = 314,                   /* FLOAT  */
    ID = 315,                      /* ID  */
    SSS = 316,                     /* SSS  */
    DATE = 317,                    /* DATE  */
    UMINUS = 318                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 114 "yacc_sql.y"

  ParsedSqlNode *                   sql_node;
  ConditionSqlNode *                condition;
  Value *                           value;
  enum CompOp                       comp;
  enum AggrFuncType                 aggr_func_type;
  RelAttrSqlNode *                  rel_attr;
  JoinSqlNode *                     joins;
  std::vector<AttrInfoSqlNode> *    attr_infos;
  AttrInfoSqlNode *                 attr_info;
  Expression *                      expression;
  AssignmentSqlNode *               assignment;
  std::vector<Expression *> *       expression_list;
  std::vector<Value> *              value_list;
  std::vector<ConditionSqlNode> *   condition_list;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  std::vector<std::string> *        relation_list;
  char *                            string;
  std::vector<char *> *             string_list;
  std::vector<std::string> *        std_string_list;
  std::vector<AssignmentSqlNode> *  assignment_list;
  int                               number;
  float                             floats;
  std::vector<std::string>*         field_or_star_list;

#line 153 "yacc_sql.hpp"

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
