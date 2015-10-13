/********************************************************************
说明:			sil解释器 . 语法解析
创建时间:	2013-8-21
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
#include "word_parser.h"

class while_context;
class grammar_parser
{
public:
	grammar_parser(void);
	~grammar_parser(void);
	bool load_file(const string& file_path);
	void load_string(const string& str);

	void test_and_skip(set<e_word_t> begin,const set<e_word_t>& follow);
	bool complie();
	void block(set<e_word_t> follows,int& stk_index,while_context* p_while_conext);//stk_index指向栈顶上面第一个未用的位置
	void body(set<e_word_t> follows,int& stk_index,while_context* p_while_conext);//略过第一个符号 '{'
	void declare(set<e_word_t> follows,int& stk_index);//略过第一个符号 'var'

	void function(set<e_word_t> follows);//略过第一个符号 'function'
	int fun_params(set<e_word_t> follows);//略过第一个符号 '('

	void statement(set<e_word_t> follows,int& stk_index,while_context* p_while_conext);
	void if_statement(set<e_word_t> follows,int& stk_index,while_context* p_while_conext);//略过第一个符号 'if'
	void while_statement(set<e_word_t> follows,int& stk_index);//略过第一个符号 'while'
	void for_statement(set<e_word_t> follows,int& stk_index);//略过第一个符号 'for'
	void return_statement(set<e_word_t> follows,int& stk_index);//略过第一个符号 'return'
	void expression(set<e_word_t> follows,int& stk_index);
	void assign_expression(set<e_word_t> follows,int& stk_index);
	void factor_logic(set<e_word_t> follows,int& stk_index);
	void factor_compare(set<e_word_t> follows,int& stk_index);
	void factor_plus_minus(set<e_word_t> follows,int& stk_index);
	void factor_mul_div(set<e_word_t> follows,int& stk_index);
	void factor_single(set<e_word_t> follows,int& stk_index);
	void term(set<e_word_t> follows,int& stk_index);
	void funcall_expression(set<e_word_t> follows,int& stk_index);
private:
	set<e_word_t> m_block_begs;
	set<e_word_t> m_stats_begs;
	set<e_word_t> m_expression_begs;
	set<e_word_t> m_const_values;
	word_parser m_words;
};
bool sil_eval(string code);
bool sil_load_file(string file_path);
/*
//author : xuegang qq:308821698
//sil语言单词的词法的正则表达式定义如下：
ident : [a-zA-Z]([a-zA-Z0-9])*
int : [0-9]+
float : [0-9](\.[0-9]+)?(e-?[0-9]+)?
string : "[^"]*"		//另外对于字符串中有\"的情形，应当作为字符串的一部分
//对于操作符和关键字不详述

//sil语言语法的BNF定义
declaration :
	var ident '=' expression ';'
	| var ident  ';'

function :
	'function' ident '(' params ')' body
	'function' ident '('  ')' body
params :
	ident
	| params ',' ident

body : 
	'{' blocks '}'  
	'{'  '}'  
blocks :
	blocks block
	| block
block : 
	declaration
	| function
	| body
	| statement

statement :
	expression 
	| if_statement
	| while_statement
	| for_statment
	| 'return' ';'
	| 'break' ';'
	| 'continue' ';'
	| ';'

if_statement :
	if_part 
	| if_part else_part
if_part :
	'if' '(' expresion ')' body
	| if' '(' expresion ')' statement
else_part :
	'else'  '('  expresion ')' body
	| 'else'  '('  expresion ')' statement

while_statement :
	'while' '(' expression ')' body
	| 'while' '(' expression ')' statement

for_statment :
	'for' '(' for_part ';' for_part ';' for_part ')' body
	| 'for' '(' for_part ';' for_part ';' for_part ')' statement
for_part :
	expression
	| 

expression
	assign_expression
	| factor_logic

assign_expression
	ident '=' expression

factor_logic :
	factor_logic '&&' factor_compare
	| factor_logic '||' factor_compare

factor_compare : 
	factor_compare '==' factor_plus_minus
	| factor_compare '!=' factor_plus_minus
	| factor_compare '>' factor_plus_minus
	| factor_compare '<' factor_plus_minus

factor_plus_minus :
	factor_plus_minus '+' factor_mul_div
	| factor_plus_minus '-' factor_mul_div

factor_mul_div :
	factor_mul_div '+' fatcor_sigle
	| factor_mul_div '-' fatcor_sigle

fatcor_sigle  :
	'!' term
	| '-' term

term:
	int
	| float
	| string
	| ident
	| '(' factor_logic ')'
	| ident '(' real_params ')'

real_params :
	| factor_logic
	| real_params ',' factor_logic

*/
