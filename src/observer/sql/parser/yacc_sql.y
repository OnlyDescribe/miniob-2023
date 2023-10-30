
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
        DESC
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
        RBRACE
        INNER
        JOIN
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        ORDER
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
        DEFAULT
        NULL_T
        LIKE_CONDITION

/** union 中定义各种数据类型，真实生成的代码也是union类型，所以不能有非POD类型的数据 **/
%union {
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
}

%token <number> NUMBER
%token <floats> FLOAT
%token <string> ID
%token <string> SSS
%token <string> DATE
//非终结符

/** type 定义了各种解析后的结果输出的是什么类型。类型对应了 union 中的定义的成员变量名称 **/
%type <number>              type
%type <condition>           condition
%type <value>               value
%type <string>              field_or_star
%type <orderbys>            order_condtions
%type <field_or_star_list>  field_or_star_list
%type <number>              number
%type <comp>                comp_op
%type <std_string_list>     id_list
%type <rel_attr>            rel_attr
%type <aggr_func_type>      aggr_func_type
%type <attr_infos>          attr_def_list
%type <attr_info>           attr_def
%type <value_list>          value_list
%type <where_node>          where
%type <condition_list>      condition_list
%type <rel_attr_list>       select_attr
%type <relation_list>       rel_list
%type <joins>               inner_join
%type <rel_attr_list>       attr_list
%type <expression>          expression
%type <expression_list>     expression_list
%type <assignment_list>     assignment_list
%type <pexpr>               pexpr
%type <func_pexpr>          func_pexpr
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

%left OR
%left AND
%left '+' '-'
%left '*' '/' 

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
      delete $2;
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
    
delete_stmt:    /*  delete 语句的语法解析树*/
    DELETE FROM ID where 
    {
      $$ = new ParsedSqlNode(SCF_DELETE);
      $$->deletion.relation_name = $3;
      if ($4 != nullptr) {
        $$->deletion.conditions.swap($4->conditions);
        delete $4;
      }
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
      if ($8 != nullptr) {
        $$->update.conditions.swap($8->conditions);
        delete $8;
      }

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
      delete $2;
      delete $4;
    }
    ;
inner_join:   // in
    {
      $$ = nullptr;
    }
    | INNER JOIN ID ON condition_list inner_join
    {
      $$ = new JoinSqlNode;
      $$->relations.emplace_back($3);
      $$->join_conds.push_back(*$5);
      if ($6 != nullptr) {
        $$->relations.insert($$->relations.end(), $6->relations.begin(), $6->relations.end());
        $$->join_conds.insert($$->join_conds.end(), $6->join_conds.begin(), $6->join_conds.end());
        delete $6;
      }
      delete $3;
      delete $5;
    }
    ;

select_stmt:        /*  select 语句的语法解析树*/
    SELECT select_attr FROM ID inner_join where
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.attributes.swap(*$2);
        delete $2;
        if (!$$->selection.IsAttributesVailid()) {
          yyerror(&@$, sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }
      // 还需要连接的一个表时relations
      $$->selection.relations.push_back($4);
      if ($5 != nullptr) {
        $$->selection.relations.insert($$->selection.relations.end(), $5->relations.begin(), $5->relations.end());
        $$->selection.join_conds.insert($$->selection.join_conds.end(), $5->join_conds.begin(), $5->join_conds.end());
        delete $5;
      }
      if ($6 != nullptr) {
        $$->selection.conditions = std::move($6->conditions);
        $$->selection.orderbys.swap($6->orderbys);
        delete $6;
      }
      delete $4;
    }
    | SELECT select_attr FROM ID rel_list where
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.attributes.swap(*$2);
        delete $2;
        if (!$$->selection.IsAttributesVailid()) {
          yyerror(&@$, sql_string, sql_result, scanner, "invalid aggr func", SCF_INVALID);
        }
      }
      if ($5 != nullptr) {
        $$->selection.relations.swap(*$5);
        delete $5;
      }
      $$->selection.relations.push_back($4);
      std::reverse($$->selection.relations.begin(), $$->selection.relations.end());

      // TODO(oldcb): 直接传进来, 不用一个个移动了
      if ($6 != nullptr) {
        $$->selection.conditions = std::move($6->conditions);
        $$->selection.orderbys.swap($6->orderbys);
        delete $6;
      }
      free($4);
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
/*  TODO(oldcb): 和PEXPR 冲突*/ 
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

pexpr:
    pexpr OR pexpr {
        ConditionSqlNode *condition_pexpr = new ConditionSqlNode(CompOp::OR, $1, $3);
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::COMPARISON;
        pexpr->cexp = condition_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | pexpr AND pexpr {
        ConditionSqlNode *condition_pexpr = new ConditionSqlNode(CompOp::AND, $1, $3);
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::COMPARISON;
        pexpr->cexp = condition_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | pexpr '+' pexpr {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::ADD, $1, $3);
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::ARITHMETIC;
        pexpr->aexp = arith_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | pexpr '-' pexpr {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::SUB, $1, $3);
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::ARITHMETIC;
        pexpr->aexp = arith_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | pexpr '*' pexpr {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::MUL, $1, $3);
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::ARITHMETIC;
        pexpr->aexp = arith_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | pexpr '/' pexpr {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::DIV, $1, $3);
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::ARITHMETIC;
        pexpr->aexp = arith_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | LBRACE pexpr RBRACE {
      $$ = $2;
      $$->name(token_name(sql_string, &@$));
    }
    | pexpr comp_op pexpr {
      ConditionSqlNode *condition_pexpr = new ConditionSqlNode(comp_op, $1, $3);
      PExpr *pexpr = new PExpr;
      pexpr->type = PExpType::COMPARISON;
      pexpr->cexp = condition_pexpr;
      pexpr->name = token_name(sql_string, &@$);
      $$ = pexpr;
    }
    | '-' pexpr %prec UMINUS {
        PArithmeticExpr *arith_pexpr = new PArithmeticExpr(PArithmeticType::NEGATIVE, $2, nullptr);
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::ARITHMETIC;
        pexpr->aexp = arith_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | value {
        PUnaryExpr *unary_pexpr = new PUnaryExpr;
        unary_pexpr->is_attr = false;
        unary_pexpr->left_value = *$1;
        
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::UNARY;
        pexpr->uexp = unary_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
        delete $1;
    }
    | ID {
        PUnaryExpr *unary_pexpr = new PUnaryExpr;
        unary_pexpr->is_attr = true;
        unary_pexpr->left_attr->attribute_name = $1;
        
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::UNARY;
        pexpr->uexp = unary_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | ID DOT ID{
        PUnaryExpr *unary_pexpr = new PUnaryExpr;
        unary_pexpr->is_attr = true;
        unary_pexpr->left_attr->relation_name = $1;
        unary_pexpr->left_attr->attribute_name = $3;
        
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::UNARY;
        pexpr->uexp = unary_pexpr;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    | func_pexpr{        
        PExpr *pexpr = new PExpr;
        pexpr->type = PExpType::FUNC;
        pexpr->fexp = $1;
        pexpr->name = token_name(sql_string, &@$);
        $$ = pexpr;
    }
    ;

func_pexpr:
  LENGTH LBRACE pexpr RBRACE
  {
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
  
select_attr:
    rel_attr attr_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      $$->emplace_back(*$1);
      delete $1;
    }
    | pexpr attr_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      $$->emplace_back(*$1);
      delete $1;
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
    $$ = "*";
  }
  | ID {
    $$ = $1;
  }
  ;
/* TODO: 可能有内存泄漏 */
field_or_star_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | field_or_star field_or_star_list {
      $$ = new std::vector<std::string>;
      $$->emplace_back($1);
      if ($2 != nullptr) {
        $$->insert($$->end(), $2->begin(), $2->end());
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

/*  TODO(oldcb): 注意 PEXPR 包含了ID, aggr_func_type 放进 PEXPR*/ 
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
attr_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA rel_attr attr_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }

      $$->emplace_back(*$2);
      delete $2;
    }
    ;

rel_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA ID rel_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<std::string>;
      }

      $$->push_back($2);
      free($2);
    }
    ;
order_condtions:
    {
      $$ = nullptr;
    }
    | rel_attr order_condtions {
      $$ = new vector<OrderBy>(1);
      $$->back().attr = *$1;
      $$->back().sort_type = SortType::ASC;
      if ($2 != nullptr) {
        $$->insert($$->end(), $2->begin(), $2->end());
        delete $2;
      }
      delete $1;
    }
    | rel_attr DESC order_condtions {
      $$ = new vector<OrderBy>(1);
      $$->back().attr = *$1;
      $$->back().sort_type = SortType::DESC;
      if ($3 != nullptr) {
        $$->insert($$->end(), $3->begin(), $3->end());
        delete $3;
      }
      delete $1;
    }
    | rel_attr ASC order_condtions {
      $$ = new vector<OrderBy>(1);
      $$->back().attr = *$1;
      $$->back().sort_type = SortType::ASC;
      if ($3 != nullptr) {
        $$->insert($$->end(), $3->begin(), $3->end());
        delete $3;
      }
      delete $1;
    }
    | COMMA order_condtions {
      $$ = $2;
    } 
    ;
where:
    /* empty */
    {
      $$ = nullptr;
    }
    | WHERE condition_list {
      $$ = new WhereSqlNode;
      $$->conditions = *$2;  
      delete $2;
    } 
    /* TODO: ORDER BY order_condtions 这些语句放一个节点里面 */
    | WHERE condition_list ORDER BY order_condtions {
      $$ = new WhereSqlNode;
      $$->conditions = *$2;  
      if ($5 != nullptr) {
        $$->orderbys = *$5;
        delete $5;
      }
      delete $2;
    } 
    | ORDER BY order_condtions {
      $$ = new WhereSqlNode;
      if ($3 != nullptr) {
        $$->orderbys = *$3;
        delete $3;
      }
    }
    ;
condition_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | condition {
      $$ = new std::vector<ConditionSqlNode>;
      $$->emplace_back(*$1);
      delete $1;
    }
    | condition AND condition_list {
      $$ = $3;
      $$->emplace_back(*$1);
      delete $1;
    }
    ;
condition:
    rel_attr NOT LIKE_CONDITION value
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 0;
      $$->right_value = *$4;
      $$->comp = NOT_LIKE;

      delete $1;
      delete $4;
    }
    | rel_attr comp_op value
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 0;
      $$->right_value = *$3;
      $$->comp = $2;

      delete $1;
      delete $3;
    }
    | value comp_op value 
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 0;
      $$->right_value = *$3;
      $$->comp = $2;

      delete $1;
      delete $3;
    }
    | rel_attr comp_op rel_attr
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 1;
      $$->right_attr = *$3;
      $$->comp = $2;

      delete $1;
      delete $3;
    }
    | value comp_op rel_attr
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 1;
      $$->right_attr = *$3;
      $$->comp = $2;

      delete $1;
      delete $3;
    }
    | value IS NULL_T
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 0;
      $$->right_value = Value(AttrType::NULLS);
      $$->comp = CompOp::IS_NULL;

      delete $1;
    }
    | value IS NOT NULL_T
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 0;
      $$->right_value = Value(AttrType::NULLS);
      $$->comp = CompOp::IS_NOT_NULL;

      delete $1;
    }
    | rel_attr IS NOT NULL_T
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 0;
      $$->right_value = Value(AttrType::NULLS);
      $$->comp = CompOp::IS_NOT_NULL;

      delete $1;
    }
    | rel_attr IS NULL_T
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 0;
      $$->right_value = Value(AttrType::NULLS);
      $$->comp = CompOp::IS_NULL;

      delete $1;
    }
    ;

comp_op:
      EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
    | LIKE_CONDITION { $$ = LIKE; }
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
