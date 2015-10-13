/********************************************************************
说明:			sil解释器 . 解释器
创建时间:	2013-8-25
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "interpret.h"
#include "func_extern_lib.h"

vector<inst_value> interpret::sm_global_data(10240);
bool  interpret::sm_is_output=false;

interpret::interpret(code_gen* p_gen)
:m_codes(p_gen->m_codes)
{
	m_ip=0;
	m_reg_base=0;
	m_reg_top=0;
}

int interpret::cacul_addr(_instruction inst)
{
	if(inst.m_type==e_load || inst.m_type==e_save)
	{
		if(inst.m_addr_t==eab_reg_sb) 
		{
			return m_reg_base+inst.m_addr;
		}
		else if(inst.m_addr_t==eab_global_data) return inst.m_addr;
	}
	else if(inst.m_type==e_call || inst.m_type==e_jmp || inst.m_type==e_jpc)
	{
		if(inst.m_addr_t==eab_absolute_ip) 
		{
			return inst.m_addr;
		}
		else if(inst.m_addr_t==eab_relat_ip) 
		{
			if(m_ip+inst.m_addr <= (int)m_codes.size()) return m_ip+inst.m_addr;
		}
		else if(inst.m_addr_t==eab_invalid) return 0;
	}
	else if(inst.m_type==e_load_const || inst.m_type==e_pop || inst.m_type==e_ret || inst.m_type==e_exit)
	{
		if(inst.m_addr_t==eab_invalid) return 0;
	}
	printf("运行时错误：寻址错误 ip=%d reg_sb=%d reg_top=%d\n",m_ip,m_reg_base,m_reg_top);
	printf("当前指令信息[%d]:%s\n",m_cur_instr.m_index,m_cur_instr.to_string().c_str());
	return -1;
}

bool interpret::run(int ip)
{
	m_ip=ip;
	bool b_exception=false;
	vector<inst_value> stk(10240);
	while((int)m_codes.size()>m_ip && !b_exception)
	{
		m_cur_instr=m_codes[m_ip++];
		int addr=cacul_addr(m_cur_instr);
		if(addr==-1) b_exception=true;
		if(addr==-1) break;
		switch(m_cur_instr.m_type)
		{
		case e_load_const:
			stk[m_reg_top++]=m_cur_instr.m_value;
			break;
		case e_load:
			if(m_cur_instr.m_addr_t==eab_global_data) stk[m_reg_top++]=sm_global_data[addr];
			else stk[m_reg_top++]=stk[addr];
			break;
		case e_save:
			if(m_cur_instr.m_addr_t==eab_global_data) sm_global_data[addr]=stk[m_reg_top-1];
			else stk[addr]=stk[m_reg_top-1];
			break;
		case e_pop:
			m_reg_top-=m_cur_instr.m_value.to_int();
			break;
		case e_call:
			{
				string fun_name=m_cur_instr.m_value.to_string();
				int param_count=stk[m_reg_top-2].to_int();
				int first_param=m_reg_top-2-param_count;
				bool b_find_in_kernel=find_internal_function(fun_name);
				bool b_find_in_user=false;
				if(b_find_in_kernel)
				{
					b_exception=!internel_fun_call(fun_name,param_count,stk,first_param);
					if(!b_exception) m_reg_top=first_param+1;
				}
				else 
				{
					b_exception=true;
				}
				if(b_exception)
				{
					 if(fun_name=="exit") 
					{
						b_exception=false;
						m_ip=m_codes.size();
					}
					else
					{
						_symbol* p_sym=find_symbol(fun_name,es_function);
						if(p_sym)
						{
							b_find_in_user=true;
							if(param_count==p_sym->m_fun_params_count)
							{
								m_cur_instr.m_addr_t=p_sym->m_addr_t;
								m_cur_instr.m_addr=p_sym->m_addr;
								addr=cacul_addr(m_cur_instr);
								if((m_cur_instr.m_addr_t!=eab_invalid) && (addr!=-1))
								{
									b_exception=false;
									stk[m_reg_top++]=m_reg_base;
									stk[m_reg_top++]=m_ip;
									m_reg_base=m_reg_top-2;
									m_ip=addr;
								}
							}
						}
					}
				}
				if(b_exception) 
				{
					if(b_find_in_kernel || b_find_in_user)
						printf("运行时错误：找到%s函数的定义，但是参数无法转换\n",m_cur_instr.m_value.to_string().c_str());
					else printf("运行时错误：未找到%s函数的定义\n",m_cur_instr.m_value.to_string().c_str());
				}
			}
			break;
		case e_ret:
			{
				int first_param=m_reg_base-2-stk[m_reg_base-2].to_int();
				stk[first_param]=stk[m_reg_base-1];
				m_ip=stk[m_reg_base+1].to_int();
				m_reg_top=first_param+1;
				m_reg_base=stk[m_reg_base].to_int();
			}
			break;
		case e_jmp:
			m_ip=addr;
			break;
		case e_jpc:
			m_reg_top--;
			if(stk[m_reg_top].to_int()==0) m_ip=addr;
			break;
		case e_exit:
			m_ip=m_codes.size();
			break;
		default:
			printf("运行时错误：未识别指令[%d]:%s\n",m_cur_instr.m_index,m_cur_instr.to_string().c_str());
			b_exception=true;
		}
	}
	if(!b_exception)
	{
		if(sm_is_output) printf("#%s",stk[m_reg_top].to_string().c_str());
	}
	return !b_exception;
}