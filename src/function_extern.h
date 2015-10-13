/********************************************************************
说明:			sil解释器 . 扩展函数
创建时间:	2013-8-32
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
//sil语言内置的函数定义
template<typename T> int sil_to_int(T value);
template<typename T> double sil_to_float(T value);
template<typename T> string sil_to_string(T value);
int sil_print(string str);
string sil_read();
bool sil_load(string file_path);
bool sil_eval(string sil_code);
int sil_bat(string win_script);
int sil_list_function();
int sil_list_asm_code();
int sil_help();