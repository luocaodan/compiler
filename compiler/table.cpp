#include "stdafx.h"
#include "table.h"

table::table() {
	block.push_back(0);
}
table::~table() {

}

bool table::insert(string name, kind k, type t, int adr, int size) {
	item entry;
	entry.name = name;
	entry.k = k;
	entry.t = t;
	entry.adr = adr;
	entry.size = size;
	tab.push_back(entry);
	return true;
}

bool table::insert_const(string name, type t, int value) {
	return insert(name, consts, t, value, 0);
}

bool table::insert_var(string name, type t, int adr) {
	return insert(name, var, t, adr, 0);
}

bool table::insert_array(string name, type t, int adr, int size) {
	return insert(name, arrays, t, adr, size);
}

bool table::insert_func(string name, type t, int adr) {
	insert(name, function, t, adr, 0);
	block.push_back(tab.size());
	return true;
}

bool table::insert_para(string name, type t, int adr) {
	return insert(name, parameter, t, adr, 0);
}

int table::find_var(string name) {
	int start = tab.size() - 1;
	int end = block.back();
	while(start >= end) {
		if(tab[start].name == name) {
			return start;
		}
		start--;
	}
	if(block.size() > 1) {     // 查全局变量
		int global_end = block[1] - 1;
		for(int i = 0;i < global_end;i++) {
			if(tab[i].name == name) {
				return i;
			}
		}
	}
	return -1;
}

int table::find_var_local(string name) {
	int start = tab.size() - 1;
	int end = block.back();
	while(start >= end) {
		if(tab[start].name == name) {
			return start;
		}
		start--;
	}
	return -1;
}

int table::find_func(string name) {
	if(block.size() < 2) {
		return -1;
	}
	for(size_t i = 1;i < block.size();i++) {
		int tab_index = block[i] - 1;
		if(tab[tab_index].name == name) {
			return tab_index;
		}
	}
	return -1;
}