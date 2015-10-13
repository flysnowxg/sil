/********************************************************************
说明:			sil解释器 . 函数扩展支持库
创建时间:	2013-8-29
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "func_extern_lib.h"
e_inst_value_t type_to_eiv<int>::eiv=eiv_int;
e_inst_value_t type_to_eiv<double>::eiv=eiv_float;
e_inst_value_t type_to_eiv<string>::eiv=eiv_string;

//参数的类型转换规则，默认规则如下
bool default_param_invert_rule(int param_count,vector<inst_value>& stk,int first_param,func_extern* pfe)
{
	for(int count_i=0;count_i<param_count;count_i++)
	{
		if(!stk[first_param+count_i].can_invert_to(pfe->m_params_t[count_i])) return false;
	}
	return true;
}
//对于加减乘除等内置操作，应当进行类型提升 ，int可以提升为float，float可以提升到string
bool internal_param_invert_rule(int param_count,vector<inst_value>& stk,int first_param,func_extern* pfe)
{
	for(int count_i=0;count_i<param_count;count_i++)
	{
		e_inst_value_t real_param_t=stk[first_param+count_i].m_type;
		e_inst_value_t form_param_t=pfe->m_params_t[count_i];
		if(real_param_t>form_param_t) return false;
	}
	return true;
}

template<> weak_param invert_param_to<weak_param>(vector<inst_value>& stk,int index)
{
	return weak_param();
}
template<> int invert_param_to<int>(vector<inst_value>& stk,int index)
{
	return stk[index].to_int();
}
template<> double  invert_param_to<double>(vector<inst_value>& stk,int index)
{
	return stk[index].to_float();
}
template<> string invert_param_to<string>(vector<inst_value>& stk,int index)
{
	return stk[index].to_string();
}

bool extern_fun_manager::register_fun(shared_ptr<func_extern> sp_fun)
{
	m_funs[sp_fun->m_fun_name].push_back(sp_fun);
	return true;
}

bool extern_fun_manager::find(const string& fun_name)
{
	map<string,vector<shared_ptr<func_extern> > >::iterator it=m_funs.find(fun_name);
	if(it!=m_funs.end()) return true;
	else return false;
}

bool extern_fun_manager::call(const string&fun_name,int param_count,vector<inst_value>& stk,int first_param)
{
	//查找算法:
	//	比较函数名得到候选函数集合
	//	比较形参个数，去掉参数个数不匹配的
	//	比较实参和形参，检查是否有完全符合的,如果找到结束查找并调用
	//	对实参进行类型转换，查找第一个能匹配的函数,如果找到结束查找并调用
	//	否则出错
	map<string,vector<shared_ptr<func_extern> > >::iterator it=m_funs.find(fun_name);
	if(it==m_funs.end()) return false;
	vector<shared_ptr<func_extern> > funs=it->second;
	for(vector<shared_ptr<func_extern> >::iterator it=funs.begin();it!=funs.end();)
	{
		if((*it)->m_params_t.size()!=param_count) it=funs.erase(it);
		else it++;
	}
	for(vector<shared_ptr<func_extern> >::iterator it=funs.begin();it!=funs.end();it++)
	{
		int count_i=0;
		for(;count_i<param_count;count_i++)
		{
			if(stk[first_param+count_i].m_type!=(*it)->m_params_t[count_i]) break;
		}
		if(count_i==param_count) 
		{
			(**it)(param_count,stk,first_param);
			return true;
		}
	}
	for(vector<shared_ptr<func_extern> >::iterator it=funs.begin();it!=funs.end();it++)
	{
		if((*it)->can_invert_params(param_count,stk,first_param))
		{
			(**it)(param_count,stk,first_param);
			return true;
		}
	}
	return false;
}

void extern_fun_manager::list_function()
{
	for(map<string,vector<shared_ptr<func_extern> > >::iterator it=m_funs.begin();it!=m_funs.end();it++)
	{
		printf("kernel#  %s\n",it->first.c_str());
	}
}

extern_fun_manager* funs_manager()
{
	static extern_fun_manager s_manager;
	return &s_manager;
}

bool find_internal_function(const string& fun_name)
{
	if(funs_manager()->find(fun_name)) return true;
	return false;
}

bool internel_fun_call(const string&fun_name,int param_count,vector<inst_value>& stk,int first_param)
{
	return funs_manager()->call(fun_name,param_count,stk,first_param);
}
