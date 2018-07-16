#pragma once
#include "stdafx.h"

struct order {
	string op;
	string num1;
	string num2;
	string res;
};

extern ofstream midcode_stream;
extern vector<order> code;

void gen_code(string op, string num1, string num2, string res);
string gen_temp_var();
string gen_label();
void print_midcode();