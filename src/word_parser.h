/********************************************************************
说明:			sil解释器 . 词法分析
创建时间:	2013-8-20
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"

//词法分析
//ident number float_number
//var function if else while 
//, ;  () {} =  
//!
// * / %  
//+ -
//==  !=  >  <

enum e_word_t
{
	ewt_invalid,
	ewt_ident,
	ewt_int,
	ewt_float,
	ewt_string,

	ewt_key_var,
	ewt_key_function,
	ewt_key_if,
	ewt_key_else,
	ewt_key_while,
	ewt_key_for,
	ewt_key_continue,
	ewt_key_break,
	ewt_key_return,

	ewt_key_comma,//   ,
	ewt_key_semicolon,//   ;
	ewt_key_lsbranch,//   (
	ewt_key_rsbranch,//   )
	ewt_key_lbbranch,//   {
	ewt_key_rbbranch,//   }

	ewt_key_assign,//   =
	ewt_key_not, //    !
	ewt_key_mul,//   *
	ewt_key_div,//   /
	ewt_key_mod,//   %
	ewt_key_plus,//   +
	ewt_key_munis,//   -
	ewt_key_eql,//   ==
	ewt_key_neql,//   !=
	ewt_key_less,//   <
	ewt_key_greate,//   >
	ewt_key_logic_or,// ||
	ewt_key_logic_and,// &&
	ewt_end_file,
};
class word
{
public:
	word(e_word_t subclass=ewt_invalid,const string& value=""):m_type(subclass),m_str_value(value),m_int_value(0),m_float_value(0){}
	word(int value):m_type(ewt_int),m_int_value(value),m_float_value(0){}
	word(double value):m_type(ewt_float),m_int_value(0),m_float_value(value){}
	e_word_t m_type;
	string m_str_value;
	int m_int_value;
	double m_float_value;
};
class word_parser
{
public:
	word_parser(const string& code="");
	virtual ~word_parser(void){}
	bool load(const string& file_path);
	word get();
	bool check(e_word_t word1_t,e_word_t word2_t=ewt_invalid);
	void push(const word& item);
private:
	void escape();
	void escape_space();
	void escape_note();
	word get_number();
	word get_float();
	word get_string();
	word get_ident_or_key();
	word get_other();
private:
	string m_code;
	unsigned int m_current_index;
	int m_current_line;
	deque<word> m_cache;
};

void parse_file_word(const string& file_path);