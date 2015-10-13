/********************************************************************
说明:			sil解释器 .
创建时间:	2013-8-31
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "sil.h"
#include "normal_helper.cpp"
#include "word_parser.cpp"
#include "grammar_parser.cpp"
#include "symbol_manager.cpp"
#include "code_gen.cpp"
#include "interpret.cpp"
#include "func_extern_lib.cpp"
#include "function_extern.cpp"

bool init_sil()
{
	inital_internal_fun();
	return true;
}
void enter_sil_consle()
{
	//执行一段sil脚本用于设置窗口标题和颜色 并启动一个读取代码字符串并求值的循环
	sil_eval(
		//"bat(\"assoc .sil=sil脚本>nul\nftype sil脚本=notepad.exe %%1 %%*>nul\");"
		"bat(\"title sil\ncolor 5f\");\
		while(true){\
			print(\"sil>\");\
			var code=read();\
			eval(code);\
			print(\"\n\");\
		}");
}