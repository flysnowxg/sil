/********************************************************************
说明:			sil解释器 . 函数扩展支持库
创建时间:	2013-8-29
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
#include "code_gen.h"

void inital_internal_fun();
bool find_internal_function(const string& fun_name);
bool internel_fun_call(const string&fun_name,int param_count,vector<inst_value>& stk,int first_param);

class func_extern;
//类型转换规则检查传入的实参是否能够被函数所接收
typedef bool (*param_invert_rule_t)(int,vector<inst_value>&,int,func_extern*);
//一下是内置的几条规则
//参数的类型转换规则，默认规则如下
bool default_param_invert_rule(int param_count,vector<inst_value>& stk,int first_param,func_extern* pfe);
//对于加减乘除等内置操作，应当进行类型提升 ，int可以提升为float，float可以提升到string
bool internal_param_invert_rule(int param_count,vector<inst_value>& stk,int first_param,func_extern* pfe);

template<typename T> T invert_param_to(vector<inst_value>& stk,int index=0);

//类型到整数索引的转换
template<typename T>
struct type_to_eiv
{
	static e_inst_value_t eiv;
};
template <typename T> e_inst_value_t type_to_eiv<T>::eiv=eiv_invalid;
template<> struct type_to_eiv<int>
{
	static e_inst_value_t eiv;
};
template<> struct type_to_eiv<double>
{
	static e_inst_value_t eiv;
};
template<> struct type_to_eiv<string>
{
	static e_inst_value_t eiv;
};

class func_extern
{
public:
	func_extern(const string& fun_name,param_invert_rule_t invert_f=default_param_invert_rule,e_inst_value_t return_t=eiv_invalid)
		:m_fun_name(fun_name),m_return_t(return_t),m_invert_f(invert_f){}
	bool can_invert_params(int param_count,vector<inst_value>& stk,int first_param)
	{
		return (*m_invert_f)(param_count,stk,first_param,this);
	}
	virtual void operator()(int param_count,vector<inst_value>& stk,int first_param)=0;
	const string m_fun_name;
	e_inst_value_t m_return_t;
	vector<e_inst_value_t> m_params_t;
	param_invert_rule_t m_invert_f;
};

class weak_param{};
//扩展函数的表示
template<typename ResultT,
	typename P1=weak_param,typename P2=weak_param,
	typename P3=weak_param,typename P4=weak_param,
	typename P5=weak_param,typename P6=weak_param>
class func_extern_impl :public func_extern
{
public:
	typedef ResultT (*fun0_t)() ;
	typedef ResultT (*fun1_t)(P1) ;
	typedef ResultT (*fun2_t)(P1,P2) ;
	typedef ResultT (*fun3_t)(P1,P2,P3) ;
	typedef ResultT (*fun4_t)(P1,P2,P3,P4) ;
	typedef ResultT (*fun5_t)(P1,P2,P3,P4,P5) ;
	typedef ResultT (*fun6_t)(P1,P2,P3,P4,P5,P6) ;
public:
#define FUNC_EXTERN_IMPL_CONSTRUCTOR(num) \
	func_extern_impl(const string& fun_name,fun##num##_t pf,param_invert_rule_t invert_f)\
	:func_extern(fun_name,invert_f)\
	{\
		m_fun = (void*)pf;\
		m_fun_index = num;\
		init_params();\
	}
	FUNC_EXTERN_IMPL_CONSTRUCTOR(0);
	FUNC_EXTERN_IMPL_CONSTRUCTOR(1);
	FUNC_EXTERN_IMPL_CONSTRUCTOR(2);
	FUNC_EXTERN_IMPL_CONSTRUCTOR(3);
	FUNC_EXTERN_IMPL_CONSTRUCTOR(4);
	FUNC_EXTERN_IMPL_CONSTRUCTOR(5);
	FUNC_EXTERN_IMPL_CONSTRUCTOR(6);

	void init_params()
	{
		m_return_t=type_to_eiv<ResultT>().eiv;
		if(type_to_eiv<P1>::eiv!=eiv_invalid) m_params_t.push_back(type_to_eiv<P1>().eiv);
		if(type_to_eiv<P2>::eiv!=eiv_invalid) m_params_t.push_back(type_to_eiv<P2>().eiv);
		if(type_to_eiv<P3>::eiv!=eiv_invalid) m_params_t.push_back(type_to_eiv<P3>().eiv);
		if(type_to_eiv<P4>::eiv!=eiv_invalid) m_params_t.push_back(type_to_eiv<P4>().eiv);
		if(type_to_eiv<P5>::eiv!=eiv_invalid) m_params_t.push_back(type_to_eiv<P5>().eiv);
		if(type_to_eiv<P6>::eiv!=eiv_invalid) m_params_t.push_back(type_to_eiv<P6>().eiv);
	}

	virtual void operator()(int param_count,vector<inst_value>& stk,int first_param)
	{
		stk[first_param] =call_wrap<ResultT>(
			invert_param_to<P1>(stk,first_param),
			invert_param_to<P2>(stk,first_param+1),
			invert_param_to<P3>(stk,first_param+2),
			invert_param_to<P4>(stk,first_param+3),
			invert_param_to<P5>(stk,first_param+4),
			invert_param_to<P6>(stk,first_param+5)
			);
	}

	template<typename XResultT,typename XP1,typename XP2,typename XP3,typename XP4,typename XP5,typename XP6>
	XResultT call_wrap(XP1 p1,XP2 p2,XP3 p3,XP4 p4,XP5 p5,XP6 p6)
	{
		if(m_fun_index==0) return ((fun0_t)m_fun)();
		if(m_fun_index==1) return ((fun1_t)m_fun)(p1);
		if(m_fun_index==2) return ((fun2_t)m_fun)(p1,p2);
		if(m_fun_index==3) return ((fun3_t)m_fun)(p1,p2,p3);
		if(m_fun_index==4) return ((fun4_t)m_fun)(p1,p2,p3,p4);
		if(m_fun_index==5) return ((fun5_t)m_fun)(p1,p2,p3,p4,p5);
		if(m_fun_index==6) return ((fun6_t)m_fun)(p1,p2,p3,p4,p5,p6);
		return XResultT();
	}
private:
	int m_fun_index;
	void* m_fun;
};

//扩展函数集合管理，和调用封装
class extern_fun_manager
{
public:
	bool register_fun(shared_ptr<func_extern> sp_fun);
	bool find(const string& fun_name);
	bool call(const string&fun_name,int param_count,vector<inst_value>& stk,int first_param);
	void list_function();
private:
	map<string,vector<shared_ptr<func_extern> > > m_funs;
};
extern_fun_manager* funs_manager();

//注册扩展函数到sil
template<typename ResultT>
void register_function(const string&fun_name,ResultT (*pf)(),param_invert_rule_t invert_f=default_param_invert_rule)
{
	funs_manager()->register_fun(shared_ptr<func_extern_impl<ResultT> >(new func_extern_impl<ResultT>(fun_name,pf,invert_f)));
}
template<typename ResultT,typename P1>
void register_function(const string&fun_name, ResultT (*pf)(P1),param_invert_rule_t invert_f=default_param_invert_rule)
{
	funs_manager()->register_fun(shared_ptr<func_extern_impl<ResultT,P1> >(new func_extern_impl<ResultT,P1>(fun_name,pf,invert_f)));
}
template<typename ResultT,typename P1,typename P2>
void register_function(const string&fun_name, ResultT (*pf)(P1,P2),param_invert_rule_t invert_f=default_param_invert_rule)
{
	funs_manager()->register_fun(shared_ptr<func_extern_impl<ResultT,P1,P2> >(new func_extern_impl<ResultT,P1,P2>(fun_name,pf,invert_f)));
}
template<typename ResultT,typename P1,typename P2,typename P3>
void register_function(const string&fun_name, ResultT (*pf)(P1,P2,P3),param_invert_rule_t invert_f=default_param_invert_rule)
{
	funs_manager()->register_fun(shared_ptr<func_extern_impl<ResultT,P1,P2,P3> >(new func_extern_impl<ResultT,P1,P2,P3>(fun_name,pf,invert_f)));
}
template<typename ResultT,typename P1,typename P2,typename P3,typename P4>
void register_function(const string&fun_name, ResultT (*pf)(P1,P2,P3,P4),param_invert_rule_t invert_f=default_param_invert_rule)
{
	funs_manager()->register_fun(shared_ptr<func_extern_impl<ResultT,P1,P2,P3,P4> >(new func_extern_impl<ResultT,P1,P2,P3,P4>(fun_name,pf,invert_f)));
}
template<typename ResultT,typename P1,typename P2,typename P3,typename P4,typename P5>
void register_function(const string&fun_name, ResultT (*pf)(P1,P2,P3,P4,P5),param_invert_rule_t invert_f=default_param_invert_rule)
{
	funs_manager()->register_fun(shared_ptr<func_extern_impl<ResultT,P1,P2,P3,P4,P5> >(new func_extern_impl<ResultT,P1,P2,P3,P4,P5>(fun_name,pf,invert_f)));
}
template<typename ResultT,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
void register_function(const string&fun_name, ResultT (*pf)(P1,P2,P3,P4,P5,P6),param_invert_rule_t invert_f=default_param_invert_rule)
{
	funs_manager()->register_fun(shared_ptr<func_extern_impl<ResultT,P1,P2,P3,P4,P5,P6> >(new func_extern_impl<ResultT,P1,P2,P3,P4,P5,P6>(fun_name,pf,invert_f)));
}
