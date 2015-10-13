/********************************************************************
说明:			sil解释器 . 扩展的例子
创建时间:	2013-8-31
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
//这是一个例子，展示了如何去扩展sil去支持对文件进行操作
//首先会定义一些符合sil接口规范的函数（由于sil的局限，sil只支持参数是int float string 这三种类型的函数）
//然后调用register_function();注册函数
//最后调用file_rw.sil脚本来操作文件
#include "../src/sil.h"
#pragma warning(disable:4311)
#pragma warning(disable:4312)
typedef int file_handle;
file_handle sil_fopen(string file_path,string open_mode)
{
	FILE* fp=fopen(file_path.c_str(),open_mode.c_str());
	return (file_handle)fp;
}
int sil_fcolse(file_handle file)
{
	return fclose((FILE*)file);
}
string sil_fread(file_handle file,int size)
{
	char* p_buf=new char[size+1];
	fread(p_buf,1,size,(FILE*)file);
	return p_buf;
}
bool sil_fwrite(file_handle file,string data)
{
	int size=fwrite(data.c_str(),1,data.size(),(FILE*)file);
	return size==data.size();
}
int sil_fseek(file_handle file,int mode,int offset)
{
	return fseek((FILE*)file,offset,mode);
}
int sil_ftell(file_handle file)
{
	return ftell((FILE*)file);
}
int main(int argc, char* argv[])
{
	printf("sil v0.1(创建时间:%s)author:xuegang[qq:308821698]\n",__TIMESTAMP__ );
	printf("这只是个例子用于展示如何去用c++扩展sil的函数调用\n");
	init_sil();
	register_function("fopen",sil_fopen);
	register_function("fclose",sil_fcolse);
	register_function("fread",sil_fread);
	register_function("fwrite",sil_fwrite);
	register_function("fseek",sil_fseek);
	register_function("ftell",sil_ftell);
	interpret::is_output_last_value(true);
	sil_load("../extern/file_rw.sil");
	enter_sil_consle();
	return 0;
}

