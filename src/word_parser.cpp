/********************************************************************
说明:			sil解释器 . 词法分析
创建时间:	2013-8-20
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "word_parser.h"
#include "normal_helper.h"

word_parser::word_parser(const string& code)
:m_code(code),
m_current_index(0),
m_current_line(0)
{
}

bool word_parser::load(const string& file_path)
{
	m_code.clear();
	m_current_index=0;
	m_current_line=0;
	while(m_cache.size()) m_cache.pop_front();
	FILE* file=fopen(file_path.c_str(),"r");
	if(file==NULL) 
	{
		report_error("打开文件%s失败\n",file_path.c_str());
		return false;
	}
	fseek(file,0,SEEK_END);
	long size=ftell(file);
	fseek(file,0,SEEK_SET);
	char* buf=new char[size+1];
	memset(buf,0,size+1);
	fread(buf,size,1,file);
	m_code=buf;
	delete[] buf;
	fclose(file);
	return true;
}

word word_parser::get()
{
	if(m_cache.size()>0) 
	{
		word tmp=m_cache.front();
		m_cache.pop_front();
		return tmp;
	}
	string result;
	while (true)
	{
		escape();
		int old_index=m_current_index;
		if(m_current_index==m_code.size()) 
		{
			m_current_index++;
			return word(ewt_end_file,"EOF");
		}
		else if(m_current_index>m_code.size()) return word();
		word result=get_number();
		if(result.m_type!=ewt_invalid) return result;
		result=get_string();
		if(result.m_type!=ewt_invalid) return result;
		result=get_ident_or_key();
		if(result.m_type!=ewt_invalid) return result;
		result=get_other();
		if(result.m_type!=ewt_invalid) return result;
		report_error("词法分析：无效字符(第%d行):%c\n",m_current_line,m_code[m_current_index]);
		m_current_index++;
	}
}

bool word_parser::check(e_word_t word1_t,e_word_t word2_t)
{
	word tmp1=get();
	if(tmp1.m_type!=word1_t) 
	{
		push(tmp1);
		return false;
	}
	else
	{
		bool b_right=true;
		if(word2_t!=ewt_invalid)
		{
			word tmp2=get();
			if(tmp2.m_type!=word2_t) b_right= false;
			push(tmp2);
		}
		push(tmp1);
		return b_right;
	}
}

void word_parser::push(const word& item)
{
	m_cache.push_front(item);
}

void  word_parser::escape()
{
	while(true) 
	{
		int old_index=m_current_index;
		escape_space();
		escape_note();
		if(old_index==m_current_index) break;
	}

}
void word_parser::escape_space()
{
	while(true)
	{
		if(m_current_index>=m_code.size()) break;
		if((m_code[m_current_index]==' ') ||
			(m_code[m_current_index]=='\t') ||
			(m_code[m_current_index]=='\r') ||
			(m_code[m_current_index]=='\n') )
		{
			m_current_index++;
			if(m_code[m_current_index]=='\n') m_current_line++;
		}
		else break;
	}
}

void word_parser::escape_note()
{
	if(m_current_index+1>m_code.size()) return;
	if((m_code[m_current_index]=='/')&& (m_code[m_current_index+1]=='/'))
	{
		m_current_index+=2;
		while(true)
		{
			char tmp=m_code[m_current_index-1];
			char tmp2=m_code[m_current_index];
			if(m_current_index>m_code.size()) return;
			m_current_index++;
			if(m_code[m_current_index-1]=='\n') 
			{
				return;
			}
		}
	}
}

word word_parser::get_number()
{
	if(m_current_index>=m_code.size()) return word();
	word result;
	while((m_code[m_current_index]>='0')&& (m_code[m_current_index]<='9'))
	{
		result.m_type=ewt_int;
		result.m_str_value+=m_code[m_current_index];
		result.m_int_value=result.m_int_value*10+m_code[m_current_index]-'0';
		result.m_float_value=result.m_int_value;
		m_current_index++;
		if(m_current_index>=m_code.size()) break;
	}
	if(m_current_index>=m_code.size()) return result;
	if(result.m_type==ewt_invalid) return result;
	//浮点数格式[0-9]+\.[0-9]+(e[-]?[0-9])?
	if(m_code[m_current_index]=='.')
	{
		result.m_type=ewt_float;
		result.m_str_value+=m_code[m_current_index];
		m_current_index++;
		if(m_current_index>=m_code.size()) return result;
		int base=10;
		while((m_code[m_current_index]>='0')&& (m_code[m_current_index]<='9'))
		{
			result.m_str_value+=m_code[m_current_index];
			result.m_float_value=(result.m_float_value*base+m_code[m_current_index]-'0')/base;
			m_current_index++;
			if(m_current_index>=m_code.size()) break;
			base*=10;
		}
		if(m_current_index>=m_code.size()) return result;
	}
	if(m_code[m_current_index]=='e')
	{
		result.m_type=ewt_float;
		result.m_str_value+=m_code[m_current_index];
		m_current_index++;
		if(m_current_index>=m_code.size()) return result;
		bool b_minus=false;
		if(m_code[m_current_index]=='-')
		{
			b_minus=true;
			result.m_str_value+=m_code[m_current_index];
			m_current_index++;
			if(m_current_index>=m_code.size()) return result;
		}
		float figure=0;
		while((m_code[m_current_index]>='0')&& (m_code[m_current_index]<='9'))
		{
			result.m_str_value+=m_code[m_current_index];
			figure=figure*10+m_code[m_current_index]-'0';
			m_current_index++;
			if(m_current_index>=m_code.size()) break;
		}
		for(int count_i=0;count_i<figure;count_i++)
		{
			if(b_minus) result.m_float_value/=10;
			else result.m_float_value*=10;
		}
	}
	return result;
}

word word_parser::get_string()
{
	if(m_current_index>=m_code.size()) return word();
	if(m_code[m_current_index]=='\"')
	{
		m_current_index++;
		if(m_current_index>=m_code.size()) return word();
		word result(ewt_string);
		while (true)
		{
			if(m_code[m_current_index]=='\"')
			{
				m_current_index++;
				return result;
			}
			if(m_code[m_current_index]=='\\')
			{
				m_current_index++;
				if(m_current_index>=m_code.size()) return word();
				char tmp=m_code[m_current_index];
				if(tmp=='n') result.m_str_value+='\n';
				else if(tmp=='t') result.m_str_value+='\t';
				else if(tmp=='r') result.m_str_value+='\r';
				else if(tmp=='\\') result.m_str_value+='\\';
				else result.m_str_value+=tmp;
				m_current_index++;
			}
			else
			{
				result.m_str_value+=m_code[m_current_index];
				m_current_index++;
			}
			if(m_current_index>=m_code.size()) break;
		}
	}
	return word();
}

word word_parser::get_ident_or_key()
{
	static map<string,e_word_t> s_keys;
	if(s_keys.empty())
	{
		s_keys["var"]=ewt_key_var;
		s_keys["function"]=ewt_key_function;
		s_keys["if"]=ewt_key_if;
		s_keys["else"]=ewt_key_else;
		s_keys["while"]=ewt_key_while;
		s_keys["for"]=ewt_key_for;
		s_keys["continue"]=ewt_key_continue;
		s_keys["break"]=ewt_key_break;
		s_keys["return"]=ewt_key_return;
	}
	if(m_current_index>=m_code.size()) return word();
	if((m_code[m_current_index]>='A')&& (m_code[m_current_index]<='z'))
	{
		word result(ewt_ident);
		char tmp=m_code[m_current_index];
		while ((tmp>'A'&&tmp<'z')||(tmp>'0'&&tmp<'9'))
		{
			result.m_str_value+=tmp;
			m_current_index++;
			if(m_current_index>=m_code.size()) break;
			tmp=m_code[m_current_index];
		}
		map<string,e_word_t>::iterator it=s_keys.find(result.m_str_value);
		if(it!=s_keys.end()) result.m_type=it->second;
		return result;
	}
	return word();
}

word word_parser::get_other()
{
	static map<string,e_word_t> s_keys;
	if(s_keys.empty())
	{
		s_keys[","]=ewt_key_comma;
		s_keys[";"]=ewt_key_semicolon;
		s_keys["("]=ewt_key_lsbranch;
		s_keys[")"]=ewt_key_rsbranch;
		s_keys["{"]=ewt_key_lbbranch;
		s_keys["}"]=ewt_key_rbbranch;
		s_keys["="]=ewt_key_assign;
		s_keys["!"]=ewt_key_not;
		s_keys["*"]=ewt_key_mul;
		s_keys["/"]=ewt_key_div;
		s_keys["%"]=ewt_key_mod;
		s_keys["+"]=ewt_key_plus;
		s_keys["-"]=ewt_key_munis;
		s_keys["=="]=ewt_key_eql;
		s_keys["!="]=ewt_key_neql;
		s_keys["<"]=ewt_key_less;
		s_keys[">"]=ewt_key_greate;
		s_keys["||"]=ewt_key_logic_or;
		s_keys["&&"]=ewt_key_logic_and;
	}
	if(m_current_index>=m_code.size()) return word();
	if(m_code.size()>(m_current_index+1))
	{
		string tmp=m_code.substr(m_current_index,2);
		map<string,e_word_t>::iterator it=s_keys.find(tmp);
		if(it!=s_keys.end()) 
		{
			m_current_index+=2;
			return word(it->second,it->first);
		}
	}
	string tmp(1,m_code[m_current_index]);
	map<string,e_word_t>::iterator it=s_keys.find(tmp);
	if(it!=s_keys.end())  
	{
		m_current_index++;
		return word(it->second,it->first);
	}
	return word();
}

void parse_file_word(const string& file_path)
{
	word_parser parser;
	parser.load(file_path);
	int count=0;
	while(true)
	{
		word tmp=parser.get();
		if(tmp.m_type==ewt_invalid) break;
		else printf("%d: %s\n",count++,tmp.m_str_value.c_str());
	}
}