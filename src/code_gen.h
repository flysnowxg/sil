/********************************************************************
说明:			sil解释器 . 代码生成器
创建时间:	2013-8-23
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
//三个空间   代码空间 全局数据空间 堆栈空间
enum e_instruction
{
	e_load_const,//(操纵堆栈空间) 加载常量到栈顶
	e_load,//(操纵堆栈空间 全局数据空间) 将变量的加载到栈顶
	e_save,//(操纵堆栈空间 全局数据空间) 将栈顶的值写入变量
	e_pop,//(操纵堆栈空间) 将reg_sb减小
	e_call,//(操纵代码空间 堆栈空间) 调用函数
	e_ret,//(操纵代码空间 堆栈空间) 从函数调用返回
	e_jmp,//(操纵代码空间) 无条件跳转
	e_jpc,//(操纵代码空间) 栈顶元素为0时跳转
	e_exit,//退出解释器
};
enum e_addr_type
{
	eab_invalid,
	eab_absolute_ip,//绝对寻址(代码空间)
	eab_relat_ip,//相对于ip寻址(代码空间)
	eab_reg_sb,//相对于sb寄存器寻址
	eab_global_data,
};
enum e_inst_value_t
{
	eiv_invalid,
	eiv_int,
	eiv_float,
	eiv_string
};
class inst_value
{
public:
	inst_value(int value=0):m_type(eiv_int),m_int_value(value),m_float_value(0){}
	inst_value(double value):m_type(eiv_float),m_int_value(0),m_float_value(value){}
	inst_value(const string& value):m_type(eiv_string),m_int_value(0),m_float_value(0),m_string_value(value){}
	bool can_invert_to(e_inst_value_t);
	int to_int();
	double to_float();
	string to_string();
	e_inst_value_t m_type;
	int m_int_value;
	double m_float_value;
	string m_string_value;
};

class _instruction
{
public:
	string to_string();
	int m_index;//
	e_instruction m_type;
	e_addr_type m_addr_t;//
	int m_addr;
	inst_value m_value;
};

class code_gen
{
public:
	code_gen(void);
	~code_gen(void){}
	_instruction* gen(e_instruction inst_type,e_addr_type base,int addr,inst_value value);
	int get_new_code_addr();
	vector<_instruction> m_codes;
};

code_gen* get_global_gener();
code_gen* get_top_gener();
int get_top_gener_id();//0--为最终合并代码的空间，1--为全局变量函数定义的空间
code_gen* create_gener();
void pop_gener();
int merge_code();
void print_code();

int alloc_global_data_space();
int get_new_code_addr(bool b_global=false);
_instruction*  gen_code(e_instruction inst_type,e_addr_type base=eab_invalid,int addr=0,inst_value value=0,bool b_global=false);
bool gen_fun_call(const string& fun_name,int param_count,int& stk_index);
_instruction*  gen_pop_instruction(int& stk_index,int pop_count=1);
void gen_load_const_instruction(int& stk_index,inst_value value);
