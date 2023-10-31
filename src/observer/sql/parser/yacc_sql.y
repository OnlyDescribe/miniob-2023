
%{

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

%}

%define api.pure full
%define parse.error verbose
/** 启用位置标识 **/
%locations
%lex-param { yyscan_t scanner }
/** 这些定义了在yyparse函数中的参数 **/
%parse-param { const char * sql_string }
%parse-param { ParsedSqlResult * sql_result }
%parse-param { void * scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        INDEX
        UNIQUE
        CALC
        SELECT
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        AGGR_MAX 
        AGGR_MIN 
        AGGR_SUM
        AGGR_AVG
        AGGR_COUNT
        LENGTH
        ROUND
        DATE_FORMAT
        ASC
        DESC
        RBRACE
        INNER
        JOIN
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        ORDER
        GROUP
        BY
        STRING_T
        TEXT_T
        FLOAT_T
        DATE_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        SET
        ON
        LOAD
        DATA
        INFILE
        EXPLAIN
        EQ
        LT
        GT
        LE
        GE
        NE
        NOT
        IS
        AS
        LIKE
        EXISTS
        IN
        DEFAULT
        HAVING
        NULL_T

/** union 中定义各种数据类型，真实生成的代码也是union类型，所以不能有非POD类型的数据 **/
%union {
  ParsedSqlNode *                   sql_node;
  PExpr *                           pexpr;
  PUnaryExpr *                      unary_pexpr;
  PArithmeticExpr *                 arith_pexpr;
  PConditionExpr *                  cond_pexpr;
  PFuncExpr *                       func_pexpr;
  PSubQueryExpr *                   subquery_pexpr;
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
}

%token <number> NUMBER
%token <floats> FLOAT
%token <string> ID
%token <string> SSS
%token <string> DATE
//非终结符

/** type 定义了各种解析后的结果输出的是什么类型。类型对应了 union 中的定义的成员变量名称 **/
%type <number>              type
%type <value>               value
%type <string>              field_or_star
%type <std_string_list>     field_or_star_list
%type <number>              number
%type <std_string_list>     id_list
%type <rel_attr>            rel_attr
%type <aggr_func_type>      aggr_func_type
%type <attr_infos>          attr_def_list
%type <attr_info>           attr_def
%type <value_list>          value_list
%type <pexpr_list>          select_attr_list
%type <expression>          expression
%type <expression_list>     expression_list
%type <assignment_list>     assignment_list
%type <pexpr_list>          select_attr
%type <from_node>           select_from
%type <from_node>           from_list
%type <cond_pexpr>          inner_join
%type <cond_pexpr>          where
%type <rel_attr_list>       group_by
%type <rel_attr_list>       group_by_list
%type <cond_pexpr>          having
%type <orderby_list>        order_by
%type <orderby_list>        order_condtions
%type <pexpr>               pexpr
%type <unary_pexpr>         unary_pexpr
%type <arith_pexpr>         arith_pexpr
%type <func_pexpr>          func_pexpr
%type <cond_pexpr>          cond_pexpr
%type <subquery_pexpr>      subquery_pexpr
%type <sql_node>            calc_stmt
%type <sql_node>            select_stmt
%type <sql_node>            insert_stmt
%type <sql_node>            update_stmt
%type <sql_node>            delete_stmt
%type <sql_node>            create_table_stmt
%type <sql_node>            drop_table_stmt
%type <sql_node>            show_tables_stmt
%type <sql_node>            desc_table_stmt
%type <sql_node>            create_index_stmt
%type <sql_node>            drop_index_stmt
%type <sql_node>            sync_stmt
%type <sql_node>            begin_stmt
%type <sql_node>            commit_stmt
%type <sql_node>            rollback_stmt
%type <sql_node>            load_data_stmt
%type <sql_node>            explain_stmt
%type <sql_node>            set_variable_stmt
%type <sql_node>            help_stmt
%type <sql_node>            exit_stmt
%type <sql_node>            command_wrapper
// commands should be a list but I use a single command instead
%type <sql_node>            commands

/* %expect 0 */
%left COMMA
%left OR
%left AND
%left EQ NE LIKE IS AS EXISTS  
%left IN NOT
%left LT GT LE GE
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS
%%

commands: command_wrapper opt_semicolon  //commands or sqls. parser starts here.
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>($1);
    sql_result->add_sql_node(std::move(sql_node));
  }
  ;

command_wrapper:
    calc_stmt
  | select_stmt
  | insert_stmt
  | update_stmt
  | delete_stmt
  | create_table_stmt
  | drop_table_stmt
  | show_tables_stmt
  | desc_table_stmt
  | create_index_stmt
  | drop_index_stmt
  | sync_stmt
  | begin_stmt
  | commit_stmt
  | rollback_stmt
  | load_data_stmt
  | explain_stmt
  | set_variable_stmt
  | help_stmt
  | exit_stmt
    ;

exit_stmt:      
    EXIT {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      $$ = new ParsedSqlNode(SCF_EXIT);
    };

help_stmt:
    HELP {
      $$ = new ParsedSqlNode(SCF_HELP);
    };

sync_stmt:
    SYNC {
      $$ = new ParsedSqlNode(SCF_SYNC);
    }
    ;

begin_stmt:
    TRX_BEGIN  {
      $$ = new ParsedSqlNode(SCF_BEGIN);
    }
    ;

commit_stmt:
    TRX_COMMIT {
      $$ = new ParsedSqlNode(SCF_COMMIT);
    }
    ;

rollback_stmt:
    TRX_ROLLBACK  {
      $$ = new ParsedSqlNode(SCF_ROLLBACK);
    }
    ;

drop_table_stmt:    /*drop table 语句的语法解析树*/
    DROP TABLE ID {
      $$ = new ParsedSqlNode(SCF_DROP_TABLE);
      $$->drop_table.relation_name = $3;
      free($3);
    };

show_tables_stmt:
    SHOW TABLES {
      $$ = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
    ;

desc_table_stmt:
    DESC ID  {
      $$ = new ParsedSqlNode(SCF_DESC_TABLE);
      $$->desc_table.relation_name = $2;
      free($2);
    }
    ;

load_data_stmt:
    LOAD DATA INFILE SSS INTO TABLE ID 
    {
      char *tmp_file_name = common::substr($4, 1, strlen($4) - 2);
      
      $$ = new ParsedSqlNode(SCF_LOAD_DATA);
      $$->load_data.relation_name = $7;
      $$->load_data.file_name = tmp_file_name;
      free($7);
      free(tmp_file_name);
    }
    ;

explain_stmt:
    EXPLAIN command_wrapper
    {
      $$ = new ParsedSqlNode(SCF_EXPLAIN);
      $$->explain.sql_node = std::unique_ptr<ParsedSqlNode>($2);
    }
    ;

set_variable_stmt:
    SET ID EQ value
    {
      $$ = new ParsedSqlNode(SCF_SET_VARIABLE);
      $$->set_variable.name  = $2;
      $$->set_variable.value = *$4;
      free($2);
      delete $4;
    }
    ;

create_index_stmt:    /*create index 语句的语法解析树*/
    CREATE INDEX ID ON ID LBRACE ID id_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      create_index.index_name = $3;
      create_index.relation_name = $5;
      create_index.is_unique = false;
      
      std::vector<std::string> *attribute_names = $8;

      if (attribute_names != nullptr) {
        create_index.attribute_name.swap(*attribute_names);
      }

      create_index.attribute_name.emplace_back($7);
      std::reverse(create_index.attribute_name.begin(), create_index.attribute_name.end());

      free($3);
      free($5);
      free($7);
      free($8);
    }
    | CREATE UNIQUE INDEX ID ON ID LBRACE ID id_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      create_index.index_name = $4;
      create_index.relation_name = $6;
      create_index.is_unique = true;
      
      std::vector<std::string> *attribute_names = $9;

      if (attribute_names != nullptr) {
        create_index.attribute_name.swap(*attribute_names);
      }

      create_index.attribute_name.emplace_back($8);
      std::reverse(create_index.attribute_name.begin(), create_index.attribute_name.end());

      free($4);
      free($6);
      free($8);
      free($9);
    }
    ;

drop_index_stmt:      /*drop index 语句的语法解析树*/
    DROP INDEX ID ON ID
    {
      $$ = new ParsedSqlNode(SCF_DROP_INDEX);
      $$->drop_index.index_name = $3;
      $$->drop_index.relation_name = $5;
      free($3);
      free($5);
    }
    ;
create_table_stmt:    /*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      std::vector<AttrInfoSqlNode> *src_attrs = $6;

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*$5);
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete $5;
    }
    ;

insert_stmt:        /*insert   语句的语法解析树*/
    INSERT INTO ID VALUES LBRACE value value_list RBRACE 
    {
      $$ = new ParsedSqlNode(SCF_INSERT);
      $$->insertion.relation_name = $3;
      if ($7 != nullptr) {
        $$->insertion.values.swap(*$7);
      }
      $$->insertion.values.emplace_back(*$6);
      std::reverse($$->insertion.values.begin(), $$->insertion.values.end());
      delete $6;
      free($3);
    }
    ;

delete_stmt:    /*  delete 语句的语法解析树*/
    DELETE FROM ID where 
    {
      $$ = new ParsedSqlNode(SCF_DELETE);
      $$->deletion.relation_name = $3;
      $$->deletion.conditions = $4;
      free($3);
    }
    ;

update_stmt:      /*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value assignment_list where 
    {
      $$ = new ParsedSqlNode(SCF_UPDATE);
      $$->update.relation_name = $2;

      if ($7 != nullptr) {
        $$->update.assignments.swap(*$7);
        delete $7;
      }

      $$->update.conditions = $8;
      
      AssignmentSqlNode node = {
        .attribute_name = std::string{$4},
        .value = *$6
      };

      $$->update.assignments.push_back(std::move(node));
      std::reverse($$->update.assignments.begin(), $$->update.assignments.end());

      free($2);
      free($4);
      free($6);
    }
    ;

select_stmt:        /*  select 语句的语法解析树*/
    SELECT select_attr FROM select_from where group_by having order_by
    {
      $$ = new ParsedSqlNode(SCF_SELECT);

      if ($2 != nullptr) {
        $$->selection.attributes.swap(*$2);
        delete $2;
        if (!IsAttributesVailid($$->selection.attributes)) {
          yyerror(&@$, sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }

      if($4 != nullptr)
      {
        FromSqlNode* from_node = $4;
        $$->selection.relations.swap(from_node->relations);
        $$->selection.join_conds.swap(from_node->join_conds);
        delete $4;
      }

      $$->selection.conditions = $5;

      if ($6 != nullptr) {
        $$->selection.groupbys = *$6;
        delete $6;
      }

      $$->selection.havings = $7;

      if ($8 != nullptr) {
        $$->selection.orderbys = *$8;
        delete $8;
      }
    }
    ;

calc_stmt:
    CALC expression_list
    {
      $$ = new ParsedSqlNode(SCF_CALC);
      std::reverse($2->begin(), $2->end());
      $$->calc.expressions.swap(*$2);
      delete $2;
    }
    ;



assignment_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA ID EQ value assignment_list
    {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<AssignmentSqlNode>;
      }

      AssignmentSqlNode node = {
        .attribute_name = std::string{$2},
        .value = *$4
      };

      $$->push_back(std::move(node));
      free($2);
      delete $4;
    }
    ;

select_attr:
    pexpr select_attr_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<PExpr *>;
      }
      $$->push_back($1);
      // std::reverse($$->begin(), $$->end());
    }
    | pexpr ID select_attr_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<PExpr *>;
      }
      PExpr * pexpr = $1;
      pexpr->alias = $2;
      $$->push_back(pexpr);
      // std::reverse($$->begin(), $$->end());
      free($2);
    }
    | pexpr AS ID select_attr_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<PExpr *>;
      }
      PExpr * pexpr = $1;
      pexpr->alias = $3;
      $$->push_back(pexpr);
      // std::reverse($$->begin(), $$->end());
      free($3);
    }
    ;

select_attr_list:
    {
      $$ = nullptr;
    }
    | COMMA pexpr select_attr_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<PExpr *>;
      }
      $$->push_back($2);
    }
    | COMMA pexpr ID select_attr_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<PExpr *>;
      }
      PExpr * pexpr = $2;
      pexpr->alias = $3;
      $$->push_back(pexpr);
      free($3);
    }
    | COMMA pexpr AS ID select_attr_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<PExpr *>;
      }
      PExpr * pexpr = $2;
      pexpr->alias = $4;
      $$->push_back(pexpr);
      free($4);
    }
    ;

select_from:   
    ID from_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $1;
      $$->relations.push_back(std::move(rel));
      std::reverse($$->relations.begin(), $$->relations.end());
      std::reverse($$->join_conds.begin(), $$->join_conds.end());
      free($1);
    }
    | ID ID from_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $1;
      rel.alias = $2;
      $$->relations.push_back(std::move(rel));
      std::reverse($$->relations.begin(), $$->relations.end());
      std::reverse($$->join_conds.begin(), $$->join_conds.end());
      free($1);
      free($2);
    }
    | ID AS ID from_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $1;
      rel.alias = $3;
      $$->relations.push_back(std::move(rel));
      std::reverse($$->relations.begin(), $$->relations.end());
      std::reverse($$->join_conds.begin(), $$->join_conds.end());
      free($1);
      free($3);
    }
    ;
from_list:
    {
      $$ = nullptr;
    }
    | COMMA ID from_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $2;
      $$->relations.push_back(rel);
      free($2);
    }
    | COMMA ID ID from_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $2;
      rel.alias = $3;
      $$->relations.push_back(rel);
      free($2);
      free($3);
    }
    | COMMA ID AS ID from_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $2;
      rel.alias = $4;
      $$->relations.push_back(rel);
      free($2);
      free($4);
    }
    | INNER JOIN ID inner_join from_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $3;
      $$->relations.push_back(rel);
      $$->join_conds.push_back($4);
      free($3);
    }
    | INNER JOIN ID ID inner_join from_list {
      if ($6 != nullptr) {
        $$ = $6;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $3;
      rel.alias = $4;
      $$->relations.push_back(rel);
      $$->join_conds.push_back($5);
      free($3);
      free($4);
    }
    | INNER JOIN ID AS ID inner_join from_list {
      if ($7 != nullptr) {
        $$ = $7;
      } else {
        $$ = new FromSqlNode;
      }
      Relation rel;
      rel.relation_name = $3;
      rel.alias = $5;
      $$->relations.push_back(rel);
      $$->join_conds.push_back($6);
      free($3);
      free($5);
    }
    ;

inner_join:
    {
      $$ = nullptr;
    }
    | ON cond_pexpr{
      $$ = $2;
    }
    ;


where:
    /* empty */
    {
      $$ = nullptr;
    }
    | WHERE cond_pexpr {
      $$ = $2;  
    } 
    ;

group_by:
    {
      $$ = nullptr;
    }
    | GROUP BY ID group_by_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.attribute_name = $3;
      $$->push_back(rel_attr);
      std::reverse($$->begin(), $$->end());
      free($3);
    }
    | GROUP BY ID DOT ID group_by_list {
      if ($6 != nullptr) {
        $$ = $6;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.relation_name = $3;
      rel_attr.attribute_name = $5;
      $$->push_back(rel_attr);
      std::reverse($$->begin(), $$->end());
      free($3);
      free($5);
    }
    ;

group_by_list:
    { 
      $$ = nullptr;
    }
    | COMMA ID group_by_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.attribute_name = $2;
      rel_attr.relation_name = $2;
      $$->push_back(rel_attr);
      free($2);
    }
    | COMMA ID DOT ID group_by_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      RelAttrSqlNode rel_attr;
      rel_attr.relation_name = $2;
      rel_attr.attribute_name = $4;
      $$->push_back(rel_attr);
      free($2);
      free($4);
    }
    ;

having:
    /* empty */
    {
      $$ = nullptr;
    }
    | HAVING cond_pexpr {
      $$ = $2;  
    } 
    ;

order_by:
    /* empty */
    {
      $$ = nullptr;
    }
    | ORDER BY rel_attr order_condtions {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *$3;
      order.sort_type = SortType::ASC;
      $$->push_back(order);
      std::reverse($$->begin(), $$->end());
      delete($3);
    }
    | ORDER BY rel_attr DESC order_condtions {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *$3;
      order.sort_type = SortType::DESC;
      $$->push_back(order);
      std::reverse($$->begin(), $$->end());
      delete($3);
    }
    | ORDER BY rel_attr ASC order_condtions {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *$3;
      order.sort_type = SortType::ASC;
      $$->push_back(order);
      std::reverse($$->begin(), $$->end());
      delete($3);
    }
    ;

order_condtions:
    {
      $$ = nullptr;
    }
    | COMMA rel_attr order_condtions {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *$2;
      order.sort_type = SortType::ASC;
      $$->push_back(order);
      delete($2);
    }
    | COMMA rel_attr DESC order_condtions {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *$2;
      order.sort_type = SortType::DESC;
      $$->push_back(order);
      delete($2);
    }
    | COMMA rel_attr ASC order_condtions {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<OrderBy>;
      }
      OrderBy order;
      order.attr = *$2;
      order.sort_type = SortType::ASC;
      $$->push_back(order);
      delete($2);
    }
    ;

subquery_pexpr:        /*  select 语句的语法解析树*/
    LBRACE SELECT select_attr FROM select_from where group_by having order_by RBRACE
    {
      $$ = new PSubQueryExpr;
      $$->sub_select = new SelectSqlNode();
      SelectSqlNode * sub_select = $$->sub_select;
      if ($3 != nullptr) {
        sub_select->attributes.swap(*$3);
        delete $3;
        if (!IsAttributesVailid(sub_select->attributes)) {
          yyerror(&@$, sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }

      if($5 != nullptr)
      {
        FromSqlNode* from_node = $5;
        sub_select->relations.swap(from_node->relations);
        sub_select->join_conds.swap(from_node->join_conds);
        delete $5;
      }

      sub_select->conditions = $6;

      if ($7 != nullptr) {
        sub_select->groupbys = *$7;
        delete $7;
      }

      sub_select->havings = $8;

      if ($9 != nullptr) {
        sub_select->orderbys = *$9;
        delete $9;
      }
    }
    ;


id_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA ID id_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<std::string>;
      }
      $$->emplace_back($2);
      free($2);
    }
    ;

attr_def_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA attr_def attr_def_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<AttrInfoSqlNode>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
    
attr_def:
    ID type LBRACE number RBRACE 
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      $$->is_not_null = false;
      free($1);
    }
    | ID type LBRACE number RBRACE NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      $$->is_not_null = false;
      free($1);
    }
    | ID type LBRACE number RBRACE DEFAULT NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      $$->is_not_null = false;
      free($1);
    }
    | ID type LBRACE number RBRACE NOT NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      $$->is_not_null = true;
      free($1);
    }
    | ID type
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->is_not_null = false;
      if($$->type == AttrType::TEXTS)
      {
        $$->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        $$->length = 4;
      }
      free($1);
    }
    | ID type NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->is_not_null = false;
      if($$->type == AttrType::TEXTS)
      {
        $$->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        $$->length = 4;
      }
      free($1);
    }
    | ID type DEFAULT NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->is_not_null = false;
      if($$->type == AttrType::TEXTS)
      {
        $$->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        $$->length = 4;
      }
      free($1);
    }
    | ID type NOT NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->is_not_null = true;
      if($$->type == AttrType::TEXTS)
      {
        $$->length = 68; // 字段长度为68， 在record中存储为16+1个指向文本数据的溢出页
      }
      else{
        $$->length = 4;
      }
      free($1);
    }
    ;
number:
    NUMBER {$$ = $1;}
    ;
type:
    INT_T      { $$=AttrType::INTS; }
    | STRING_T { $$=AttrType::CHARS; }
    | TEXT_T   { $$=AttrType::TEXTS; }
    | FLOAT_T  { $$=AttrType::FLOATS; }
    | DATE_T   { $$=AttrType::DATES; }
    ;


value_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA value value_list  { 
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Value>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
value:
    NUMBER {
      $$ = new Value((int)$1);
      @$ = @1;
    }
    |FLOAT {
      $$ = new Value((float)$1);
      @$ = @1;
    }
    |SSS {
      char *tmp = common::substr($1,1,strlen($1)-2);
      if(strlen(tmp) > 65535){
        yyerror(&@$, sql_string, sql_result, scanner, "invalid text", SCF_INVALID);
      }
      $$ = new Value(tmp);
      free(tmp);
    }
    |NULL_T {
      $$ = new Value(AttrType::NULLS);
      @$ = @1;
    }
    |DATE {
      char *tmp = common::substr($1,1,strlen($1)-2);
      $$ = new Value(tmp, DATES);
      free(tmp);
      if(!$$->check_date()){
        yyerror(&@$, sql_string, sql_result, scanner, "invalid date", SCF_INVALID);
      }
    }
    ;
    

aggr_func_type: 
    AGGR_MAX { $$ = AggrFuncType::MAX; }
    | AGGR_MIN { $$ = AggrFuncType::MIN; }
    | AGGR_SUM { $$ = AggrFuncType::SUM; }
    | AGGR_AVG { $$ = AggrFuncType::AVG; }
    | AGGR_COUNT { $$ = AggrFuncType::COUNT; }
    ;
field_or_star:
  '*' {
    $$ = strdup("*");
  }
  | ID {
    $$ = $1;
  }
  | ID DOT ID {
    std::string res = $1;
    res += ".";
    res += $3;
    $$ = strdup(res.c_str());
  }
  ;
field_or_star_list:
    {
      $$ = nullptr;
    }
    | field_or_star field_or_star_list {
      $$ = new std::vector<std::string>;
      $$->emplace_back($1);
      if ($2 != nullptr) {
        $$->insert($$->end(), $2->begin(), $2->end());
        delete $2;
      }
    }
    | COMMA field_or_star field_or_star_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<std::string>;
      }
      $$->emplace_back($2);
    }
    ;

rel_attr:
    '*' {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = "";
      $$->attribute_name = "*";
    }
    | ID DOT '*' {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = "*";
      free($1);
    }
    | ID {
      $$ = new RelAttrSqlNode;
      $$->attribute_name = $1;
      free($1);
    }
    | ID DOT ID {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = $3;
      free($1);
      free($3);
    }
    | aggr_func_type LBRACE field_or_star_list RBRACE {
      $$ = new RelAttrSqlNode;
      $$->aggr_type = $1;
      if ($3 != nullptr) {
        $$->aggregates = *$3;
        delete $3;
      }
    }
    ;
  
expression_list:
    expression
    {
      $$ = new std::vector<Expression*>;
      $$->emplace_back($1);
    }
    | expression COMMA expression_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $$->emplace_back($1);
    }
    ;

expression:
    expression '+' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::ADD, $1, $3, sql_string, &@$);
    }
    | expression '-' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::SUB, $1, $3, sql_string, &@$);
    }
    | expression '*' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::MUL, $1, $3, sql_string, &@$);
    }
    | expression '/' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::DIV, $1, $3, sql_string, &@$);
    }
    | LBRACE expression RBRACE {
      $$ = $2;
      $$->set_name(token_name(sql_string, &@$));
    }
    | '-' expression %prec UMINUS {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, $2, nullptr, sql_string, &@$);
    }
    | value {
      $$ = new ValueExpr(*$1);
      $$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    ;


unary_pexpr:
  value {
    PUnaryExpr *unary_pexpr = new PUnaryExpr;
    unary_pexpr->is_attr = false;
    unary_pexpr->value = *$1;
    $$ = unary_pexpr;
    delete $1;
  }
  | rel_attr {
    PUnaryExpr *unary_pexpr = new PUnaryExpr;
    unary_pexpr->is_attr = true;
    unary_pexpr->attr = *$1;
    $$ = unary_pexpr;
    delete $1;
  }

cond_pexpr:
    pexpr OR pexpr{
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::OR, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr AND pexpr{
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::AND, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr EQ pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::EQUAL_TO, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr NE pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_EQUAL, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr LT pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::LESS_THAN, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr GT pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::GREAT_THAN, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr LE pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::LESS_EQUAL, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr GE pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::GREAT_EQUAL, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr LIKE pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::LIKE, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr NOT LIKE pexpr
    {
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_LIKE, $1, $4);
      $$ = condition_pexpr;
    }
    | pexpr IS NULL_T
    {
      PUnaryExpr *unary_pexpr = new PUnaryExpr;
      unary_pexpr->is_attr = false;
      unary_pexpr->value = Value(AttrType::NULLS);
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::UNARY;
      pexpr->uexp = unary_pexpr;
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IS_NULL, $1, pexpr);
      $$ = condition_pexpr;
    }
    | pexpr IS NOT NULL_T
    {
      PUnaryExpr *unary_pexpr = new PUnaryExpr;
      unary_pexpr->is_attr = false;
      unary_pexpr->value = Value(AttrType::NULLS);
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::UNARY;
      pexpr->uexp = unary_pexpr;
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IS_NOT_NULL, $1, pexpr);
      $$ = condition_pexpr;
    }
    | pexpr IN pexpr{
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::IN, $1, $3);
      $$ = condition_pexpr;
    }
    | pexpr NOT IN pexpr{
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_IN, $1, $4);
      $$ = condition_pexpr;
    }
    | EXISTS pexpr{
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::EXISTS, nullptr, $2);
      $$ = condition_pexpr;
    }
    | NOT EXISTS pexpr{
      PConditionExpr *condition_pexpr = new PConditionExpr(CompOp::NOT_EXISTS, nullptr, $3);
      $$ = condition_pexpr;
    }
    ;

  arith_pexpr:
      pexpr '+' pexpr {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::ADD, $1, $3);
          $$ = arith_pexpr;
      }
      | pexpr '-' pexpr {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::SUB, $1, $3);
          $$ = arith_pexpr;
      }
      | pexpr '*' pexpr {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::MUL, $1, $3);
          $$ = arith_pexpr;
      }
      | pexpr '/' pexpr {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::DIV, $1, $3);
          $$ = arith_pexpr;
      }
      | '-' pexpr %prec UMINUS {
          PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::NEGATIVE, $2, nullptr);
          $$ = arith_pexpr;
      }

func_pexpr:
    LENGTH LBRACE pexpr RBRACE {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::LENGTH;
      func_pexpr->params.push_back($3);
      $$ = func_pexpr;
    }
    | ROUND LBRACE pexpr RBRACE
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::ROUND;
      func_pexpr->params.push_back($3);
      $$ = func_pexpr;
    }
    | ROUND LBRACE pexpr COMMA pexpr RBRACE
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::ROUND;
      func_pexpr->params.push_back($3);
      func_pexpr->params.push_back($5);
      $$ = func_pexpr;
    }
    | DATE_FORMAT LBRACE pexpr COMMA pexpr RBRACE
    {
      PFuncExpr *func_pexpr = new PFuncExpr;
      func_pexpr->type = PFuncType::DATE_FORMAT;
      func_pexpr->params.push_back($3);
      func_pexpr->params.push_back($5);
      $$ = func_pexpr;
    }
    ;

pexpr:
    cond_pexpr {
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::COMPARISON;
      pexpr->cexp = $1;
      pexpr->name = token_name(sql_string, &@$);
      $$ = pexpr;
    }
    | arith_pexpr {      
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::ARITHMETIC;
      pexpr->aexp = $1;
      pexpr->name = token_name(sql_string, &@$);
      $$ = pexpr;
    }
    | LBRACE pexpr RBRACE {
      $$ = $2;
      $$->name = token_name(sql_string, &@$);
    }
    | unary_pexpr {
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::UNARY;
      pexpr->uexp = $1;
      pexpr->name = token_name(sql_string, &@$);
      $$ = pexpr;
    }
    | func_pexpr {        
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::FUNC;
        pexpr->fexp = $1;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | subquery_pexpr { 
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::SUBQUERY;
        pexpr->sexp = $1;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    ;

opt_semicolon: /*empty*/
    | SEMICOLON
    ;
%%
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
