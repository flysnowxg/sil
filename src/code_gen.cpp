/********************************************************************
说明:			sil解释器 . 代码生成器
创建时间:	2013-8-23
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "normal_helper.h"
#include "code_gen.h"
#include "symbol_manager.h"
#include "func_extern_lib.h"

bool inst_value::can_invert_to(e_inst_value_t to_type)
{
	if(to_type==eiv_string) return true;
	else if((to_type==eiv_float) || (to_type==eiv_int) )
	{
		if(m_type!=eiv_string) return true;
		word_parser parser(m_string_value);
		word tmp=parser.get();
		if(!parser.check(ewt_end_file)) return false;
		else if(tmp.m_type==ewt_int) return true;
		else if(tmp.m_type==ewt_float) return true;
	}
	return false;
}

int inst_value::to_int()
{
	if(m_type==eiv_int) return m_int_value;
	else if(m_type==eiv_float) return (int)m_float_value;
	else if(m_type==eiv_string)
	{
		return atoi(m_string_value.c_str());
	}
	else return 0;
}

double inst_value::to_float()
{
	if(m_type==eiv_int) return m_int_value;
	else if(m_type==eiv_float) return m_float_value;
	else if(m_type==eiv_string)
	{
		return atof(m_string_value.c_str());
	}
	else return 0;
}

string inst_value::to_string()
{
	if(m_type==eiv_string) return m_string_value;
	else if(m_type==eiv_int)
	{
		char buf[32];
		sprintf(buf,"%d",m_int_value);
		return buf;
	}
	else if(m_type==eiv_float)
	{
		char buf[32];
		sprintf(buf,"%10.14f",m_float_value);
		return buf;
	}
	return "????";
}

string _instruction::to_string()
{
	static map<e_instruction,string> s_inst;
	if(s_inst.size()==0)
	{
		s_inst[e_load_const]="e_load_const";
		s_inst[e_load]="e_load";
		s_inst[e_save]="e_save";
		s_inst[e_pop]="e_pop";
		s_inst[e_call]="e_call";
		s_inst[e_ret]="e_ret";
		s_inst[e_jmp]="e_jmp";
		s_inst[e_jpc]="e_jpc";
		s_inst[e_exit]="e_exit";
	}
	static map<e_addr_type,string> s_add_t;
	if(s_add_t.size()==0)
	{
		s_add_t[eab_invalid]="-";
		s_add_t[eab_absolute_ip]="ai";
		s_add_t[eab_relat_ip]="ri";
		s_add_t[eab_reg_sb]="rs";
		s_add_t[eab_global_data]="gd";
	}
	char buf[128];
	sprintf(buf,"%s   %s:%d   %s",s_inst[m_type].c_str(),s_add_t[m_addr_t].c_str(),m_addr,m_value.to_string().c_str());
	return buf;
}

code_gen::code_gen(void)
{
	m_codes.reserve(10240);
}

_instruction* code_gen::gen(e_instruction inst_type,e_addr_type base,int addr,inst_value value)
{
	_instruction inst;
	inst.m_index=m_codes.size();
	inst.m_type=inst_type;
	inst.m_addr_t=base;
	inst.m_addr=addr;
	inst.m_value=value;
	m_codes.push_back(inst);
	return &m_codes[m_codes.size()-1];
}

int code_gen::get_new_code_addr()
{
	return m_codes.size();
}

static vector<shared_ptr<code_gen> >* get_geners()
{
	static vector<shared_ptr<code_gen> > s_tables;
	if(s_tables.size()==0)
	{
		s_tables.reserve(1024);
		s_tables.push_back(shared_ptr<code_gen>(new code_gen()));
	}
	return &s_tables;
}

code_gen* get_global_gener()
{
	vector<shared_ptr<code_gen> >* p_geners=get_geners();
	return (*p_geners)[0].get();
}

code_gen* get_top_gener()
{
	vector<shared_ptr<code_gen> >* p_geners=get_geners();
	return (*p_geners)[p_geners->size()-1].get();
}

code_gen* create_gener()
{
	vector<shared_ptr<code_gen> >* p_geners=get_geners();
	p_geners->push_back(shared_ptr<code_gen>(new code_gen));
	return get_top_gener();
}

void pop_gener()
{
	vector<shared_ptr<code_gen> >* p_geners=get_geners();
	p_geners->pop_back();
}

int get_top_gener_id()
{
	return get_geners()->size()-1;
}

int merge_code()
{
	code_gen* p_global=get_global_gener();
	code_gen* p_top=get_top_gener();
	int result=p_global->m_codes.size();
	if(p_global!=p_top)
	{
		copy(p_top->m_codes.begin(),p_top->m_codes.end(),inserter(p_global->m_codes,p_global->m_codes.end()));
	}
	else
	{
		report_error("合并代码失败'\n");
	}
	return result;
}

_instruction*  gen_code(e_instruction inst_type,e_addr_type base,int addr,inst_value value,bool b_global)
{
	if(b_global) return get_global_gener()->gen(inst_type,base,addr,value);
	else return get_top_gener()->gen(inst_type,base,addr,value);
}

bool gen_fun_call(const string& fun_name,int param_count,int& stk_index)
{
	//param1,param2,param3,param_count,ret_vaule,prev_reg_sb,prev_reg_ip,其中prev_reg_sb对应堆栈起始位置
	gen_load_const_instruction(stk_index,0);//返回值
	stk_index-=(param_count+1);
	gen_code(e_call,eab_invalid,0,fun_name);
	return true;
}

_instruction* gen_pop_instruction(int& stk_index,int pop_count)
{
	stk_index-=pop_count;
	return gen_code(e_pop,eab_invalid,0,pop_count);
}

void gen_load_const_instruction(int& stk_index,inst_value value)
{
	gen_code(e_load_const,eab_invalid,0,value);
	stk_index++;
}

int get_new_code_addr(bool b_global)
{
	if(b_global) return get_global_gener()->get_new_code_addr();
	else return get_top_gener()->get_new_code_addr();
}

int alloc_global_data_space()
{
	static int s_index=0;
	return s_index++;
}

void print_code()
{
	int count=0;
	code_gen* p_global=get_global_gener();
	for(vector<_instruction>::iterator it=p_global->m_codes.begin();it!=p_global->m_codes.end();it++)
	{
		printf("%d:  %s\n",count++,it->to_string().c_str());
	}
}
