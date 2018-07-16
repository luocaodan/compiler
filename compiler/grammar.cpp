#include "stdafx.h"
#include "grammar.h"

table sym_tab;                   // 符号表
map<string, string> str_tab;	 // string常量表
ofstream grammar_stream;		 // 语法分析输出流
bool grammar_flag = false;		 // 为true则打印语法分析结果
static int adr = 0;				 // 相对地址
static int num;					 // 读出的常量
static int ret = 0;				 // 检测函数是否有返回值
static string cur_var;			 // 当前生成的临时变量名
static type cur_type;			 // 当前生成的临时变量的类型
static string cur_label;		 // 当前生成的标签名
static string op;
static string num1;
static string num2;
static string res;

static set<symbol> typeset;
static set<symbol> const_set;
static set<symbol> beg_prog;
static set<symbol> beg_stat;

void init() {
	typeset.insert(intsy);
	typeset.insert(charsy);

	const_set.insert(intcon);
	const_set.insert(charcon);

	beg_prog.insert(constsy);
	beg_prog.insert(intsy);
	beg_prog.insert(charsy);
	beg_prog.insert(voidsy);

	beg_stat.insert(ifsy);
	beg_stat.insert(whilesy);
	beg_stat.insert(lbrace);
	beg_stat.insert(ident);
	beg_stat.insert(scanfsy);
	beg_stat.insert(printfsy);
	beg_stat.insert(switchsy);
	beg_stat.insert(returnsy);
	beg_stat.insert(semicolon);
	// 没错，语句的开始还有分号
}

/*
＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
*/

void program() {
	init();
	getsym();
	if(cur_sym.second == constsy) {
		const_decl(1);
	}
	if(cur_sym.second == intsy || cur_sym.second == charsy) {
		var_decl(1);
	}
	// 全局变量结束
	gen_code("end", "", "", "global");
	int flag = false;
	while(cur_sym.second == intsy || cur_sym.second == charsy || cur_sym.second == voidsy) {
		if(cur_sym.second == intsy || cur_sym.second == charsy) {
			func_def();
		}
		else {
			streamoff pre_index = fin.tellg();
			pair<string, symbol> pre_sym = cur_sym;
			getsym();
			if(cur_sym.second == mainsy) {
				flag = true;
				break;
			}
			else {
				reset(pre_index);
				cur_sym = pre_sym;
				proc_def();
			}
		}
	}
	if(!flag) {
		error1(Miss_main);
	}
	else {
		main_func();
	}
}

/*
＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
*/

void const_decl(int flag) {
	getsym();
	const_def(flag);
	if(!test(semicolon, beg_prog, Miss_semicolon)){
		return;
	}
	grammar_stream << "This is a const declaration" << endl;
	getsym();
	while(cur_sym.second == constsy) {
		const_decl(flag);
	}
}

/*
＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
*/

void var_decl(int flag) {
	set<symbol> end_decl;
	end_decl.insert(intsy);
	end_decl.insert(charsy);
	end_decl.insert(voidsy);

	while(cur_sym.second == charsy || cur_sym.second == intsy) {
		pair<string, symbol> pre_sym = cur_sym;
		streamoff pre_index = fin.tellg();
		getsym();
		getsym();
		if(cur_sym.second == semicolon || cur_sym.second == comma || cur_sym.second == lbrack) {
			reset(pre_index);
			cur_sym = pre_sym;
			var_def(flag);
			if(!test(semicolon, end_decl, Miss_semicolon)) {
				return;
			}
			getsym();
			grammar_stream << "This is a var declaration" << endl;
		}
		else if(cur_sym.second == lparent || cur_sym.second == lbrace) {
			reset(pre_index);
			cur_sym = pre_sym;
			break;
		}
		else {
			skip(end_decl, Var_decl_error);
		}
	}
}

/*
＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’ )}
*/

void var_def(int flag) {
	symbol tp = cur_sym.second;
	type t = (tp == intsy)? ints: chars;	
	set<symbol> end_def;
	end_def.insert(semicolon);
	end_def.insert(intsy);
	end_def.insert(charsy);
	end_def.insert(voidsy);

	do {
		getsym();
		if(!test(ident, end_def, Expect_ident)) {
			return;
		}

		string var_name = cur_sym.first;

		getsym();
		if(cur_sym.second == lbrack) {
			getsym();
			if(!test(intcon, end_def, Miss_array_size)) {
				return;
			}
			int kind = constant_liter(1);
			if(kind != 0) {
				skip(end_def, Miss_array_size);
				return;
			}
			getsym();
			int arr_len = num;
			if(!test(rbrack, end_def, Miss_rbrack)) {
				return;
			}
			if(sym_tab.find_var_local(var_name) != -1 && flag == 0) {
				skip(end_def, Ident_multi_def);
				return;
			}
			if(sym_tab.find_var(var_name) != -1 && flag == 1) {
				skip(end_def, Ident_multi_def);
				return;
			}
			getsym();
			adr += arr_len;

			// 插入符号表
			sym_tab.insert_array(var_name, t, adr, arr_len);

			// 生成四元式
			op = "array";
			num1 = (tp == intsy)? "int": "char";
			num2 = to_string((long long)arr_len);
			res = var_name;
			gen_code(op, num1, num2, res);

			grammar_stream << "This is a var array definition" << endl;
		}
		else {
			if(sym_tab.find_var_local(var_name) != -1 && flag == 0) {
				error1(Ident_multi_def);
				return;
			}

			if(sym_tab.find_var(var_name) != -1 && flag == 1) {
				error1(Ident_multi_def);
				return;
			}

			adr++;

			// 插入符号表
			sym_tab.insert_var(var_name, t, adr);

			// 生成四元式
			op = "var";
			num1 = (tp == intsy)? "int": "char";
			num2 = "";
			res = var_name;
			gen_code(op, num1, num2, res);

			grammar_stream << "This is a var definition" << endl;
		}
	}
	while(cur_sym.second == comma);
}
/*
＜主函数＞    ::= void main‘(’‘)’‘{’＜复合语句＞‘}’
*/

void main_func() {
	// main 函数插入符号表
	adr = 0;
	sym_tab.insert_func("main", voids, adr);

	// 生成四元式
	op = "func";
	num1 = "void";
	num2 = "";
	res = "main";
	gen_code(op, num1, num2, res);

	getsym();
	if(!test(lparent, rbrace, Miss_lparent)) {
		return;
	}
	getsym();
	if(!test(rparent, rbrace, Miss_rparent)) {
		return;
	}
	getsym();
	if(!test(lbrace, rbrace, Miss_lbrace)) {
		return;
	}
	getsym();
	compound_stat();
	if(cur_sym.second != rbrace) {
		error1(Miss_rbrace);
		return;
	}
	if(ret == 1) {
		error1(Void_func_ret);
	}
	ret = 0;
	
	// 生成四元式 程序结束
	op = "exit";
	num1 = "";
	num2 = "";
	res = "main";
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a main function" << endl;
}

/*
＜无返回值函数定义＞  ::= void＜标识符＞(’＜参数＞‘)’‘{’＜复合语句＞‘}’| void＜标识符＞{’＜复合语句＞‘}’
*/

void proc_def() {
	set<symbol> beg_set;
	beg_set.insert(voidsy);
	beg_set.insert(charsy);
	beg_set.insert(intsy);

	getsym();
	if(!test(ident, rbrace, Expect_ident)) {
		return;
	}
	// 插入符号表
	string proc_name = cur_sym.first;
	adr = 0;
	sym_tab.insert_func(proc_name, voids, adr);

	// 生成四元式
	op = "func";
	num1 = "void";
	num2 = "";
	res = proc_name;
	gen_code(op, num1, num2, res);

	getsym();
	if(cur_sym.second == lparent) {
		parameters();
		if(!test(rparent, beg_set, Miss_rparent)){
			return;
		}
		getsym();
	}
	if(!test(lbrace, beg_set, Miss_lbrace)) {
		return;
	}
	getsym();
	compound_stat();
	
	if(!test(rbrace, beg_set, Miss_rbrace)){
		return;
	}
	getsym();
	if(ret == 1) {
		error1(Void_func_ret);
	}
	ret = 0;

	// 函数定义结束
	op = "end";
	num1 = "";
	num2 = "";
	res = proc_name;
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a function with no return value" << endl;
}

/*
＜声明头部＞‘(’＜参数＞‘)’ ‘{’＜复合语句＞‘}’|＜声明头部＞‘{’＜复合语句＞‘}’
*/

void func_def() {
	set<symbol> beg_set;
	beg_set.insert(voidsy);
	beg_set.insert(charsy);
	beg_set.insert(intsy);

	symbol ret_type = cur_sym.second;
	string func_name = decl_head();
	
	// 函数定义开始
	op = "func";
	num1 = (ret_type == intsy)? "int": "char";
	num2 = "";
	res = func_name;
	gen_code(op, num1, num2, res);

	if(cur_sym.second == lparent) {
		parameters();
		if(!test(rparent, beg_set, Miss_rparent)){
			return;
		}
		getsym();
	}
	if(!test(lbrace, beg_set, Miss_lbrace)) {
		return;
	}
	getsym();
	compound_stat();
	
	if(!test(rbrace, beg_set, Miss_rbrace)){
		return;
	}
	getsym();
	if(ret == 0) {
		error1(Func_no_ret);
	}
	ret = 0;

	// 函数定义结束
	op = "end";
	num1 = "";
	num2 = "";
	res = func_name;
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a function with return value" << endl;
}

/*
＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
*/

string decl_head() {
	string func_name = "";
	symbol ret_type = cur_sym.second;
	type t = (ret_type == intsy)? ints: chars;

	getsym();
	if(cur_sym.second != ident) {
		error1(Expect_ident);
		getsym();
	}
	else {
		func_name = cur_sym.first;
		// 插入符号表
		adr = 0;
		sym_tab.insert_func(func_name, t, adr);

		// 生成四元式

		getsym();
		grammar_stream << "This is a declare head" << endl;
	}
	return func_name;
}

/*
＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
                               | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
*/

void const_def(int flag) {
	set<symbol> end_def = beg_prog;
	end_def.insert(semicolon);
	if(!test(typeset, end_def, Miss_type)) {
		return;
	}

	symbol tp = cur_sym.second;
	type t = (tp == intsy)? ints: chars;

	do {
		getsym();
		if(!test(ident, end_def, Expect_ident)) {
			return;
		}
		string const_name = cur_sym.first;
		getsym();
		if(!test(becomes, end_def, Miss_becomes)) {
			return;
		}
		getsym();
		int sign = 1;
		bool has_sign = false;
		if(cur_sym.second == add || cur_sym.second == sub) {
			has_sign = true;
			if(cur_sym.second == sub) {
				sign = -1;
			}
			getsym();
		}
		if(!test(const_set, end_def, Const_Assign_error)) {
			return;
		}
		int kind = constant_liter(sign);
		if(kind == 1 && has_sign) {
			skip(end_def, Const_Assign_error);
			return;
		}

		if(sym_tab.find_var_local(const_name) != -1 && flag == 0) {
			error1(Ident_multi_def);
			getsym();
			return;
		}

		if(sym_tab.find_var(const_name) != -1 && flag == 1) {
			error1(Ident_multi_def);
			getsym();
			return;
		}

		// 插入符号表
		adr++;
		sym_tab.insert_const(const_name, t, sign*num);

		/*
		常量直接替换为值，不生成四元式
		op = "const";
		num1 = (tp == intsy)? "int": "char";
		num2 = to_string((long long)num);
		res = const_name;
		gen_code(op, num1, num2, res);
		*/

		grammar_stream << "This is a const definition" << endl;
		getsym();
	}
	while(cur_sym.second == comma);
}

int constant_liter(int sign) {
	long long inum = 0;
	if(cur_sym.second == intcon) {
		for(size_t i=0;i < cur_sym.first.size();i++) {
			inum = inum*10 + cur_sym.first[i] - '0';
			if(inum*sign > INT_MAX || inum*sign < INT_MIN) {
				num = 0;
				error1(Const_overflow);
				return 0;
			}
		}
		if(cur_sym.first[0] == '0' && cur_sym.first.size() > 1) {
			num = (int)inum;
			error1(Intcon_preamble_zero);
			return 0;
		}
		num = (int)inum;
		return 0;
	}
	else {
		num = cur_sym.first[0];
		return 1;
	}
}

/*
＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
*/

void parameters() {
	int count = 0;          // 参数个数

	do {
		getsym();
		if(!test(typeset, rparent, Miss_type)) {
			return;
		}

		symbol tp = cur_sym.second;
		type t = (tp == intsy)? ints: chars;

		getsym();
		if(!test(ident, rparent, Expect_ident)) {
			return;
		}
		// 参数信息插入符号表
		string para_name = cur_sym.first;
		adr++;
		sym_tab.insert_para(para_name, t, adr);

		// 生成四元式 
		op = "para";
		num1 = (tp==intsy)? "int": "char";
		num2 = "";
		res = para_name;
		gen_code(op, num1, num2, res);

		getsym();
		count++;
	}
	while(cur_sym.second == comma);
	
	// 反填参数个数
	int cur_func_index = sym_tab.block.back() -  1;
	sym_tab.tab[cur_func_index].size = count;
}

/*
＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
*/

void compound_stat() {
	if(cur_sym.second == constsy) {
		const_decl(0);
	}
	if(typeset.find(cur_sym.second) != typeset.end()) {
		var_decl(0);
	}
	stat_column();
}

/*
＜语句列＞   ::= ｛＜语句＞｝
*/

void stat_column() {
	while(beg_stat.find(cur_sym.second) != beg_stat.end()) {
		statements();
	}
}

/*
＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞; 
				|＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;
				|＜情况语句＞｜＜返回语句＞;
*/

void statements() {
	if(cur_sym.second == ifsy) {
		if_stat();
	}
	else if(cur_sym.second == whilesy) {
		while_stat();
	}
	else if(cur_sym.second == lbrace) {
		getsym();
		stat_column();
		if(!test(rbrace, beg_stat, Miss_rbrace)) {
			return;
		}
		getsym();
	}
	else if(cur_sym.second == scanfsy) {
		scanf_stat();
		if(!test(semicolon, beg_stat, Miss_semicolon)) {
			return;
		}
		getsym();
	}
	else if(cur_sym.second == printfsy) {
		printf_stat();
		if(!test(semicolon, beg_stat, Miss_semicolon)) {
			return;
		}
		getsym();
	}
	else if(cur_sym.second == switchsy) {
		case_stat();
	}
	else if(cur_sym.second == returnsy) {
		return_stat();
		if(!test(semicolon, beg_stat, Miss_semicolon)) {
			return;
		}
		getsym();
	}
	else if(cur_sym.second == ident) {
		streamoff pre_index = fin.tellg();
		pair<string, symbol> pre_sym = cur_sym;
		getsym();
		if(cur_sym.second == becomes || cur_sym.second == lbrack) {
			reset(pre_index);
			cur_sym = pre_sym;
			assign_stat();
			if(!test(semicolon, beg_stat, Miss_semicolon)) {
				return;
			}
			getsym();
		}
		else {	
			reset(pre_index);
			cur_sym = pre_sym;
			call(false);
			if(!test(semicolon, beg_stat, Miss_semicolon)) {
			getsym();
				return;
			}
			getsym();
		}
	}
	else if(cur_sym.second == semicolon) {
		// <空>;
		getsym();
	}
	else {

	}
}

/*
＜条件语句＞::= if ‘(’＜条件＞‘)’＜语句＞else＜语句＞
*/

void if_stat() {
	string label_else;
	string label_end;

	getsym();
	if(test(lparent, beg_stat, Miss_lparent)) {
		getsym();
		if(cur_sym.second == rparent) {
			skip(beg_stat, Miss_condition);
		}
		else {
			label_else = condition();	
			if(test(rparent, beg_stat, Miss_rparent)) {
				getsym();
			}
		}
	}
	statements();
	if(cur_sym.second != elsesy) {
		error1(If_stat_miss_else);
		return;
	}
	else {	
		// 跳过else，到语句结束
		label_end = gen_label();
		op = "jmp";
		num1 = "";
		num2 = "";
		res = label_end;
		gen_code(op, num1, num2, res);

		// else标签
		op = "label";
		num1 = "";
		num2 = "";
		res = label_else;
		gen_code(op, num1, num2, res);

		getsym();
		statements();
	}

	// if语句结束标签
	op = "label";
	num1 = "";
	num2 = "";
	res = label_end;
	gen_code(op, num1, num2, res);
	grammar_stream << "This is a if statement" << endl;
}

/*
＜循环语句＞   ::=  while ‘(’＜条件＞‘)’＜语句＞
*/

void while_stat() {
	string while_begin = gen_label();
	string while_end;

	// while 开始
	op = "label";
	num1 = "while_beg";
	num2 = "";
	res = while_begin;
	gen_code(op, num1, num2, res);

	getsym();
	if(test(lparent, beg_stat, Miss_lparent)) {
		getsym();
		while_end = condition();	
		if(test(rparent, beg_stat, Miss_rparent)) {
			getsym();
		}
	}
	statements();
	
	// 语句执行结束调到while开始标签
	op = "jmp";
	num1 = "";
	num2 = "";
	res = while_begin;
	gen_code(op, num1, num2, res);

	// while结束
	op = "label";
	num1 = "while_end";
	num2 = "";
	res = while_end;
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a while statement" << endl;
}

/*
＜条件＞ ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ 
*/

string condition() {
	// 不满足条件的跳转标签
	string condition_false = gen_label(); 

	// 表达式子程序生成cur_var
	expression();
	string temp_num1 = cur_var;
	if(cur_sym.second == eql) {
		getsym();
		expression();
		num2 = cur_var;

		// 生成中间代码
		op = "bne";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == neq) {
		getsym();
		expression();
		num2 = cur_var;

		// 生成中间代码
		op = "beq";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == gtr) {
		getsym();
		expression();
		num2 = cur_var;

		// 生成中间代码
		op = "ble";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == geq) {
		getsym();
		expression();
		num2 = cur_var;

		// 生成中间代码
		op = "blt";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == lss) {
		getsym();
		expression();
		num2 = cur_var;

		// 生成中间代码
		op = "bge";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == leq) {
		getsym();
		expression();
		num2 = cur_var;

		// 生成中间代码
		op = "bgt";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == rparent) {
		num2 = "0";

		// 生成中间代码
		op = "beq";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else {
		skip(rparent, Condition_op_error);
	}
	return condition_false;
}

/*
＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
*/

void scanf_stat() {
	getsym();
	if(!test(lparent, beg_stat, Miss_lparent)) {
		return;
	}
	do {
		getsym();
		if(!test(ident, beg_stat, Expect_ident)) {
			return;
		}
		string var_name = cur_sym.first;
		int index = sym_tab.find_var(var_name);
		if(index == -1) {
			skip(beg_stat, Ident_undef);
			return;
		}
		type tp = sym_tab.tab[index].t;

		// 生成中间代码
		op = "scanf";
		num1 = "";
		num2 = (tp == ints)? "int": "char";
		res = var_name;
		gen_code(op, num1, num2, res);

		getsym();
	}
	while(cur_sym.second == comma);
	if(!test(rparent, beg_stat, Miss_rparent)) {
		return;
	}
	getsym();
	grammar_stream << "This is a scanf statement" << endl;
}

/*
＜写语句＞    ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
*/

void printf_stat() {
	static int str_no = 0;			// string常量数
	string temp_op = "printf";
	string temp_num1;               // 字符串在字符串常量表中的键
	string str_value;				// string常量值

	getsym();
	if(!test(lparent, beg_stat, Miss_lparent)) {
		return;
	}
	getsym();
	if(cur_sym.second == stringcon) {
		str_value = cur_sym.first;
		str_no++;
		temp_num1 = "$str_" + to_string((long long)str_no); 
		str_tab.insert(make_pair(temp_num1, str_value));

		getsym();
		if(cur_sym.second == comma) {
			getsym();
			expression();
			if(!test(rparent, beg_stat, Miss_rparent)) {
				return;
			}

			num2 = cur_var;
			res = (cur_type == ints)? "int": "char";
			gen_code(temp_op, temp_num1, num2, res);

			getsym();
		}
		else if(cur_sym.second == rparent) {
			num2 = "";
			res = "";
			gen_code(temp_op, temp_num1, num2, res); 
			getsym();
		}
		else {
			skip(beg_stat, Miss_rparent);
		}
	}
	else {
		expression();
		if(!test(rparent, beg_stat, Miss_rparent)) {
			return;
		}

		num1 = "";
		num2 = cur_var;
		res = (cur_type == ints)? "int": "char";
		gen_code(temp_op, num1, num2, res);

		getsym();
	}
}

/*
＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞[＜缺省＞] ‘}’
*/

void case_stat() {
	string switch_end = gen_label();

	getsym();
	if(!test(lparent, rbrace, Miss_lparent)) {
		return;
	}
	getsym();
	expression();

	if(!test(rparent, rbrace, Miss_rparent)) {
		return;
	}
	getsym();
	if(!test(lbrace, rbrace, Miss_lbrace)) {
		return;
	}
	getsym();
	case_label(cur_var, switch_end);
	if(cur_sym.second == defaultsy) {
		default_stat(switch_end);
	}
	if(!test(rbrace, beg_stat, Miss_rbrace)) {
		return;
	}

	// switch语句结束
	op = "label";
	num1 = "";
	num2 = "";
	res = switch_end;
	gen_code(op, num1, num2, res);

	getsym();
	grammar_stream << "This is a switch statement" << endl;
}

/*
＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
*/

void case_label(string expr_var, string switch_end) {
	while(cur_sym.second == casesy) {
		one_case(expr_var, switch_end);
	}
}

/*
＜情况子语句＞  ::=  case＜常量＞：＜语句＞
*/

void one_case(string expr_var, string switch_end) {
	string one_case_end = gen_label();
	getsym();
	int sign = 1;
	bool flag = false;
	if(cur_sym.second == add || cur_sym.second == sub) {
		flag = true;
		if(cur_sym.second == sub) {
			sign = -1;
		}
		getsym();
	}
	if(cur_sym.second == charcon) {
		if(flag) {
			skip(beg_stat, Case_stat_error);
		}
		int kind = constant_liter(1);
		if(kind != 1) {
			skip(beg_stat, Case_stat_error);
		}
		else {
			op = "bne";
			num1 = expr_var;
			num2 = to_string((long long)num);
			res = one_case_end;
			gen_code(op, num1, num2, res);

			getsym();
			if(test(colon, beg_stat, Case_stat_miss_colon)) {
				getsym();
			}
		}
	}
	else if(test(intcon, beg_stat, Case_stat_error)) {
		int kind = constant_liter(sign);
		if(kind == -1 || kind == 1 && flag) {
			skip(beg_stat, Case_stat_error);
		}
		else {
			// 比较表达式和常量值，不相等则跳转到下一个case
			op = "bne";
			num1 = expr_var;
			num2 = to_string((long long)(num * sign));
			res = one_case_end;
			gen_code(op, num1, num2, res);

			getsym();
			if(test(colon, beg_stat, Case_stat_miss_colon)) {
				getsym();
			}
		}
	}
	statements();

	// 执行语句后跳转到switch语句结束
	op = "jmp";
	num1 = "";
	num2 = "";
	res = switch_end;
	gen_code(op, num1, num2, res);

	// one case 结束标签
	op = "label";
	num1 = "";
	num2 = "";
	res = one_case_end;
	gen_code(op, num1, num2, res);
}

/*
＜缺省＞   ::=  default : ＜语句＞
*/

void default_stat(string switch_end) {
	getsym();
	if(test(colon, beg_stat, Default_error)) {
		getsym();
	}
	statements();

	// 跳转到switch语句结束
	op = "jmp";
	num1 = "";
	num2 = "";
	res = switch_end;
	gen_code(op, num1, num2, res);
}

/*
＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
*/

void return_stat() {
	getsym();
	if(cur_sym.second == lparent) {
		getsym();
		expression();
		if(!test(rparent, beg_stat, Miss_rparent)) {
			return;
		}
		getsym();
		ret = 1;
		// 有返回值
		op = "return";
		num1 = "";
		num2 = "";
		res = cur_var;
		gen_code(op, num1, num2, res);
	}
	else {
		// 无返回值
		op = "return";
		num1 = "";
		num2 = "";
		res = "";
		gen_code(op, num1, num2, res);
	}
	grammar_stream << "This is a return statement" << endl;
}
/*
<调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’|<标识符> 
*/

string call(bool factor_flag) {
	set<symbol> end_call = beg_stat;

	string func_name = cur_sym.first;
	int index = sym_tab.find_func(func_name);

	if(index == -1) {
		skip(end_call, Func_undef);
		return "";
	}
	
	getsym();
	if(cur_sym.second == lparent) {
		value_para(func_name);
		set<symbol> end_stat = beg_stat;
		if(test(rparent, end_stat, Miss_rparent)) {
			getsym();
		}
	}
	
	// 生成中间代码 语句调用和表达式中的调用
	op = "call";
	num1 = func_name;
	num2 = "";
	if(factor_flag) {
		// 表达式中的调用，生成临时变量
		res = gen_temp_var();
		gen_code(op, num1, num2, res);
	}
	else {
		// 语句函数调用
		res = "";
		gen_code(op, num1, num2, res);
	}

	grammar_stream << "This is a function call statement" << endl;
	return res;
}

/*
＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
*/

void assign_stat() {
	string var_name = cur_sym.first;
	int index = sym_tab.find_var(var_name);
	if(index == -1) {
		skip(beg_stat, Ident_undef);
		return;
	}
	
	type var_type = sym_tab.tab[index].t;	
	// 可能是数组下标
	string temp_num2 = "";

	getsym();	
	if(cur_sym.second == lbrack) {
		getsym();
		expression();
		if(!test(rbrack, beg_stat, Miss_rbrack)) {
			return;
		}
		getsym();

		// 数组下标
		temp_num2 = cur_var;
	}
	if(!test(becomes, beg_stat, Assign_stat_error)) {
		return;
	}
	getsym();
	expression();

	// 右值
	num1 = cur_var;
	if(temp_num2 != "") {   // 变量赋值给数组
		gen_code("assign_to_array", num1, temp_num2, var_name);
	}
	else{					// 普通赋值
		gen_code("assign", num1, temp_num2, var_name);
	}

	// 类型不匹配
	if(cur_type != var_type) {
		error1(Type_mismatch);
	}
	grammar_stream << "This is a assign statement" << endl;
}

/*
＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}
*/

void value_para(string func_name) {
	int count = 0;
	vector<string> para_tab;
	do {
		getsym();
		expression();
		para_tab.push_back(cur_var);
		count++;
	}
	while(cur_sym.second == comma);

	size_t len = para_tab.size();
	for(size_t i = 0;i < len;i++) {
		gen_code("valpara", "", "", para_tab[i]);
	}
	int func_index = sym_tab.find_func(func_name);
	int para_size = sym_tab.tab[func_index].size;
	if(count < para_size) {
		error1(Parameter_too_few);
	}
	else if(count > para_size) {
		error1(Parameter_too_much);
	}
}

/*
＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
*/

void expression() {
	string term1;
	string temp_op;
	if(cur_sym.second == add || cur_sym.second == sub) {
		// 表达式计算结果为整形
		if(cur_sym.second == add) {
			getsym();
			term();
			// +a
			term1 = cur_var;
		}
		else {
			getsym();
			term();
			// -a
			term1 = gen_temp_var();
			op = "sub";
			num1 = "0";
			num2 = cur_var;
			res = term1;
			gen_code(op, num1, num2, res);
		}
		while(cur_sym.second == add || cur_sym.second == sub) {
			if(cur_sym.second == add) {
				// ... + a
				temp_op = "add";
			}
			else {
				// ... - a
				temp_op = "sub";
			}
			getsym();
			term();

			num1 = term1;
			num2 = cur_var;
			term1 = gen_temp_var();
			res = term1;
			gen_code(temp_op, num1, num2, res);
		}
		// 表达式计算完成
		cur_var = term1;
		cur_type = ints;
	}
	else {
		term();
		term1 = cur_var;
		while(cur_sym.second == add || cur_sym.second == sub) {
			if(cur_sym.second == add) {
				temp_op = "add";
			}
			else {
				temp_op = "sub";
			}
			getsym();
			term();

			cur_type = ints;
			
			// 生成中间代码
			num1 = term1;
			num2 = cur_var;
			term1 = gen_temp_var();
			res = term1;
			gen_code(temp_op, num1, num2, res);

		}

		// 表达式计算完成
		cur_var = term1;
	}
	grammar_stream << "This is a expression" << endl;
}

/*
＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
*/

void term() {
	factor();
	string fac1 = cur_var;
	string temp_op;
	while(cur_sym.second == times || cur_sym.second == idiv) {
		if(cur_sym.second == times) {
			temp_op = "mul";
		}
		else {
			temp_op = "div";
		}
		getsym();
		factor();
		cur_type = ints;

		// 生成中间代码
		num1 = fac1;
		num2 = cur_var;
		fac1 = gen_temp_var();
		res = fac1;
		gen_code(temp_op, num1, num2, res);
	}
	cur_var = fac1;
}

/*
＜因子＞    ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’|‘(’＜表达式＞‘)’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞   
*/

void factor() {
	set<symbol> end_factor = beg_stat;

	if(cur_sym.second == ident) {
		string temp_name = cur_sym.first;

		streamoff pre_index = fin.tellg();
		pair<string, symbol> pre_sym = cur_sym;
		getsym();
		if(cur_sym.second == lbrack) {   // 数组
			getsym();
			expression();
			if(cur_sym.second != rbrack) {
				error1(Miss_rbrack);
				return;
			}
			getsym();

			int index = sym_tab.find_var(temp_name);
			if(index == -1) {
				error1(Ident_undef);
				return;
			}

			// 数组中取值赋给临时变量
			op = "assign_to_var";
			num1 = temp_name;
			num2 = cur_var;
			cur_var = gen_temp_var();
			res = cur_var;
			gen_code(op, num1, num2, res);

			cur_type = sym_tab.tab[index].t;
		}
		else {
			// 查符号表判断是函数还是变量
			int index = sym_tab.find_func(temp_name);

			// 函数调用
			if(index != -1) {
				type tp = sym_tab.tab[index].t;
				if(tp != voids) {
					reset(pre_index);
					cur_sym = pre_sym;
					cur_var = call(true);            // 函数调用返回的临时变量（有返回值的）
					cur_type = tp;
				}
				else {
					error1(Factor_void_func);
					return;
				}
			}
			// 变量
			else {
				index = sym_tab.find_var(temp_name);
				if(index != -1) {
					kind k = sym_tab.tab[index].k;
					// 常量
					if(k == consts) {
						cur_var = to_string((long long)sym_tab.tab[index].adr);  // 常量直接为值
						cur_type = sym_tab.tab[index].t;
					}
					// 变量
					else {
						cur_var = temp_name;
						cur_type = sym_tab.tab[index].t;
					}
				}
				// 未在符号表中找到
				else {
					error1(Ident_undef);
					return;
				}
			}
		}
	}
	else if(cur_sym.second == lparent) {
		getsym();
		expression();
		if(cur_sym.second != rparent) {
			error1(Miss_rparent);
			return;
		}
		getsym();
	}
	else if(cur_sym.second == add || cur_sym.second == sub) {
		cur_type = ints;
		int sign = 1;
		if(cur_sym.second == sub)
			sign = -1;
		getsym();
		int kind = constant_liter(sign);
		if(kind != 0) {   // char常量或出错
			error1(Factor_error);
			getsym();
			return;
		}
		cur_var = to_string((long long)(sign * num));
		getsym();
	}
	else if(cur_sym.second == intcon) {
		int kind = constant_liter(1);
		if(kind == 0) {
			cur_type = ints;
			cur_var = to_string((long long)num);
		}
		getsym();
	}
	else if(cur_sym.second == charcon) {
		int kind = constant_liter(1);
		if(kind = 1) {
			cur_type = chars;
			cur_var = to_string((long long)num);
		}
		getsym();
	}
	else {
		error1(Factor_error);
	}
}