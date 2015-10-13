/********************************************************************
说明:			sil解释器 . 符号管理
创建时间:	2013-8-22
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
#include "normal_helper.h"
#include "code_gen.h"
enum e_symbol
{
	es_invalid,
	es_value,
	es_function,
};

class _symbol
{
public:
	_symbol(e_symbol subclass,const string& name="",e_addr_type addr_base=eab_invalid,int addr=0)
		:m_type(subclass),m_name(name),m_addr_t(addr_base),m_addr(addr),m_fun_params_count(0){}
	e_symbol m_type;
	string m_name;
	e_addr_type m_addr_t;
	int m_addr;
	int m_fun_params_count;
};

class symbol_table
{
public:
	symbol_table(int fun_scope):m_function_scope(fun_scope)
	{
		m_symbols.reserve(10240);
	}
	_symbol* find_symbol(const string& name)
	{
		for(unsigned int count=m_symbols.size();count>0;count--)
		{
			if(m_symbols[count-1].m_name==name) return &m_symbols[count-1];
		}
		return NULL;
	}
	_symbol* push_symbol(const _symbol& sym)
	{
		if(m_symbols.size()>=10240)
		{
			report_error("符号太多，超出管理范围\n");
			return NULL;
		}
		m_symbols.push_back(sym);
		return &m_symbols[m_symbols.size()-1];
	}
	int get_fun_scope(){return m_function_scope;}
	void list_function();
private:
	vector<_symbol> m_symbols;
	int m_function_scope;
};

symbol_table* get_global_table();
symbol_table* get_top_table();
symbol_table* create_table();
void pop_table();
_symbol* find_symbol(const string& name,e_symbol sym_type=es_value);
_symbol* push_symbol(const _symbol& sym,bool b_replace=false);

