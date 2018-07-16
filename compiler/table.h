#pragma once
#include "stdafx.h"
const int max_lev = 10;

enum kind {
	consts, var, function, parameter, arrays
};

enum type {
	voids, ints, chars
};

struct item {
	string name;   // ������
	kind k;        // ����
	type t;		   // ����
	int adr;       // ���ƫ�ƣ�����Ϊ������ֵ
	int size;	   // �����С������������
};

struct table {
	vector<item> tab;     // ���ű�
	vector<int> block;   // �ֳ���������
	table();
	~table();
	int find_var(string name);
	int find_var_local(string name);
	int find_func(string name);
	bool insert(string name, kind k, type t, int adr, int size);
	bool insert_const(string name, type t, int value);
	bool insert_var(string name, type t, int adr);
	bool insert_array(string name, type t, int adr, int size);
	bool insert_func(string name, type t, int adr);
	bool insert_para(string name, type t, int adr);
};
