/********************************************************************
说明:			sil解释器 . 支持库
创建时间:	2013-8-28
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
#include "word_parser.h"

extern bool g_display_grammar;
extern bool g_display_debug_info;
class print_hepler
{
public:
	print_hepler(const char* fun_name,int line,bool b_out_fl,bool b_print=true)
		:m_line(line),m_b_out_fl(b_out_fl),m_b_print(b_print)
	{
		strcpy(m_fun_name,fun_name);
	}
	~print_hepler(){}
	void operator()(const char* format,...);

protected:
	char m_fun_name[512];
	int m_line;
	int m_b_out_fl;
	int m_b_print;
};
#define debug_print  print_hepler(__FUNCTION__,__LINE__,true,g_display_debug_info)

class error_printer:public print_hepler
{
public:
	error_printer(const char* fun_name,int line,bool b_out_fl)
		:print_hepler(fun_name,line,b_out_fl){}
	void operator()(const char* format,...);
	static int get_error_count();
	static void clear_error_count(){sm_error_count=0;}
private:
	static int sm_error_count;
};
#define report_error  error_printer(__FUNCTION__,__LINE__,false)

class grammar_printer:public print_hepler
{
public:
	grammar_printer(const char* fun_name,int line,bool b_out_fl,bool b_print)
		:print_hepler(fun_name,line,b_out_fl,b_print){}
	void operator()(word& item);
};
#define grammar_debug grammar_printer(__FUNCTION__,__LINE__,true,g_display_grammar)

bool sil_set_sil(string key,int value);

string get_current_dir();
string get_parent_dir(string dir);
void set_current_dir(const string& dir);
string get_file_full_path(string file_path,string cur_dir="");
string get_file_dir(const string& file_path);