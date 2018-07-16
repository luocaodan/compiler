#pragma once
#include "stdafx.h"
#include "error.h"
#include "table.h"
#include "insymbol.h"
#include "midcode.h"
#include <limits>

extern table sym_tab;				// 符号表
extern map<string, string> str_tab;	// 字符串常量表
extern ofstream grammar_stream;		// 语法分析输出流
extern bool grammar_flag;			// 为true则打印语法分析结果

void program();																	// 程序
void const_decl(int flag);														// 常量说明 flag 1 为全局 0 为局部
void var_decl(int flag);														// 变量说明，返回可能读到的函数信息 flag 1 为全局 0 为局部
void func_def();																// 有返回值函数定义
void proc_def();																// 无返回值函数定义
void main_func();																// 主函数
void const_def(int flag);														// 常量定义
int constant_liter(int sign);													// 处理常量
void var_def(int flag);															// 变量定义
string decl_head();																// 声明头部
void parameters();																// 参数表
void compound_stat();															// 复合语句
void stat_column();																// 语句列
void statements();																// 语句
void if_stat();																	// if语句
void while_stat();																// while语句
string condition();																// 条件
void scanf_stat();																// 读语句
void printf_stat();																// 写语句
void case_stat();																// switch 语句
void case_label(string expr_var, string switch_end);							// 情况表
void one_case(string expr_var, string switch_end);								// 情况子语句
void default_stat(string switch_end);											// 缺省
void return_stat();																// 返回语句
string call(bool factor_flag);													// 调用语句
void assign_stat();																// 赋值语句
void value_para(string func_name);												// 值参数表
void expression();																// 表达式
void term();																	// 项
void factor();																	// 因子