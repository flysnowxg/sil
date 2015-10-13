/********************************************************************
说明:			sil解释器 . 符号管理
创建时间:	2013-8-22
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "symbol_manager.h"
#include "code_gen.h"
#include "normal_helper.h"

void symbol_table::list_function()
{
	vector<_symbol>::iterator it=m_symbols.begin();
	for(;it!=m_symbols.end();++it)
	{
		if(it->m_type==es_function) printf("user#  %s\n",it->m_name.c_str());
	}
}

static vector<shared_ptr<symbol_table> >* get_tables()
{
	static vector<shared_ptr<symbol_table> > s_tables;
	if(s_tables.size()==0)
	{
		s_tables.reserve(1024);
		s_tables.push_back(shared_ptr<symbol_table>(new symbol_table(1)));
		s_tables[0]->push_symbol(_symbol(es_value,"true",eab_global_data,alloc_global_data_space()));
		s_tables[0]->push_symbol(_symbol(es_value,"false",eab_global_data,alloc_global_data_space()));
	}
	return &s_tables;
}

symbol_table* get_global_table()
{
	vector<shared_ptr<symbol_table> >* p_tabels=get_tables();
	return (*p_tabels)[0].get();
}

symbol_table* get_top_table()
{
	vector<shared_ptr<symbol_table> >* p_tabels=get_tables();
	return (*p_tabels)[p_tabels->size()-1].get();
}

symbol_table* create_table()
{
	debug_print("create_table\n");
	vector<shared_ptr<symbol_table> >* p_tabels=get_tables();
	p_tabels->push_back(shared_ptr<symbol_table>(new symbol_table(get_top_gener_id())));
	return get_top_table();
}

void pop_table()
{
	vector<shared_ptr<symbol_table> >* p_tabels=get_tables();
	p_tabels->pop_back();
	debug_print("pop_table\n");
}

_symbol* find_symbol(const string& name,e_symbol sym_type)
{
	vector<shared_ptr<symbol_table> >* p_tabels=get_tables();
	if(sym_type==es_function)
	{
		for(unsigned int count=p_tabels->size();count>0;count--)
		{
			_symbol* p_sym=(*p_tabels)[count-1]->find_symbol(name);
			if(p_sym!=NULL &&p_sym->m_type==es_function) return p_sym;
		}
	}
	else if(sym_type==es_value)
	{
		symbol_table* p_top=get_top_table();
		for(unsigned int count=p_tabels->size();count>0;count--)
		{
			shared_ptr<symbol_table> sp_table=(*p_tabels)[count-1];
			if(p_top->get_fun_scope()!=sp_table->get_fun_scope()) continue;
			_symbol* p_sym=sp_table->find_symbol(name);
			if(p_sym!=NULL &&p_sym->m_type==es_value) return p_sym;
		}
		symbol_table* p_global=get_global_table();
		_symbol* p_sym=p_global->find_symbol(name);
		if(p_sym!=NULL &&p_sym->m_type==es_value) return p_sym;
	}
	return NULL;
}

_symbol* push_symbol(const _symbol& sym,bool b_replace)
{
	symbol_table* p_tabel=get_top_table();
	_symbol* p_sym=p_tabel->find_symbol(sym.m_name);
	if(p_sym)
	{
		if(b_replace) 
		{
			*p_sym=sym;
			return p_sym;
		}
		else 
		{
			report_error("重复符号定义:%s\n",p_sym->m_name.c_str());
			return NULL;
		}
	}
	return p_tabel->push_symbol(sym);
}
