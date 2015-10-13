/********************************************************************
说明:			sil解释器 . 一些基本定义和头文件
创建时间:	2013-8-27
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <vector>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <memory>
#include <memory>
#include <functional>
using namespace std;
#define STD_TR1 
#ifdef STD_TR1
using namespace std::tr1;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#pragma warning(disable:4996)
#pragma warning(disable:4267)