#pragma once
#include "stdafx.h"
#include "insymbol.h"

struct compile_err {
	int line;
	int start;
	int end;
	int err_code;
};

enum err_no {
	Unknow_char, 
	Const_overflow, 
	Charcon_too_much_char,
	Miss_single_quote,
	Miss_double_quote,
	Str_const_too_long,
	Miss_rbrace,
	Ident_multi_def,
	Miss_type,
	Miss_rbrack,
	Miss_array_size,
	Ident_undef,
	Miss_semicolon,
	Miss_lparent,
	Miss_condition,
	Miss_rparent,
	If_stat_miss_else,
	Case_stat_miss_colon,
	Case_stat_error,
	Default_error,
	Parameter_too_few,
	Parameter_too_much,
	Func_no_ret,
	Void_func_ret,
	Miss_main,
	Expect_ident,
	Miss_becomes,
	Const_Assign_error,
	Miss_lbrace,
	Condition_op_error,
	Intcon_preamble_zero,
	Assign_stat_error,
	Factor_error,
	Var_decl_error,
	Factor_void_func,
	Func_undef,
	Type_mismatch
};

extern ofstream error_stream;		// 错误流
extern vector<compile_err> errs;	// 存放当前行的错误
extern int ll;                      // 当前行号
extern int cc;						// 当前字符在行中的索引
extern int errpos;					// 错误位置
extern bool err_flag;				// 错误标志
string error_msg(int n);
void print_error();
void error(int n);
void error1(int n);
void skip(set<symbol> symset, int n);
void skip(symbol expected, int n);
bool test(symbol expected, symbol des, int n);
bool test(symbol expected, set<symbol> symset, int n);
bool test(set<symbol> expected_set, symbol des, int n);
bool test(set<symbol> set1, set<symbol> set2, int n);