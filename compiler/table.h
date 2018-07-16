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
	string name;   // 变量名
	kind k;        // 种类
	type t;		   // 类型
	int adr;       // 相对偏移，常量为常量的值
	int size;	   // 数组大小或函数参数个数
};

struct table {
	vector<item> tab;     // 符号表
	vector<int> block;   // 分程序索引表
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
