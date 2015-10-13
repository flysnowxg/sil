/********************************************************************
说明:			sil解释器 . 支持库
创建时间:	2013-8-28
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "normal_helper.h"
#ifdef GCC
#include <unistd.h>
#else
#include <direct.h>
#endif
bool g_display_grammar=false;
bool g_display_debug_info=false;
void print_hepler::operator()(const char* format,...)
{
	if(!m_b_print) return;
	if(m_b_out_fl) printf("%s:%d ",m_fun_name,m_line);
	va_list arg_list;
	va_start(arg_list,format);
	vprintf(format,arg_list);
	va_end(arg_list);
}

int error_printer::sm_error_count=0;

void error_printer::operator()(const char* format,...)
{
	sm_error_count++;
	if(!m_b_print) return;
	if(m_b_out_fl) printf("%s:%d ",m_fun_name,m_line);
	va_list arg_list;
	va_start(arg_list,format);
	vprintf(format,arg_list);
	va_end(arg_list);
}

int error_printer::get_error_count()
{
	return sm_error_count;
}

void grammar_printer::operator()(word& item)
{
	if(!m_b_print) return;
	if(m_b_out_fl) printf("%s:%d ==>   ",m_fun_name,m_line);
	printf("%s\n",item.m_str_value.c_str());
}

bool sil_set_sil(string key,int value)
{
	int select=0;
	vector<pair<string,string> > cmds;
	cmds.push_back(make_pair("grammar_parser","显示语法解析过程"));
	cmds.push_back(make_pair("debug_info","显示调试信息"));
	cmds.push_back(make_pair("last_v","显示解释器执行的最后一句代码的返回值"));
	//cmds.push_back(make_pair("asm","显示编译后的代码"));
	if(key==cmds[0].first) 
	{
		g_display_grammar=value?true:false;
		select=0;
	}
	else if(key==cmds[1].first) 
	{
		g_display_debug_info=value?true:false;
		select=1;
	}
	else if(key==cmds[2].first) 
	{
		interpret::is_output_last_value(value?true:false);
		select=2;
	}
	//else if(key==cmds[3].first) g_display_debug_info=value?true:false;
	else
	{
		printf("sil内部命令列表:\n");
		printf("\t使用格式：set_sil(command,b_enable); 例如：set_sil(\"last_v\",false);\n");
		printf("命令列表:\n");
		for(vector<pair<string,string> >::iterator it=cmds.begin();it!=cmds.end();++it)
		{
				printf("\t%s  [不]%s\n",it->first.c_str(),it->second.c_str());
		}
		return 0;
	}
	printf("%s%s",(value?"开启":"关闭"),cmds[select].second.c_str());
	return 0;
}

static string g_current_dir;
string get_current_dir()
{
	if(g_current_dir=="")
	{
		char dir[256];
		getcwd(dir,256);
		g_current_dir=dir;
	}
	if(g_current_dir.size()==0) return "";
	int index=g_current_dir.size()-1;
	if(g_current_dir[index]=='/' ||g_current_dir[index]=='/')g_current_dir[index]=0;
	return g_current_dir;
}
string get_parent_dir(string dir)
{
	int pos=dir.find_last_of("/\\");
	if(pos==string::npos) return "";
	return dir.substr(0,pos);
}
void set_current_dir(const string& dir)
{
	g_current_dir=dir;
}
string get_file_full_path(string file_path,string cur_dir)
{
	//xuegang 5种情况
	if(cur_dir=="") cur_dir=get_current_dir();
	if(file_path.size()<2) return file_path;
	if(file_path[0]=='.')
	{
		if(file_path[1]=='.')// ../../dir/file
		{
			if(file_path.size()<3) return "";
			else if(file_path[2]!='\\' && file_path[2]!='/') return "";
			return get_file_full_path(file_path.erase(0,3),get_parent_dir(cur_dir));
		}
		else if(file_path[1]=='\\' || file_path[1]=='/')// ./dir/file
		{
			return cur_dir+file_path.erase(0,1);
		}
		else return "";
	}
	else if(file_path[0]=='\\')// \dir/file
	{
		return cur_dir.substr(0,2)+file_path;
	}
	else if(file_path[1]==':')// c:/dir/file
	{
		return file_path;
	}
	else// dir/file
	{
		return cur_dir+"/"+file_path;
	}
}

string get_file_dir(const string& file_path)
{
	return get_parent_dir(get_file_full_path(file_path));
}