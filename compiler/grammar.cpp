#include "stdafx.h"
#include "grammar.h"

table sym_tab;                   // ���ű�
map<string, string> str_tab;	 // string������
ofstream grammar_stream;		 // �﷨���������
bool grammar_flag = false;		 // Ϊtrue���ӡ�﷨�������
static int adr = 0;				 // ��Ե�ַ
static int num;					 // �����ĳ���
static int ret = 0;				 // ��⺯���Ƿ��з���ֵ
static string cur_var;			 // ��ǰ���ɵ���ʱ������
static type cur_type;			 // ��ǰ���ɵ���ʱ����������
static string cur_label;		 // ��ǰ���ɵı�ǩ��
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
	// û�����Ŀ�ʼ���зֺ�
}

/*
������    ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������
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
	// ȫ�ֱ�������
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
������˵���� ::=  const���������壾;{ const���������壾;}
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
������˵����  ::= ���������壾;{���������壾;}
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
���������壾  ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]�� )}
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

			// ������ű�
			sym_tab.insert_array(var_name, t, adr, arr_len);

			// ������Ԫʽ
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

			// ������ű�
			sym_tab.insert_var(var_name, t, adr);

			// ������Ԫʽ
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
����������    ::= void main��(����)����{����������䣾��}��
*/

void main_func() {
	// main ����������ű�
	adr = 0;
	sym_tab.insert_func("main", voids, adr);

	// ������Ԫʽ
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
	
	// ������Ԫʽ �������
	op = "exit";
	num1 = "";
	num2 = "";
	res = "main";
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a main function" << endl;
}

/*
���޷���ֵ�������壾  ::= void����ʶ����(������������)����{����������䣾��}��| void����ʶ����{����������䣾��}��
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
	// ������ű�
	string proc_name = cur_sym.first;
	adr = 0;
	sym_tab.insert_func(proc_name, voids, adr);

	// ������Ԫʽ
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

	// �����������
	op = "end";
	num1 = "";
	num2 = "";
	res = proc_name;
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a function with no return value" << endl;
}

/*
������ͷ������(������������)�� ��{����������䣾��}��|������ͷ������{����������䣾��}��
*/

void func_def() {
	set<symbol> beg_set;
	beg_set.insert(voidsy);
	beg_set.insert(charsy);
	beg_set.insert(intsy);

	symbol ret_type = cur_sym.second;
	string func_name = decl_head();
	
	// �������忪ʼ
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

	// �����������
	op = "end";
	num1 = "";
	num2 = "";
	res = func_name;
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a function with return value" << endl;
}

/*
������ͷ����   ::=  int����ʶ���� |char����ʶ����
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
		// ������ű�
		adr = 0;
		sym_tab.insert_func(func_name, t, adr);

		// ������Ԫʽ

		getsym();
		grammar_stream << "This is a declare head" << endl;
	}
	return func_name;
}

/*
���������壾   ::=   int����ʶ��������������{,����ʶ��������������}
                               | char����ʶ���������ַ���{,����ʶ���������ַ���}
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

		// ������ű�
		adr++;
		sym_tab.insert_const(const_name, t, sign*num);

		/*
		����ֱ���滻Ϊֵ����������Ԫʽ
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
��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
*/

void parameters() {
	int count = 0;          // ��������

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
		// ������Ϣ������ű�
		string para_name = cur_sym.first;
		adr++;
		sym_tab.insert_para(para_name, t, adr);

		// ������Ԫʽ 
		op = "para";
		num1 = (tp==intsy)? "int": "char";
		num2 = "";
		res = para_name;
		gen_code(op, num1, num2, res);

		getsym();
		count++;
	}
	while(cur_sym.second == comma);
	
	// �����������
	int cur_func_index = sym_tab.block.back() -  1;
	sym_tab.tab[cur_func_index].size = count;
}

/*
��������䣾   ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
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
������У�   ::= ������䣾��
*/

void stat_column() {
	while(beg_stat.find(cur_sym.second) != beg_stat.end()) {
		statements();
	}
}

/*
����䣾    ::= ��������䣾����ѭ����䣾| ��{��������У���}�������з���ֵ����������䣾; 
				|���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;
				|�������䣾����������䣾;
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
		// <��>;
		getsym();
	}
	else {

	}
}

/*
��������䣾::= if ��(������������)������䣾else����䣾
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
		// ����else����������
		label_end = gen_label();
		op = "jmp";
		num1 = "";
		num2 = "";
		res = label_end;
		gen_code(op, num1, num2, res);

		// else��ǩ
		op = "label";
		num1 = "";
		num2 = "";
		res = label_else;
		gen_code(op, num1, num2, res);

		getsym();
		statements();
	}

	// if��������ǩ
	op = "label";
	num1 = "";
	num2 = "";
	res = label_end;
	gen_code(op, num1, num2, res);
	grammar_stream << "This is a if statement" << endl;
}

/*
��ѭ����䣾   ::=  while ��(������������)������䣾
*/

void while_stat() {
	string while_begin = gen_label();
	string while_end;

	// while ��ʼ
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
	
	// ���ִ�н�������while��ʼ��ǩ
	op = "jmp";
	num1 = "";
	num2 = "";
	res = while_begin;
	gen_code(op, num1, num2, res);

	// while����
	op = "label";
	num1 = "while_end";
	num2 = "";
	res = while_end;
	gen_code(op, num1, num2, res);

	grammar_stream << "This is a while statement" << endl;
}

/*
�������� ::=  �����ʽ������ϵ������������ʽ���������ʽ�� 
*/

string condition() {
	// ��������������ת��ǩ
	string condition_false = gen_label(); 

	// ���ʽ�ӳ�������cur_var
	expression();
	string temp_num1 = cur_var;
	if(cur_sym.second == eql) {
		getsym();
		expression();
		num2 = cur_var;

		// �����м����
		op = "bne";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == neq) {
		getsym();
		expression();
		num2 = cur_var;

		// �����м����
		op = "beq";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == gtr) {
		getsym();
		expression();
		num2 = cur_var;

		// �����м����
		op = "ble";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == geq) {
		getsym();
		expression();
		num2 = cur_var;

		// �����м����
		op = "blt";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == lss) {
		getsym();
		expression();
		num2 = cur_var;

		// �����м����
		op = "bge";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == leq) {
		getsym();
		expression();
		num2 = cur_var;

		// �����м����
		op = "bgt";
		res = condition_false;
		gen_code(op, temp_num1, num2, res);
	}
	else if(cur_sym.second == rparent) {
		num2 = "0";

		// �����м����
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
������䣾    ::=  scanf ��(������ʶ����{,����ʶ����}��)��
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

		// �����м����
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
��д��䣾    ::= printf ��(�� ���ַ�����,�����ʽ�� ��)��| printf ��(�����ַ����� ��)��| printf ��(�������ʽ����)��
*/

void printf_stat() {
	static int str_no = 0;			// string������
	string temp_op = "printf";
	string temp_num1;               // �ַ������ַ����������еļ�
	string str_value;				// string����ֵ

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
�������䣾  ::=  switch ��(�������ʽ����)�� ��{���������[��ȱʡ��] ��}��
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

	// switch������
	op = "label";
	num1 = "";
	num2 = "";
	res = switch_end;
	gen_code(op, num1, num2, res);

	getsym();
	grammar_stream << "This is a switch statement" << endl;
}

/*
�������   ::=  ���������䣾{���������䣾}
*/

void case_label(string expr_var, string switch_end) {
	while(cur_sym.second == casesy) {
		one_case(expr_var, switch_end);
	}
}

/*
���������䣾  ::=  case��������������䣾
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
			// �Ƚϱ��ʽ�ͳ���ֵ�����������ת����һ��case
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

	// ִ��������ת��switch������
	op = "jmp";
	num1 = "";
	num2 = "";
	res = switch_end;
	gen_code(op, num1, num2, res);

	// one case ������ǩ
	op = "label";
	num1 = "";
	num2 = "";
	res = one_case_end;
	gen_code(op, num1, num2, res);
}

/*
��ȱʡ��   ::=  default : ����䣾
*/

void default_stat(string switch_end) {
	getsym();
	if(test(colon, beg_stat, Default_error)) {
		getsym();
	}
	statements();

	// ��ת��switch������
	op = "jmp";
	num1 = "";
	num2 = "";
	res = switch_end;
	gen_code(op, num1, num2, res);
}

/*
��������䣾   ::=  return[��(�������ʽ����)��]
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
		// �з���ֵ
		op = "return";
		num1 = "";
		num2 = "";
		res = cur_var;
		gen_code(op, num1, num2, res);
	}
	else {
		// �޷���ֵ
		op = "return";
		num1 = "";
		num2 = "";
		res = "";
		gen_code(op, num1, num2, res);
	}
	grammar_stream << "This is a return statement" << endl;
}
/*
<������䣾 ::= ����ʶ������(����ֵ��������)��|<��ʶ��> 
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
	
	// �����м���� �����úͱ��ʽ�еĵ���
	op = "call";
	num1 = func_name;
	num2 = "";
	if(factor_flag) {
		// ���ʽ�еĵ��ã�������ʱ����
		res = gen_temp_var();
		gen_code(op, num1, num2, res);
	}
	else {
		// ��亯������
		res = "";
		gen_code(op, num1, num2, res);
	}

	grammar_stream << "This is a function call statement" << endl;
	return res;
}

/*
����ֵ��䣾   ::=  ����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
*/

void assign_stat() {
	string var_name = cur_sym.first;
	int index = sym_tab.find_var(var_name);
	if(index == -1) {
		skip(beg_stat, Ident_undef);
		return;
	}
	
	type var_type = sym_tab.tab[index].t;	
	// �����������±�
	string temp_num2 = "";

	getsym();	
	if(cur_sym.second == lbrack) {
		getsym();
		expression();
		if(!test(rbrack, beg_stat, Miss_rbrack)) {
			return;
		}
		getsym();

		// �����±�
		temp_num2 = cur_var;
	}
	if(!test(becomes, beg_stat, Assign_stat_error)) {
		return;
	}
	getsym();
	expression();

	// ��ֵ
	num1 = cur_var;
	if(temp_num2 != "") {   // ������ֵ������
		gen_code("assign_to_array", num1, temp_num2, var_name);
	}
	else{					// ��ͨ��ֵ
		gen_code("assign", num1, temp_num2, var_name);
	}

	// ���Ͳ�ƥ��
	if(cur_type != var_type) {
		error1(Type_mismatch);
	}
	grammar_stream << "This is a assign statement" << endl;
}

/*
��ֵ������   ::= �����ʽ��{,�����ʽ��}
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
�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
*/

void expression() {
	string term1;
	string temp_op;
	if(cur_sym.second == add || cur_sym.second == sub) {
		// ���ʽ������Ϊ����
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
		// ���ʽ�������
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
			
			// �����м����
			num1 = term1;
			num2 = cur_var;
			term1 = gen_temp_var();
			res = term1;
			gen_code(temp_op, num1, num2, res);

		}

		// ���ʽ�������
		cur_var = term1;
	}
	grammar_stream << "This is a expression" << endl;
}

/*
���     ::= �����ӣ�{���˷�������������ӣ�}
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

		// �����м����
		num1 = fac1;
		num2 = cur_var;
		fac1 = gen_temp_var();
		res = fac1;
		gen_code(temp_op, num1, num2, res);
	}
	cur_var = fac1;
}

/*
�����ӣ�    ::= ����ʶ����������ʶ������[�������ʽ����]��|��(�������ʽ����)������������|���ַ��������з���ֵ����������䣾   
*/

void factor() {
	set<symbol> end_factor = beg_stat;

	if(cur_sym.second == ident) {
		string temp_name = cur_sym.first;

		streamoff pre_index = fin.tellg();
		pair<string, symbol> pre_sym = cur_sym;
		getsym();
		if(cur_sym.second == lbrack) {   // ����
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

			// ������ȡֵ������ʱ����
			op = "assign_to_var";
			num1 = temp_name;
			num2 = cur_var;
			cur_var = gen_temp_var();
			res = cur_var;
			gen_code(op, num1, num2, res);

			cur_type = sym_tab.tab[index].t;
		}
		else {
			// ����ű��ж��Ǻ������Ǳ���
			int index = sym_tab.find_func(temp_name);

			// ��������
			if(index != -1) {
				type tp = sym_tab.tab[index].t;
				if(tp != voids) {
					reset(pre_index);
					cur_sym = pre_sym;
					cur_var = call(true);            // �������÷��ص���ʱ�������з���ֵ�ģ�
					cur_type = tp;
				}
				else {
					error1(Factor_void_func);
					return;
				}
			}
			// ����
			else {
				index = sym_tab.find_var(temp_name);
				if(index != -1) {
					kind k = sym_tab.tab[index].k;
					// ����
					if(k == consts) {
						cur_var = to_string((long long)sym_tab.tab[index].adr);  // ����ֱ��Ϊֵ
						cur_type = sym_tab.tab[index].t;
					}
					// ����
					else {
						cur_var = temp_name;
						cur_type = sym_tab.tab[index].t;
					}
				}
				// δ�ڷ��ű����ҵ�
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
		if(kind != 0) {   // char���������
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