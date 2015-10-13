/********************************************************************
说明:			sil解释器
创建时间:	2013-8-30
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "../src/sil.h"

int main(int argc, char* argv[])
{
	init_sil();
	//如果传入一个要解释的文件，解释执行文件即可，否则启动命令行解释器
	if(argc<2)
	{
		printf("sil v0.1(创建时间:%s)author:xuegang[qq:308821698]\n",__TIMESTAMP__ );
		printf("输入  help();  可显示帮助信息\n");
		interpret::is_output_last_value(true);
		enter_sil_consle();
	}
	else
	{
		sil_load(argv[1]);
	}
	return 0;
}