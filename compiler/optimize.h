#pragma once
#include "stdafx.h"
#include "midcode.h"
#include "mips.h"

extern map<string, string> global_reg_tab;
extern map<string, string> para_reg_tab;
extern int opt_common_count;

void global_reg_alloc(int func_index);

bool is_var(string name);
	
void add_var(string name, int weight);

void calcu_var_num(order o, int weight);

int loop_weight(int beg_index, int weight);				// ѭ����Ȩ

bool comp(const pair<string, int>& p1, const pair<string, int>& p2);

void gen_opt_code();									// ���������ӱ��ʽ

void const_combine();									// �����ϲ�����������

void para_reg_alloc(int func_index);					// a0-a3