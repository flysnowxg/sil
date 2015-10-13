/********************************************************************
说明:			sil解释器 . 语法解析
创建时间:	2013-8-21
作者:			薛钢 
联系地址:	qq:308821698 （http://t.qq.com/flysnowxg/）
*********************************************************************/
#include "normal_helper.h"
#include "grammar_parser.h"
#include "code_gen.h"
#include "symbol_manager.h"

set<e_word_t> _create_syms(
	e_word_t symol1=ewt_invalid,e_word_t symol2=ewt_invalid,e_word_t symol3=ewt_invalid,
	e_word_t symol4=ewt_invalid,e_word_t symol5=ewt_invalid,e_word_t symol6=ewt_invalid,
	e_word_t symol7=ewt_invalid,e_word_t symol8=ewt_invalid,e_word_t symol9=ewt_invalid)
{
	set<e_word_t> result;
	if(symol1!=ewt_invalid) result.insert(symol1);
	if(symol2!=ewt_invalid) result.insert(symol2);
	if(symol3!=ewt_invalid) result.insert(symol3);
	if(symol4!=ewt_invalid) result.insert(symol4);
	if(symol5!=ewt_invalid) result.insert(symol5);
	if(symol6!=ewt_invalid) result.insert(symol6);
	if(symol7!=ewt_invalid) result.insert(symol7);
	if(symol8!=ewt_invalid) result.insert(symol8);
	if(symol9!=ewt_invalid) result.insert(symol9);
	return result;
}

set<e_word_t> _create_syms(const set<e_word_t>& set1,e_word_t symol1=ewt_invalid,e_word_t symol5=ewt_invalid,
	e_word_t symol2=ewt_invalid,e_word_t symol3=ewt_invalid,e_word_t symol4=ewt_invalid,e_word_t symol6=ewt_invalid,
	e_word_t symol7=ewt_invalid,e_word_t symol8=ewt_invalid)
{
	set<e_word_t> result=set1;
	if(symol1!=ewt_invalid) result.insert(symol1);
	if(symol2!=ewt_invalid) result.insert(symol2);
	if(symol3!=ewt_invalid) result.insert(symol3);
	if(symol4!=ewt_invalid) result.insert(symol4);
	if(symol5!=ewt_invalid) result.insert(symol5);
	if(symol6!=ewt_invalid) result.insert(symol6);
	if(symol7!=ewt_invalid) result.insert(symol7);
	if(symol8!=ewt_invalid) result.insert(symol8);
	return result;
}

set<e_word_t> _create_syms(const set<e_word_t>& set1,const set<e_word_t>& set2,
	e_word_t symol1=ewt_invalid,e_word_t symol2=ewt_invalid,e_word_t symol3=ewt_invalid,e_word_t symol4=ewt_invalid,
	e_word_t symol5=ewt_invalid,e_word_t symol6=ewt_invalid,e_word_t symol7=ewt_invalid,e_word_t symol8=ewt_invalid)
{
	set<e_word_t> result=set1;
	std::copy(set2.begin(),set2.end(),inserter(result,result.end()));
	if(symol1!=ewt_invalid) result.insert(symol1);
	if(symol2!=ewt_invalid) result.insert(symol2);
	if(symol3!=ewt_invalid) result.insert(symol3);
	if(symol4!=ewt_invalid) result.insert(symol4);
	if(symol5!=ewt_invalid) result.insert(symol5);
	if(symol6!=ewt_invalid) result.insert(symol6);
	if(symol7!=ewt_invalid) result.insert(symol7);
	if(symol8!=ewt_invalid) result.insert(symol8);
	return result;
}

class while_context
{
public:
	while_context():m_begin_stk_index(0){}
	int m_begin_stk_index;
	vector<_instruction*> continues;
	vector<_instruction*> breaks;
};

grammar_parser::grammar_parser(void)
{
	m_const_values=_create_syms(ewt_int,ewt_float,ewt_string);
	m_expression_begs=_create_syms(m_const_values,ewt_ident,ewt_key_munis,ewt_key_not,ewt_key_lsbranch);
	m_stats_begs=_create_syms(m_expression_begs,ewt_key_if,ewt_key_while,ewt_key_for,ewt_key_continue,ewt_key_break,ewt_key_return,ewt_key_semicolon);
	m_block_begs=_create_syms(m_stats_begs,ewt_key_var,ewt_key_function,ewt_key_lbbranch,ewt_key_rbbranch);
}

grammar_parser::~grammar_parser(void)
{
}

bool grammar_parser::load_file(const string& file_path)
{
	return m_words.load(file_path);
}

void grammar_parser::load_string(const string& str)
{
	m_words=word_parser(str);
}

void grammar_parser::test_and_skip(set<e_word_t> begin,const set<e_word_t>& follow)
{
	word tmp=m_words.get();
	m_words.push(tmp);
	if(begin.find(tmp.m_type)!=begin.end()) return;
	copy(follow.begin(),follow.end(),inserter(begin,begin.end()));
	while(true)
	{
		tmp=m_words.get();
		if(begin.find(tmp.m_type)!=begin.end()) 
		{
			m_words.push(tmp);
			return;
		}
		else if(tmp.m_type==ewt_invalid) break;
		report_error("语法错误：未识别单词%s\n",tmp.m_str_value.c_str());
	}
}

bool grammar_parser::complie()
{
	error_printer::clear_error_count();
	create_gener();
	_instruction* p_jmp=gen_code(e_jmp,eab_absolute_ip,0,0,true);
	int stk_index=0;
	gen_load_const_instruction(stk_index,1);
	_symbol* p_sym=find_symbol("true");
	gen_code(e_save,p_sym->m_addr_t,p_sym->m_addr);
	gen_pop_instruction(stk_index);
	block(_create_syms(ewt_end_file),stk_index,NULL);
	gen_code(e_exit);
	p_jmp->m_addr=merge_code();
	pop_gener();
	word end_file=m_words.get();
	if(end_file.m_type!=ewt_end_file) 
	{
		report_error("语法错误：未完整识别源代码\n");
		return false;
	}
	return true;
}
//xuegang 未略过第一个符号
void grammar_parser::block(set<e_word_t> follows,int& stk_index,while_context* p_while_conext)
{
	follows=_create_syms(follows,m_block_begs);
	while(true)
	{
		word tmp=m_words.get();
		if(tmp.m_type==ewt_key_var)
		{
			grammar_debug(tmp);
			declare(follows,stk_index);
		}
		else if(tmp.m_type==ewt_key_function)
		{
			grammar_debug(tmp);
			function(follows);
		}
		else if(tmp.m_type==ewt_key_lbbranch)
		{
			grammar_debug(tmp);
			body(follows,stk_index,p_while_conext);
		}
		else if(m_stats_begs.find(tmp.m_type)!=m_stats_begs.end())
		{
			m_words.push(tmp);
			statement(follows,stk_index,p_while_conext);
		}
		else 
		{
			m_words.push(tmp);
			break;
		}
	}
}
//略过第一个符号 '{'
void grammar_parser::body(set<e_word_t> follows,int& stk_index,while_context* p_while_conext)
{
	int stk_index_old=stk_index;
	create_table();
	block(_create_syms(follows,ewt_key_rbbranch),stk_index,p_while_conext);
	pop_table();
	gen_pop_instruction(stk_index,stk_index-stk_index_old);
	word right_branch=m_words.get();
	if(right_branch.m_type==ewt_key_rbbranch) 
	{
		grammar_debug(right_branch);
	}
	else 
	{
		report_error("语法错误：此处应为 ‘}' \n");
		m_words.push(right_branch);
	}
	test_and_skip(follows,_create_syms());
}
//略过第一个符号 'var'
void grammar_parser::declare(set<e_word_t> follows,int& stk_index)
{
	word ident=m_words.get();
	if(ident.m_type==ewt_ident)
	{
		grammar_debug(ident);
		_symbol* p_sym=NULL;
		if((get_top_gener_id()==1)&& (get_global_table()==get_top_table()))
		{
			p_sym=push_symbol(_symbol(es_value,ident.m_str_value,eab_global_data,alloc_global_data_space()),true);
		}
		else
		{
			p_sym=push_symbol(_symbol(es_value,ident.m_str_value,eab_reg_sb,stk_index));
			gen_load_const_instruction(stk_index,0);
		}

		word assgin=m_words.get();
		if(assgin.m_type==ewt_key_assign)
		{
			grammar_debug(assgin);
			expression(follows,stk_index);
			if(p_sym) gen_code(e_save,p_sym->m_addr_t,p_sym->m_addr);
			gen_pop_instruction(stk_index);
		}
		else m_words.push(assgin);
		word semicolon=m_words.get();
		if(semicolon.m_type==ewt_key_semicolon)
		{
			grammar_debug(semicolon);
			return;
		}
		else m_words.push(semicolon);
	}
	else m_words.push(ident);
	test_and_skip(follows,_create_syms());
}
//略过第一个符号 'function'
void grammar_parser::function(set<e_word_t> follows)
{
	//堆栈的样子
	//param1,param2,param3,param_count,ret_vaule,prev_reg_sb,prev_reg_ip,其中prev_reg_sb对应堆栈起始位置
	word ident=m_words.get();
	if(ident.m_type==ewt_ident)
	{
		grammar_debug(ident);
		_symbol* p_sym=push_symbol(_symbol(es_function,ident.m_str_value,eab_absolute_ip,get_new_code_addr(true)),true);
		if(get_top_gener_id()>1) report_error("语法错误：不支持函数内定义函数\n");
		_instruction* p_jmp=gen_code(e_jmp,eab_absolute_ip,0,0,true);
		create_gener();
		create_table();
		word lmbrach=m_words.get();
		if(lmbrach.m_type=ewt_key_lsbranch)
		{
			grammar_debug(lmbrach);
			p_sym->m_fun_params_count=fun_params(follows);
		}
		int stk_index=2;
		word lbbranch=m_words.get();
		if(lbbranch.m_type==ewt_key_lbbranch)
		{
			grammar_debug(lbbranch);
			body(follows,stk_index,NULL);
		}
		else m_words.push(lbbranch);
		gen_code(e_ret);//如果没有写返回语句，加一句默认的返回语句

		p_jmp->m_addr=merge_code();
		pop_table();
		pop_gener();
	}
	else 
	{
		report_error("语法错误：函数定义缺少函数名\n");
		m_words.push(ident);
	}
	test_and_skip(follows,_create_syms());
	
}
//略过第一个符号 '('
int grammar_parser::fun_params(set<e_word_t> follows)
{
	int params_count=0;
	vector<_symbol*> syms;
	while(true)
	{
		word tmp=m_words.get();
		if(tmp.m_type==ewt_ident)
		{
			grammar_debug(tmp);
			_symbol* p_sym=push_symbol(_symbol(es_value,tmp.m_str_value,eab_reg_sb,params_count));
			if(p_sym) syms.push_back(p_sym);
			params_count++;
		}
		else if(tmp.m_type==ewt_key_rsbranch) 
		{
			grammar_debug(tmp);
			for(vector<_symbol*>::iterator it=syms.begin();it!=syms.end();it++)
			{
				(*it)->m_addr-=(params_count+2);
			}
			return params_count;
		}
		else 
		{
			m_words.push(tmp);
			break;
		}
		word comma=m_words.get();
		if(comma.m_type==ewt_key_comma)
		{
			grammar_debug(comma);
			continue;
		}
		else m_words.push(comma);
	}
	test_and_skip(follows,_create_syms());
	return 0;
}
//未略过第一个符号
void grammar_parser::statement(set<e_word_t> follows,int& stk_index,while_context* p_while_conext)
{
	word tmp=m_words.get();
	if(tmp.m_type==ewt_ident)
	{
		m_words.push(tmp);
		if(m_expression_begs.find(tmp.m_type)!=m_expression_begs.end())
		{
			expression(_create_syms(follows,ewt_key_assign),stk_index);
			gen_pop_instruction(stk_index);
			word semicolon=m_words.get();
			if(semicolon.m_type!=ewt_key_semicolon)
			{
				report_error("语法错误：语句应以 ;结束\n");
				m_words.push(semicolon);
			}
			else grammar_debug(semicolon);
		}
	}
	else if(m_expression_begs.find(tmp.m_type)!=m_expression_begs.end())
	{
		m_words.push(tmp);
		expression(follows,stk_index);
		gen_pop_instruction(stk_index);
		word semicolon=m_words.get();
		if(semicolon.m_type!=ewt_key_semicolon) 
		{
			report_error("语法错误：语句应以 ;结束\n");
			m_words.push(semicolon);
		}
		else grammar_debug(semicolon);
	}
	else if(tmp.m_type==ewt_key_if)
	{
		grammar_debug(tmp);
		if_statement(follows,stk_index,p_while_conext);
	}
	else if(tmp.m_type==ewt_key_while)
	{
		grammar_debug(tmp);
		while_statement(follows,stk_index);
	}
	else if(tmp.m_type==ewt_key_for)
	{
		grammar_debug(tmp);
		for_statement(follows,stk_index);
	}
	else if(tmp.m_type==ewt_key_break)
	{
		grammar_debug(tmp);
		if(p_while_conext==NULL)
		{
			report_error("语法错误：不在循环中，此处不应有break;\n");
		}
		else 
		{
			gen_code(e_pop,eab_invalid,0,stk_index-p_while_conext->m_begin_stk_index);
			p_while_conext->breaks.push_back(gen_code(e_jmp,eab_relat_ip,0,0));
		}
		if(m_words.check(ewt_key_semicolon)) m_words.get();
		else 
		{
			report_error("语法错误：break语句缺少分号\n");
		}
	}
	else if(tmp.m_type==ewt_key_continue)
	{
		grammar_debug(tmp);
		if(p_while_conext==NULL)
		{
			report_error("语法错误：不在循环中，此处不应有continue;\n");
		}
		else 
		{
			gen_code(e_pop,eab_invalid,0,stk_index-p_while_conext->m_begin_stk_index);
			p_while_conext->continues.push_back(gen_code(e_jmp,eab_relat_ip,0,0));
		}
		if(m_words.check(ewt_key_semicolon)) m_words.get();
		else 
		{
			report_error("语法错误：break语句缺少分号\n");
		}
	}
	else if(tmp.m_type==ewt_key_return)
	{
		grammar_debug(tmp);
		return_statement(follows,stk_index);
	}
	else if(tmp.m_type==ewt_key_semicolon)
	{
		grammar_debug(tmp);
	}
	else 
	{
		m_words.push(tmp);
		test_and_skip(follows,_create_syms());
	}
}
//略过第一个符号 'if'
void grammar_parser::if_statement(set<e_word_t> follows,int& stk_index,while_context* p_while_conext)
{
	word lmbranch=m_words.get();
	if(lmbranch.m_type==ewt_key_lsbranch)
	{
		grammar_debug(lmbranch);
		expression(_create_syms(follows,ewt_key_rsbranch),stk_index);
		word rmbranch=m_words.get();
		if(rmbranch.m_type==ewt_key_rsbranch)
		{
			grammar_debug(rmbranch);
			_instruction* p_if_jmp=gen_code(e_jpc,eab_relat_ip,0,0);//if语句应当在执行完判断后把栈顶元素弹出
			stk_index--;
			int inst_if_body_begin=get_new_code_addr();
			word lbbranch=m_words.get();
			if(lbbranch.m_type==ewt_key_lbbranch)
			{
				grammar_debug(lbbranch);
				body(_create_syms(follows,ewt_key_else),stk_index,p_while_conext);
			}
			else 
			{
				m_words.push(lbbranch);
				statement(_create_syms(follows,ewt_key_else),stk_index,p_while_conext);
			}
			word wd_else=m_words.get();
			if(wd_else.m_type==ewt_key_else)
			{
				grammar_debug(wd_else);
				_instruction* p_else_jmp=gen_code(e_jmp,eab_relat_ip,0,0);
				int inst_else_body_begin=get_new_code_addr();
				p_if_jmp->m_addr=get_new_code_addr()-inst_if_body_begin;
				word lbbranch=m_words.get();
				if(lbbranch.m_type==ewt_key_lbbranch)
				{
					grammar_debug(lbbranch);
					body(follows,stk_index,p_while_conext);
				}
				else 
				{
					m_words.push(lbbranch);
					statement(_create_syms(follows,ewt_key_else),stk_index,p_while_conext);
				}
				p_else_jmp->m_addr=get_new_code_addr()-inst_else_body_begin;
			}
			else
			{
				m_words.push(wd_else);
				p_if_jmp->m_addr=get_new_code_addr()-inst_if_body_begin;
			}
		}
	}
	else m_words.push(lmbranch);
	test_and_skip(follows,_create_syms());
}
//略过第一个符号 'while'
void grammar_parser::while_statement(set<e_word_t> follows,int& stk_index)
{
	word lmbranch=m_words.get();
	if(lmbranch.m_type==ewt_key_lsbranch)
	{
		grammar_debug(lmbranch);
		int test_begin=get_new_code_addr(false);
		expression(_create_syms(follows,ewt_key_rsbranch),stk_index);
		word rmbranch=m_words.get();
		if(rmbranch.m_type==ewt_key_rsbranch)
		{
			grammar_debug(rmbranch);
			_instruction* p_while_jmp=gen_code(e_jpc,eab_relat_ip);
			stk_index--;
			while_context context;
			context.m_begin_stk_index=stk_index;
			word lbbranch=m_words.get();
			if(lbbranch.m_type==ewt_key_lbbranch)
			{
				grammar_debug(lbbranch);
				body(follows,stk_index,&context);
			}
			else 
			{
				m_words.push(lbbranch);
				statement(_create_syms(follows,ewt_key_else),stk_index,&context);
			}
			_instruction* p_loop_jmp=gen_code(e_jmp,eab_relat_ip);
			p_loop_jmp->m_addr=test_begin-(p_loop_jmp->m_index+1);
			p_while_jmp->m_addr=get_new_code_addr()-(p_while_jmp->m_index+1);
			for(vector<_instruction*>::iterator it=context.breaks.begin();it!=context.breaks.end();it++)
			{
				(*it)->m_addr=get_new_code_addr()-((*it)->m_index+1);
			}
			for(vector<_instruction*>::iterator it=context.continues.begin();it!=context.continues.end();it++)
			{
				(*it)->m_addr=test_begin-((*it)->m_index+1);
			}
		}
	}
	else
	{
		m_words.push(lmbranch);
		test_and_skip(follows,_create_syms());
	}
}
//略过第一个符号 'for'
void grammar_parser::for_statement(set<e_word_t> follows,int& stk_index)
{
	word lmbranch=m_words.get();
	if(lmbranch.m_type==ewt_key_lsbranch)
	{
		grammar_debug(lmbranch);
		if(m_words.check(ewt_key_semicolon))
		{
			m_words.get();
		}
		else if(m_words.check(ewt_key_var)) 
		{
			m_words.get();
			declare(follows,stk_index);
		}
		else
		{
			expression(follows,stk_index);
			gen_pop_instruction(stk_index);
			word tmp=m_words.get();
			if(tmp.m_type!=ewt_key_semicolon)
			{
				report_error("for语句中缺少分号\n");
				m_words.push(tmp);
			}
		}

		_instruction* p_jmp_to_end=NULL;
		int test_begin=get_new_code_addr();
		if(m_words.check(ewt_key_semicolon))
		{
			m_words.get();
			p_jmp_to_end=gen_pop_instruction(stk_index,0);
		}
		else
		{
			expression(follows,stk_index);
			p_jmp_to_end=gen_code(e_jpc,eab_relat_ip);
			stk_index--;
			word tmp=m_words.get();
			if(tmp.m_type!=ewt_key_semicolon)
			{
				report_error("for语句中缺少分号\n");
				m_words.push(tmp);
			}
		}
		_instruction* p_jmp_to_body=gen_code(e_jmp,eab_relat_ip);

		if(m_words.check(ewt_key_rsbranch));
		else
		{
			expression(_create_syms(follows,ewt_key_rsbranch),stk_index);
			gen_pop_instruction(stk_index);
		}
		_instruction* p_jmp_to_test=gen_code(e_jmp,eab_relat_ip);
		p_jmp_to_test->m_addr=test_begin-(p_jmp_to_test->m_index+1);
		p_jmp_to_body->m_addr=get_new_code_addr()-(p_jmp_to_body->m_index+1);
		word tmp=m_words.get();
		if(tmp.m_type!=ewt_key_rsbranch)
		{
			report_error("for语句中缺少分号\n");
			m_words.push(tmp);
		}

		while_context context;
		context.m_begin_stk_index=stk_index;
		word lbbranch=m_words.get();
		if(lbbranch.m_type==ewt_key_lbbranch)
		{
			grammar_debug(lbbranch);
			body(follows,stk_index,&context);
		}
		else 
		{
			m_words.push(lbbranch);
			statement(_create_syms(follows,ewt_key_else),stk_index,&context);
		}
		_instruction* p_loop_jmp=gen_code(e_jmp,eab_relat_ip);
		p_loop_jmp->m_addr=(p_jmp_to_body->m_index+1)-(p_loop_jmp->m_index+1);
		p_jmp_to_end->m_addr=get_new_code_addr()-(p_jmp_to_end->m_index+1);
		for(vector<_instruction*>::iterator it=context.breaks.begin();it!=context.breaks.end();it++)
		{
			(*it)->m_addr=get_new_code_addr()-((*it)->m_index+1);
		}
		for(vector<_instruction*>::iterator it=context.continues.begin();it!=context.continues.end();it++)
		{
			(*it)->m_addr=(p_jmp_to_body->m_index+1)-((*it)->m_index+1);
		}
	}
	else m_words.push(lmbranch);
	test_and_skip(follows,_create_syms());
}
//略过第一个符号 'return'
void grammar_parser::return_statement(set<e_word_t> follows,int& stk_index)
{
	if(!m_words.check(ewt_key_semicolon))
	{
		expression(follows,stk_index);
		gen_code(e_save,eab_reg_sb,-1);
		gen_pop_instruction(stk_index);
	}
	gen_code(e_ret);
	word tmp=m_words.get();
	if(tmp.m_type==ewt_key_semicolon)
	{
		grammar_debug(tmp);
		return;
	}
	else m_words.push(tmp);
}

//未略过第一个符号
void grammar_parser::expression(set<e_word_t> follows,int& stk_index)
{
	if(m_words.check(ewt_ident,ewt_key_assign))
	{
		assign_expression(follows,stk_index);
	}
	else
	{
		factor_logic(_create_syms(follows,
			_create_syms(ewt_key_logic_and,ewt_key_logic_or,ewt_key_eql,ewt_key_neql,ewt_key_less,ewt_key_greate),
			ewt_key_plus,ewt_key_munis,ewt_key_mul,ewt_key_div),stk_index);
	}
}
//未略过第一个符号
void grammar_parser::assign_expression(set<e_word_t> follows,int& stk_index)
{
	word ident=m_words.get();
	_symbol* p_sym=find_symbol(ident.m_str_value);
	inst_value vaule(0);
	word assgin=m_words.get();
	if(assgin.m_type==ewt_key_assign)
	{
		grammar_debug(assgin);
		expression(follows,stk_index);
		if(p_sym && p_sym->m_type==es_value)
		{
			gen_code(e_save,p_sym->m_addr_t,p_sym->m_addr);
		}
		else report_error("语法错误：未找到符号%s\n",ident.m_str_value.c_str());
		return;
	}
	else m_words.push(assgin);
	test_and_skip(follows,_create_syms());
}
void grammar_parser::factor_logic(set<e_word_t> follows,int& stk_index)
{
	factor_compare(follows,stk_index);
	while(true)
	{
		string fun_name;
		word tmp=m_words.get();
		if(tmp.m_type==ewt_key_logic_and) fun_name="&&";
		else if(tmp.m_type==ewt_key_logic_or) fun_name="||";
		else m_words.push(tmp);
		if(fun_name.size()>0)
		{
			grammar_debug(tmp);
			factor_compare(follows,stk_index);
			gen_load_const_instruction(stk_index,2);
			gen_fun_call(fun_name,2,stk_index);
		}
		else break;
	}
}
//未略过第一个符号
void grammar_parser::factor_compare(set<e_word_t> follows,int& stk_index)
{
	factor_plus_minus(follows,stk_index);
	while(true)
	{
		string fun_name;
		word tmp=m_words.get();
		if(tmp.m_type==ewt_key_eql) fun_name="==";
		else if(tmp.m_type==ewt_key_neql) fun_name="!=";
		else if(tmp.m_type==ewt_key_less) fun_name="<";
		else if(tmp.m_type==ewt_key_greate) fun_name=">";
		else m_words.push(tmp);
		if(fun_name.size()>0)
		{
			grammar_debug(tmp);
			factor_plus_minus(follows,stk_index);
			gen_load_const_instruction(stk_index,2);
			gen_fun_call(fun_name,2,stk_index);
		}
		else break;
	}
}
//未略过第一个符号
void grammar_parser::factor_plus_minus(set<e_word_t> follows,int& stk_index)
{
	factor_mul_div(follows,stk_index);
	while(true)
	{
		string fun_name;
		word tmp=m_words.get();
		if(tmp.m_type==ewt_key_plus) fun_name="+";
		else if(tmp.m_type==ewt_key_munis) fun_name="-";
		else m_words.push(tmp);
		if(fun_name.size()>0)
		{
			grammar_debug(tmp);
			factor_mul_div(follows,stk_index);
			gen_load_const_instruction(stk_index,2);
			gen_fun_call(fun_name,2,stk_index);
		}
		else break;
	}
}
//未略过第一个符号
void grammar_parser::factor_mul_div(set<e_word_t> follows,int& stk_index)
{
	factor_single(follows,stk_index);
	while(true)
	{
		string fun_name;
		word tmp=m_words.get();
		if(tmp.m_type==ewt_key_mul) fun_name="*";
		else if(tmp.m_type==ewt_key_div) fun_name="/";
		else if(tmp.m_type==ewt_key_mod) fun_name="%";
		else m_words.push(tmp);
		if(fun_name.size()>0)
		{
			grammar_debug(tmp);
			factor_single(follows,stk_index);
			gen_load_const_instruction(stk_index,2);
			gen_fun_call(fun_name,2,stk_index);
		}
		else break;
	}
}
//未略过第一个符号
void grammar_parser::factor_single(set<e_word_t> follows,int& stk_index)
{
	string fun_name;
	word tmp=m_words.get();
	if(tmp.m_type==ewt_key_munis) fun_name="-";
	else if(tmp.m_type==ewt_key_not) fun_name="!";
	else m_words.push(tmp);
	if(fun_name.size()>0)
	{
		grammar_debug(tmp);
	}
	term(follows,stk_index);
	if(fun_name.size()>0)
	{
		gen_load_const_instruction(stk_index,1);
		gen_fun_call(fun_name,1,stk_index);
	}
}
//未略过第一个符号
void grammar_parser::term(set<e_word_t> follows,int& stk_index)
{
	word tmp=m_words.get();
	if(m_const_values.find(tmp.m_type)!=m_const_values.end())
	{
		grammar_debug(tmp);
		inst_value value(0);
		if(tmp.m_type==ewt_int) value=inst_value(tmp.m_int_value);
		else if(tmp.m_type==ewt_float) value=inst_value(tmp.m_float_value);
		else if(tmp.m_type==ewt_string) value=inst_value(tmp.m_str_value);
		gen_load_const_instruction(stk_index,value);
	}
	else if(tmp.m_type==ewt_ident)
	{
		word lmbranch=m_words.get();
		if(lmbranch.m_type==ewt_key_lsbranch) 
		{
			m_words.push(lmbranch);
			m_words.push(tmp);
			funcall_expression(follows,stk_index);
		}
		else 
		{
			m_words.push(lmbranch);
			grammar_debug(tmp);
			_symbol* p_sym=find_symbol(tmp.m_str_value);
			if(p_sym && p_sym->m_type==es_value)
			{
				gen_code(e_load,p_sym->m_addr_t,p_sym->m_addr);
				stk_index++;
			}
			else report_error("语法错误：未找到符号%s\n",tmp.m_str_value.c_str());
		}
	}
	else if(tmp.m_type==ewt_key_lsbranch)
	{
		grammar_debug(tmp);
		factor_logic(_create_syms(follows,ewt_key_rsbranch),stk_index);
		word rmbranch=m_words.get();
		if(rmbranch.m_type==ewt_key_rsbranch)
		{
			grammar_debug(rmbranch);
		}
		else m_words.push(rmbranch);
	}
	else m_words.push(tmp);
	test_and_skip(follows,_create_syms());
}

//未略过第一个符号
void grammar_parser::funcall_expression(set<e_word_t> follows,int& stk_index)
{
	word ident=m_words.get();
	grammar_debug(ident);
	word lmbranch=m_words.get();
	grammar_debug(lmbranch);
	int param_count=0;
	while(true)
	{
		word wd_tmp=m_words.get();
		if(wd_tmp.m_type==ewt_key_rsbranch) 
		{
			grammar_debug(wd_tmp);
			gen_load_const_instruction(stk_index,param_count);// 压入参数个数
			gen_fun_call(ident.m_str_value,param_count,stk_index);
			return;
		}
		else if(m_expression_begs.find(wd_tmp.m_type)!=m_expression_begs.end())
		{
			m_words.push(wd_tmp);
			factor_logic(_create_syms(follows,ewt_key_comma,ewt_key_rsbranch),stk_index);
			param_count++;
		}
		else
		{
			m_words.push(wd_tmp);
			break;
		}
		word wd_tmp2=m_words.get();
		if(wd_tmp2.m_type==ewt_key_comma)
		{
			grammar_debug(wd_tmp2);
		}
		else if(wd_tmp2.m_type==ewt_key_rsbranch) 
		{
			m_words.push(wd_tmp2);
		}
		else
		{
			m_words.push(wd_tmp2);
			break;
		}
	}
	test_and_skip(follows,_create_syms());
}

bool sil_eval(string code)
{
	int ip=get_new_code_addr(true);
	grammar_parser parser;
	parser.load_string(code);
	parser.complie();
	if(error_printer::get_error_count()==0)
	{
		//print_code();
		interpret vm(get_top_gener());
		return vm.run(ip);
	}
	return false;
}

bool sil_load(string file_path)
{
	string cur_dir=get_current_dir();
	string file_full_path=get_file_full_path(file_path);
	set_current_dir(get_file_dir(file_path));
	int ip=get_new_code_addr(true);
	grammar_parser parser;
	parser.load_file(file_full_path);
	parser.complie();
	bool b_result=false;
	if(error_printer::get_error_count()==0)
	{
		//print_code();
		interpret vm(get_top_gener());
		vm.run(ip);
		b_result=true;
	}
	set_current_dir(cur_dir);
	return b_result;
}
