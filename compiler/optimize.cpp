#include "stdafx.h"
#include "optimize.h"

map<string, string> global_reg_tab;
map<string, string> para_reg_tab;
int opt_common_count = 0;

static map<string, int> var_num;

void global_reg_alloc(int func_index) {
	int i = func_index + 1;
	vector<string> local_list;
	while(code[i].op == "para") {
		i++;
	}
	while(code[i].op == "array" || code[i].op == "var") {
		if(code[i].op == "var") {
			local_list.push_back(code[i].res);
		}
		i++;
	}

	while(code[i].op != "end" && code[i].op != "exit") {
		order temp = code[i];
		if(temp.op == "label" && temp.num1 == "while_beg") {
			i = loop_weight(i, 10);
			i++;
		}
		else {
			calcu_var_num(temp, 1);
			i++;
		}
	}
	
	// 只对局部变量分配
	for(map<string, int>::iterator it = var_num.begin();it != var_num.end();) {
		vector<string>::iterator result = find(local_list.begin(), local_list.end(), it->first);
		if(result == local_list.end()) {
			it = var_num.erase(it);
		}
		else {
			it++;
		}
	}

	// 排序
	vector<pair<string, int>> sorted(var_num.begin(), var_num.end());
	sort(sorted.begin(), sorted.end(), comp);

	/* 调试信息
	cout << code[func_index].res << endl;
	for(vector<pair<string, int>>::iterator it = sorted.begin();it != sorted.end();it++) {
		cout << it->first << ": " << it->second << endl;
	}
	cout << "-------------------------------------" << endl;
	*/
	
	// 分配寄存器
	int reg_num = 0;
	for(vector<pair<string, int>>::iterator it = sorted.begin();it != sorted.end() && reg_num <= 7;it++) {
		global_reg_tab[it->first] = "$s" + to_string((long long)reg_num);
		reg_num++;
	}
	
	/* 调试信息
	cout << code[func_index].res << endl;
	for(map<string, string>::iterator it = global_reg_tab.begin();it != global_reg_tab.end();it++) {
		cout << it->first << ": " << it->second << endl;
	}
	cout << "-------------------------------------" << endl;
	*/

	// 清空var_num
	var_num.clear();
}

bool is_var(string name) {
	if(name.size() == 0) {
		return false;
	}
	if(is_int(name)) {
		return false;
	}
	if(name[0] == '$') {
		return false;
	}
	return true;
}

void add_var(string name, int weight) {
	if(!is_var(name)) {
		return;
	}
	if(var_num.find(name) == var_num.end()) {
		var_num[name] = weight;
	}
	else {
		var_num[name] += weight;
	}
}

void calcu_var_num(order o, int weight) {
	if(o.op == "mul" || o.op == "div" || o.op == "sub" || o.op == "add") {
		add_var(o.num1, weight);
		add_var(o.num2, weight);
	}
	else if(o.op == "assign_to_var") {
		add_var(o.num2, weight);
	}
	else if(o.op == "assign_to_array") {
		add_var(o.num1, weight);
		add_var(o.num2, weight);
	}
	else if(o.op == "assign") {
		add_var(o.num1,weight);
		add_var(o.res, weight);
	}
	else if(o.op == "valpara") {
		add_var(o.res, weight);
	}
	else if(o.op == "scanf") {
		add_var(o.res, weight);
	}
	else if(o.op == "printf") {
		add_var(o.num2, weight);
	}
	else if(o.op == "return") {
		add_var(o.res, weight);
	}
	else if(o.op == "beq" || o.op == "bne" || o.op == "ble" ||
			o.op == "bge" || o.op == "blt" || o.op == "bgt") {
		add_var(o.num1, weight);
		add_var(o.num2, weight);
	}
}

// 循环加权
int loop_weight(int beg_index, int weight) {
	int i = ++beg_index;
	order temp = code[i];
	while(temp.op != "label" || temp.num1 != "while_end") {
		if(temp.op == "label" && temp.num1 == "while_beg") {
			i = loop_weight(i, weight * 10);
			i++;
			temp = code[i];
		}
		else {
			calcu_var_num(temp, weight);
			i++;
			temp = code[i];
		}
	}
	return i;
}

bool comp(const pair<string, int>& p1, const pair<string, int>& p2) {
	return p1.second > p2.second;
}

void gen_opt_code() {
	for(size_t i = 0;i < code.size();i++) {
		if(code[i].op == "mul" || code[i].op == "div" ||
		   code[i].op == "add" || code[i].op == "sub")
		{
			vector<order> temp;
			// 变量映射表
			map<string, string> var_map_tab;
			while(code[i].op == "mul" || code[i].op == "div" ||
				  code[i].op == "add" || code[i].op == "sub")
			{
				order& first = code[i];
				size_t j;
				for(j = 0;j < temp.size();j++) {
					order& second = temp[j];
					if(first.op == second.op && 
					   first.num1 == second.num1 && first.num2 == second.num2) 
					{
						var_map_tab[first.res] = second.res;
						// 调试信息
						//cout << "delete common expression " << first.num1 << " " << first.op << " " << first.num2 << endl;
						code.erase(code.begin()+i);
						opt_common_count++;
						i--;
						break;
					}
				}
				if(j == temp.size()) {
					if(var_map_tab.find(first.num1) != var_map_tab.end()) {
						first.num1 = var_map_tab[first.num1];
					}
					if(var_map_tab.find(first.num2) != var_map_tab.end()) {
						first.num2 = var_map_tab[first.num2];
					}
					temp.push_back(first);
				}
				i++;
			}
			i--;
		}
	}
}

void const_combine() {
	// 常量映射表
	map<string, int> const_map_tab;
	for(size_t i = 0;i < code.size();i++) {
		if(code[i].op == "mul" || code[i].op == "div" ||
		   code[i].op == "add" || code[i].op == "sub")
		{
			if(const_map_tab.find(code[i].num1) != const_map_tab.end()) {
				code[i].num1 = to_string((long long)const_map_tab[code[i].num1]);
			}
			if(const_map_tab.find(code[i].num2) != const_map_tab.end()) {
				code[i].num2 = to_string((long long)const_map_tab[code[i].num2]);
			}
			if(is_int(code[i].num1) && is_int(code[i].num2)) {
				int n1 = atoi(code[i].num1.c_str());
				int n2 = atoi(code[i].num2.c_str());
				if(code[i].op == "mul") {
					const_map_tab[code[i].res] = n1 * n2;
				}
				else if(code[i].op == "div") {
					const_map_tab[code[i].res] = n1 / n2;
				}
				else if(code[i].op == "add") {
					const_map_tab[code[i].res] = n1 + n2;
				}
				else {
					const_map_tab[code[i].res] = n1 - n2;
				}
				// 调试信息
				// cout << "delete " << code[i].op << " " << code[i].num1 << " " << code[i].num2  << " " << code[i].res << endl;
				code.erase(code.begin() + i);
				i--;
			}
		}
		else {
			if(const_map_tab.find(code[i].num1) != const_map_tab.end()) {
				code[i].num1 = to_string((long long)const_map_tab[code[i].num1]);
			}
			if(const_map_tab.find(code[i].num2) != const_map_tab.end()) {
				code[i].num2 = to_string((long long)const_map_tab[code[i].num2]);
			}
			if(const_map_tab.find(code[i].res) != const_map_tab.end()) {
				code[i].res = to_string((long long)const_map_tab[code[i].res]);
			}
		}
	}
}

void para_reg_alloc(int func_index) {
	int i = func_index + 1;
	int para_count = 0;
	while(code[i].op == "para" && para_count <= 3) {
		para_reg_tab[code[i].res] = "$a" + to_string((long long)para_count);
		para_count++;
		i++;
	}
}