/********************************************************************
说明:			sil解释器 . 
创建时间:	2013-8-31
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#pragma once
#include "base_define.h"
#include "normal_helper.h"
#include "word_parser.h"
#include "grammar_parser.h"
#include "symbol_manager.h"
#include "code_gen.h"
#include "interpret.h"
#include "func_extern_lib.h"
#include "function_extern.h"

bool init_sil();//初始化sil
void enter_sil_consle();//sil控制台
