#include "stdafx.h"
#include "midcode.h"

vector<order> code;
ofstream midcode_stream;

void gen_code(string op, string num1, string num2, string res) {
	order cur_order;
	cur_order.op = op;
	cur_order.num1 = num1;
	cur_order.num2 = num2;
	cur_order.res = res;
	code.push_back(cur_order);
}

string gen_temp_var() {
	static int var_no = 0;
	var_no++;
	// ∑¿÷π÷ÿ√˚£¨º”$
	string temp_head = "$temp_";
	string temp_num = to_string((long long)var_no);
	string temp = temp_head + temp_num;
	return temp;
}

string gen_label() {
	static int label_no = 0;
	label_no++;
	string label_head = "label_";
	string label_num = to_string((long long) label_no);
	string label = label_head + label_num;
	return label;
}

void print_midcode() {
	size_t n = code.size();
	for(size_t i = 0;i < n;i++) {
		midcode_stream << code[i].op << ", " << code[i].num1 << ", " << code[i].num2 << ", " << code[i].res << endl;
	}
}