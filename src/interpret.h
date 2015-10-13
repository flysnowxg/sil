/********************************************************************
说明:			sil解释器 . 解释器
创建时间:	2013-8-25
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
#include "code_gen.h"

class interpret
{
public:
	interpret(code_gen* p_gen);
	int cacul_addr(_instruction);
	bool run(int ip=0);
	static void is_output_last_value(bool is_output){sm_is_output=is_output;}
private:
	vector<_instruction>& m_codes;
	int m_ip;
	int m_reg_base;
	int m_reg_top;
	_instruction m_cur_instr;
	static vector<inst_value> sm_global_data;
	static bool sm_is_output;
};
