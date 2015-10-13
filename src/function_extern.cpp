/********************************************************************
说明:			sil解释器 . 扩展函数
创建时间:	2013-8-32
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "function_extern.h"
#include "func_extern_lib.h"
#include "symbol_manager.h"
#ifndef GCC
#include <windows.h>
#endif

template<template <typename T> class Op ,typename T>
T base_op_ot_tt(T first)
{
	return  Op<T>()(first);
}
template<template <typename T> class Op ,typename T>
T base_op_ot_ttt(T first,T second)
{
	return  Op<T>()(first,second);
}
template<template <typename T> class Op ,typename ResultT,typename T>
ResultT base_op_ort_rtt(T first,T second)
{
	return  Op<T>()(first,second);
}

template<typename T>
int sil_to_int(T value)
{
	return inst_value(value).to_int();
}

template<typename T>
double sil_to_float(T value)
{
	return inst_value(value).to_float();
}

template<typename T>
string sil_to_string(T value)
{
	return inst_value(value).to_string();
}

bool sil_eval(string code);
bool sil_load_file(string file_path);

int sil_print(string str)
{
	printf("%s",str.c_str());
	return 0;
}

string sil_read()
{
	string result;
	char input=0;
	while(input!='\n')
	{
		input=getchar();
		if(input=='\n') break;
		result+=input;
	}while(input!='\n');
	return result;
}

int sil_bat(string win_script)
{
#ifndef GCC
	char bat_file_name[64];
	sprintf(bat_file_name,"temp%d%d.bat",rand(),rand());

	ofstream o_file(bat_file_name);
	o_file<<"echo off\r\n";
	o_file<<win_script<<" \r\n";
	o_file.close();

	char cmd_line[4096];
	sprintf(cmd_line,"cmd /q /c \"call %s\"",bat_file_name);
	STARTUPINFOA si = { sizeof(si) }; 
	PROCESS_INFORMATION pi; 
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = TRUE;
	BOOL bRet = ::CreateProcessA ( NULL,cmd_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); 
	if(bRet) 
	{ 
		WaitForSingleObject(pi.hThread,INFINITE);
		::CloseHandle (pi.hThread); 
		::CloseHandle (pi.hProcess); 
	} 
	remove(bat_file_name);
#endif
	return 0;
}

int sil_list_asm_code()
{
	print_code();
	return 0;
}

int sil_list_function()
{
	 funs_manager()->list_function();
	 get_global_table()->list_function();
	 return 0;
}

int sil_strlen(string str)
{
	return str.size();
}
string sil_substr(string str,int start,int size)
{
	return str.substr(start,size);
}

int sil_help()
{
	char description[]=
		"sil是一个简单的解释性语言，可以非常容易的嵌入到c++代码中，也能非常容易的用c++进行扩展\n"
		"界面显示说明：\n"
		" 输入1+2*3; 会回显 #7 ，输入print(\"hello\");会回显 hello#0\n"
		" #号之前的是调用print之类函数的输出，#之后的是最后执行的语句的返回值\n"
		"函数帮助：\n"
		" to_int 将一个值转换为int类型，例如：to_int(\"123\");\n"
		" to_float 将一个值转换为float类型，例如：to_float(2);\n"
		" to_string 将一个值转换为string类型，例如：to_string(254);\n"
		" strlen 求字符串的长度，例如：strlen(\"hello\");\n"
		" substr 截取字符串的的一部分，例如：substr(\"hello\",1,3);\n"
		" eval 可以求值一个字符串形式的表达式，例如:  var code=\"1+2*3;\";eval(code);\n"
		" load 可以加载并执行一个sil代码的文件，例如： load(\"../lib/math_test.sil\");\n"
		" exit 终止脚本的执行，例如：print(\"hello\");exit();print(\"world\");\n"
		" print 打印一个值，例如：print(\"hello world\"+3);\n"
		" read 可以从控制台读取一个字符串，例如：var tmp=read();print(tmp);\n"
		" bat 可以执行一个windows命令，例如：bat(\"dir\");\n"
		" list_function 打印已经定义的内置函数和脚本函数，例如：list_funciton();\n"
		" list_asm_code 打印编译出的代码，例如：list_asm_code();\n"
		" set_sil 可以设置解释器的一些开关，例如：set_sil(\"\",0);\n"
		" help 显示帮助信息，例如：help();\n"
		;
	printf("%s",description);
	return 0;
}

void inital_internal_fun()
{

	register_function("+",base_op_ot_ttt<plus,int >,internal_param_invert_rule);
	register_function("+",base_op_ot_ttt<plus,double>,internal_param_invert_rule);
	register_function("+",base_op_ot_ttt<plus,string>,internal_param_invert_rule);
	register_function("-",base_op_ot_ttt<minus,int>,internal_param_invert_rule);
	register_function("-",base_op_ot_ttt<minus,double>,internal_param_invert_rule);
	register_function("*",base_op_ot_ttt<multiplies,int>,internal_param_invert_rule);
	register_function("*",base_op_ot_ttt<multiplies,double>,internal_param_invert_rule);
	register_function("/",base_op_ot_ttt<divides,int>,internal_param_invert_rule);
	register_function("/",base_op_ot_ttt<divides,double>,internal_param_invert_rule);
	register_function("%",base_op_ot_ttt<modulus,int>,internal_param_invert_rule);

	register_function("<",base_op_ort_rtt<less,int,int >,internal_param_invert_rule);
	register_function("<",base_op_ort_rtt<less,int,double>,internal_param_invert_rule);
	register_function("<",base_op_ort_rtt<less,int ,string>,internal_param_invert_rule);
	register_function(">",base_op_ort_rtt<greater,int,int>,internal_param_invert_rule);
	register_function(">",base_op_ort_rtt<greater,int,double>,internal_param_invert_rule);
	register_function(">",base_op_ort_rtt<greater,int,string>,internal_param_invert_rule);
	register_function("==",base_op_ort_rtt<equal_to,int,int>,internal_param_invert_rule);
	register_function("==",base_op_ort_rtt<equal_to,int,double>,internal_param_invert_rule);
	register_function("==",base_op_ort_rtt<equal_to,int,string>,internal_param_invert_rule);
	register_function("!=",base_op_ort_rtt<not_equal_to,int,int>,internal_param_invert_rule);
	register_function("!=",base_op_ort_rtt<not_equal_to,int,double>,internal_param_invert_rule);
	register_function("!=",base_op_ort_rtt<not_equal_to,int ,string>,internal_param_invert_rule);

	register_function("-",base_op_ot_tt<negate,int >,internal_param_invert_rule);
	register_function("-",base_op_ot_tt<negate,double >,internal_param_invert_rule);
	register_function("!",base_op_ot_tt<logical_not,int >,internal_param_invert_rule);
	register_function("!",base_op_ot_tt<logical_not,double >,internal_param_invert_rule);
	register_function("&&",base_op_ot_ttt<logical_and,int >,internal_param_invert_rule);
	register_function("&&",base_op_ot_ttt<logical_and,double >,internal_param_invert_rule);
	register_function("||",base_op_ot_ttt<logical_or,int >,internal_param_invert_rule);
	register_function("||",base_op_ot_ttt<logical_or,double >,internal_param_invert_rule);

	register_function("to_int",sil_to_int<string>);
	register_function("to_float",sil_to_float<string>);
	register_function("to_string",sil_to_string<string>);
	register_function("eval",sil_eval);
	register_function("load",sil_load);
	register_function("print",sil_print);
	register_function("read",sil_read);
	register_function("bat",sil_bat);
	register_function("list_function",sil_list_function);
	register_function("list_asm_code",sil_list_asm_code);
	register_function("set_sil",sil_set_sil);
	register_function("help",sil_help);

	register_function("strlen",sil_strlen);
	register_function("substr",sil_substr);
}