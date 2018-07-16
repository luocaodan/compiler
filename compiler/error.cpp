#include "stdafx.h"
#include "error.h"

ofstream error_stream;
vector<compile_err> errs;
int ll = 1;
int cc = -1;
int errpos = 0;
bool err_flag = false;

void error(int n) {
	err_flag = true;
	compile_err err;
	err.line = ll;
	err.start = errpos;
	err.end = cc;
	err.err_code = n;
	errs.push_back(err);
}

void error1(int n) {
	err_flag = true;
	compile_err err;
	err.line = ll;
	err.start = cc - cur_sym.first.size() + 1;
	err.end = cc + 1;
	err.err_code = n;
	errs.push_back(err);
}

void print_error() {
	if(errs.size() != 0){
		for(size_t i = 0;i < errs.size();i++) {
			error_stream << "error:" << error_msg(errs[i].err_code) << " in line " << errs[i].line << " position " << errs[i].start+1 << endl;
		}
		error_stream << endl;
		errs.clear();
	}
}

string error_msg(int n) {
	string msg[60];

	msg[0] = "未知字符";
	msg[1] = "整形或字符常量超出范围";
	msg[2] = "字符常量中字符太多";
	msg[3] = "缺少右单引号";
	msg[4] = "缺少右双引号";
	msg[5] = "字符串常量过长";
	msg[6] = "缺少\"}\"";
	msg[7] = "标识符重定义";
	msg[8] = "缺少类型说明符";
	msg[9] = "缺少\"]\"";
	msg[10] = "缺少数组大小";
	msg[11] = "标识符未声明";
	msg[12] = "缺少\";\"";
	msg[13] = "缺少\"(\"";			 // if while 函数调用
	msg[14] = "缺少条件表达式";
	msg[15] = "缺少\")\"";
	msg[16] = "if 语句缺少 else";
	msg[17] = "case 语句缺少\":\"";
	msg[18] = "case 语法错误";
	msg[19] = "非法的default";
	msg[20] = "函数参数过少";
	msg[21] = "函数参数过多";
	msg[22] = "函数没有返回值";
	msg[23] = "void 函数不应该返回值";
	msg[24] = "缺少main函数或程序未按文法格式排列";
	msg[25] = "期待一个标识符";
	msg[26] = "常量定义缺少赋值符号";
	msg[27] = "常量赋值错误";
	msg[28] = "缺少\"{\"";
	msg[29] = "条件运算符错误";
	msg[30] = "整型常量不能有前导0";
	msg[31] = "赋值语句语法错误";
	msg[32] = "因子错误";
	msg[33] = "变量声明语法错误";
	msg[34] = "因子中无返回值函数参加运算";
	msg[35] = "函数未定义";
	msg[36] = "类型不匹配";

	return msg[n];
}

void skip(set<symbol> symset, int n) {
	error1(n);
	//getsym();
	while(symset.find(cur_sym.second) == symset.end() && cur_sym.second != endsy) {
		getsym();
	}
}

void skip(symbol expected, int n) {
	error1(n);
	//getsym();
	while(cur_sym.second != expected && cur_sym.second != endsy) {
		getsym();
	}
}

bool test(symbol expected, symbol des, int n) {
	if(cur_sym.second != expected) {
		skip(des, n);
		return false;
	}
	return true;
}

bool test(symbol expected, set<symbol> symset, int n) {
	if(cur_sym.second != expected) {
		getsym();
		skip(symset, n);
		return false;
	}
	return true;
}

bool test(set<symbol> expected_set, symbol des, int n) {
	if(expected_set.find(cur_sym.second)==expected_set.end()) {
		getsym();
		skip(des, n);
		return false;
	}
	return true;
}

bool test(set<symbol> set1, set<symbol> set2, int n) {
	if(set1.find(cur_sym.second)==set1.end()) {
		skip(set2, n);
		return false;
	}
	return true;
}