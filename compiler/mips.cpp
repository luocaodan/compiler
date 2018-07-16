#include "stdafx.h"
#include "mips.h"

ofstream mips_stream;
static size_t code_no;					// 处理到中间代码的位置
static vector<var_info> vars;			// 存放变量（包括源码中的变量和生成的临时变量）的名字和相对$fp的偏移
static vector<var_info> global_vars;	// 全局变量
static int dx = 8;						// 运行栈内变量相对$fp的偏移，初始化为8，($fp) 调用者基址  4($fp) 当前函数返回地址
static int global_dx = 0;				// 全局变量相对$gp的偏移
static int valpara_count = 0;			// 当前值参个数
static bool main_flag;					// 标记main函数

void insert_global_var(string name, int size) {
	var_info g_var;
	g_var.name = name;
	g_var.offset = global_dx;
	global_vars.push_back(g_var);
	global_dx += size;
}

void insert_var_info(string name, int size) {
	var_info temp;
	temp.name = name;
	temp.offset = dx;
	vars.push_back(temp);
	dx += size;
}

var_adr find_var(string name) {
	var_adr temp;
	size_t len = vars.size();
	// 在局部变量和临时变量中查找
	for(size_t i = 0;i < len;i++) {
		if(vars[i].name == name) {
			temp.reg = "$fp";
			temp.offset = -vars[i].offset;
			return temp;
		}
	}
	// 在全局变量中查找
	len = global_vars.size();
	for(size_t i = 0;i < len;i++) {
		if(global_vars[i].name == name) {
			temp.reg = "$gp";
			temp.offset = global_vars[i].offset;
			return temp;
		}
	}
	temp.reg = "not_alloc";				// 未找到则为还没有为已经开辟好的空间分配临时变量
	return temp;
}

void data_segment() {					// 数据段
	mips_stream << ".data" << endl;
	mips_stream << "$global:\t.space\t4" << endl;
	// 全局变量
	size_t index = 0;
	while(code[index].op != "end") {
		string var_name = code[index].res;
		// 变量
		if(code[index].op == "var") {
			mips_stream << var_name << ":\t.space\t4" << endl;
			insert_global_var(var_name, 4);
		}
		// 数组
		else {
			int array_space = atoi(code[index].num2.c_str());
			array_space *= 4;
			mips_stream << var_name << ":\t.space\t" << array_space << endl;
			insert_global_var(var_name, array_space);
		}
		index++;
	}
	code_no = index + 1;   // 从函数处开始
	// 字符串常量
	size_t len = code.size();
	while(index < len) {
		if(code[index].op == "printf") {
			if(code[index].num1 != "") {
				mips_stream << code[index].num1 << ": .asciiz " << "\"" << str_tab[code[index].num1] << "\"" << endl;
			}
		}
		index++;
	}
}

void text_segment() {
	mips_stream << ".text" << endl;
	mips_stream << "la $gp, $global" << endl;			// $gp指向全局变量基地址（data段从$global开始）
	mips_stream << "addi $gp, $gp, 4" << endl;
	mips_stream << "subi $sp, $sp, 4" << endl;	// main函数sp-4，留出prev abp位置
	mips_stream << "move $fp, $sp" << endl;		// 设置main函数基址
	mips_stream << "j\t" << "main" << endl;
	//mips_stream << "nop" << endl;

	size_t len = code.size();
	while(code_no < len) {
		order temp = code[code_no];
		if(temp.op == "var") {
			var_m();
		}
		else if(temp.op == "array") {
			array_m();
		}
		else if(temp.op == "func") {
			func_m();
		}
		else if(temp.op == "para") {
			para_m();
		}
		else if(temp.op == "end") {
			end_m();
		}
		else if(temp.op == "call") {
			call_m();
		}
		else if(temp.op == "valpara") {
			valpara_m();
		}
		else if(temp.op == "assign") {
			assign_m();
		}
		else if(temp.op == "assign_to_var") {
			assign_to_var_m();
		}
		else if(temp.op == "assign_to_array") {
			assign_to_array_m();
		}
		else if(temp.op == "scanf") {
			scanf_m();
		}
		else if(temp.op == "printf") {
			printf_m();
		}
		else if(temp.op == "return") {
			return_m();
		}
		else if(temp.op == "add" || temp.op == "sub" || temp.op == "mul" || temp.op == "div") {
			op_m(temp.op);
		}
		else if(temp.op == "beq" || temp.op == "bne" || temp.op == "ble" ||
				temp.op == "bge" || temp.op == "blt" || temp.op == "bgt") {
			condition_m(temp.op);
		}
		else if(temp.op == "jmp") {
			jmp_m();
		}
		else if(temp.op == "label") {
			label_m();
		}
		else {
			exit_m();
		}
		code_no++;
	}
}

void assemble() {
	data_segment();
	text_segment();
}

void var_m() {
	insert_var_info(code[code_no].res, 4);
}

void array_m() {
	int arr_size = atoi(code[code_no].num2.c_str()) * 4;
	insert_var_info(code[code_no].res, arr_size);
}

void func_m() {
	string func_name = code[code_no].res;
	// 标记是否为main函数
	if(func_name == "main") {
		main_flag = true;
	}
	int index = sym_tab.find_func(func_name);

	// 函数开始dx初始化为8
	dx = 8;
	// 函数标签
	mips_stream << func_name << ":" << endl;
	// 保存当前函数的返回地址
	mips_stream << "\tsubi $sp, $sp, 4" << endl;
	mips_stream << "\tsw $ra, ($sp)" << endl;
	// 在函数开始为参数，局部变量和临时变量分配好空间
	// 参数
	int para_space =  sym_tab.tab[index].size * 4;
	// 局部变量
	size_t i = code_no + 1 + sym_tab.tab[index].size;
	int var_space = 0;
	while(code[i].op == "var" || code[i].op == "array") {
		if(code[i].op == "var") {
			var_space += 4;
		}
		else {
			var_space += 4 * atoi(code[i].num2.c_str());
		}
		i++;
	}
	// 临时变量
	int temp_space = 0;
	set<string> temp_var_tab;
	while(i < code.size() && code[i].op != "end" && code[i].op != "exit") {
		if(code[i].op == "add" || code[i].op == "sub" || code[i].op == "mul" || code[i].op == "div" || 
			code[i].op == "assign" || code[i].op == "assign_to_var" || code[i].op == "call") {
			string temp_name = code[i].res;
			if(temp_name.size() > 1 && temp_name[0] == '$' && temp_name[1] == 't') {
				temp_var_tab.insert(temp_name);
			}
		}
		i++;
	}
	temp_space = temp_var_tab.size() * 4;
	int space_sum = para_space + var_space + temp_space;

	// 开好空间
	mips_stream << "\tsubi $sp, $sp, " << space_sum << endl;

#if OPTM == 1
	// 引用计数
	global_reg_alloc(code_no);
	// 参数分配寄存器
	// para_reg_alloc(code_no);
#endif
}

void para_m() {
	string para_name = code[code_no].res;
	insert_var_info(para_name, 4);
}

void end_m() {
	// 函数结束，变量信息表重置为空
	vars.clear();

	// 取出$ra，保存在fp-4
	mips_stream << "\tlw $ra, -4($fp)" << endl;
	// 当前运行栈退栈
	mips_stream << "\taddi $sp, $fp, 4" << endl;
	// 恢复调用者活动记录基址
	mips_stream << "\tlw $fp, ($fp)" << endl;
	// 跳转
	mips_stream << "\tjr $ra" << endl;
	//mips_stream << "\tnop" << endl;

#if OPTM == 1
	// 清空全局寄存器分配表
	global_reg_tab.clear();
#endif
}

void call_m() {
	// 值参个数重置为0
	valpara_count = 0;

#if OPTM == 1
	// 保存全局寄存器
	for(map<string, string>::iterator it = global_reg_tab.begin();it != global_reg_tab.end();it++) {
		string name = it->first;
		var_adr reg_adr = find_var(name);
		mips_stream << "\tsw " << global_reg_tab[name] << ", " << reg_adr.offset << "(" << reg_adr.reg << ")" << endl;
	}
#endif

	string func_name = code[code_no].num1;
	// 保存调用者的$fp
	mips_stream << "\tsubi $sp, $sp, 4" << endl;
	mips_stream << "\tsw $fp, ($sp)" << endl;
	// 设置被调用函数的fp
	mips_stream << "\tmove $fp, $sp" << endl;
	// jal 
	mips_stream << "\tjal " << func_name << endl;
	//mips_stream << "\tnop" << endl;

	// 函数调用返回值赋给变量
	if(code[code_no].res != "") {
		// 插入临时变量
		insert_var_info(code[code_no].res, 4);
		var_adr temp_adr = find_var(code[code_no].res);
		// 返回值在$v1寄存器，写入到变量对应位置
		mips_stream << "\tsw $v1, " << temp_adr.offset << "(" << temp_adr.reg << ")" << endl;
	}
#if OPTM == 1
	// 重新取出全局寄存器的值
	for(map<string, string>::iterator it = global_reg_tab.begin();it != global_reg_tab.end();it++) {
		string name = it->first;
		var_adr reg_adr = find_var(name);
		mips_stream << "\tlw " << global_reg_tab[name] << ", " << reg_adr.offset << "(" << reg_adr.reg << ")" << endl;
	}
#endif
}

void valpara_m() {
	valpara_count++;
	// (sp) 存储当前函数最后一个变量，sp-4为调用者的fp（活动记录基地址）, sp-8为被调用函数ra，sp-12为第一个参数
	
#if OPTM == 0
	// 取出参数值到$t0
	string valpara_name = code[code_no].res;
	// 常量
	if(is_int(valpara_name)) {
		int para_value = atoi(valpara_name.c_str());
		mips_stream << "\tli $t0, " << para_value << endl;
	}
	// 变量
	else {
		var_adr para_adr = find_var(valpara_name);

		mips_stream << "\tlw $t0, " << para_adr.offset << "(" << para_adr.reg << ")" << endl;
	}
	// 将$t0的值写入到形参变量位置
	int pos = -(valpara_count*4 + 8);
	mips_stream << "\tsw $t0, " << pos << "($sp)" << endl;
#else
	// 计算参数位置
	int pos = -(valpara_count*4 + 8);
	string valpara_name = code[code_no].res;
	// 常量取到$t1
	if(is_int(valpara_name)) {
		int para_value = atoi(valpara_name.c_str());
		mips_stream << "\tli $t0, " << para_value << endl;
		mips_stream << "\tsw $t0, " << pos << "($sp)" << endl;
	}
	// 分配了寄存器
	else if(global_reg_tab.find(valpara_name) != global_reg_tab.end()) {
		mips_stream << "\tsw " << global_reg_tab[valpara_name] << ", " << pos << "($sp)" << endl;
	}
	// 
	else {
		var_adr para_adr = find_var(valpara_name);
		mips_stream << "\tlw $t0, " << para_adr.offset << "(" << para_adr.reg << ")" << endl;
		mips_stream << "\tsw $t0, " << pos << "($sp)" << endl;
	}
#endif
}

void return_m() {
	string temp_name = code[code_no].res;
	// 有返回值
#if OPTM == 0
	if(temp_name != "") {
		if(is_int(temp_name)) {
			int ret_value = atoi(temp_name.c_str());
			mips_stream << "\tli $v1, " << ret_value << endl;
		}
		else {
			// 变量寻址
			var_adr temp_adr = find_var(temp_name); 
			// 返回值写入v1寄存器
			mips_stream << "\tlw $v1, " << temp_adr.offset << "(" << temp_adr.reg << ")" << endl;
		}
	}
#else
	if(temp_name != "") {
		if(is_int(temp_name)) {
			int ret_value = atoi(temp_name.c_str());
			mips_stream << "\tli $v1, " << ret_value << endl;
		}
		else if(global_reg_tab.find(temp_name) != global_reg_tab.end()) {
			mips_stream << "\tmove $v1, " << global_reg_tab[temp_name] << endl;
		}
		else {
			// 变量寻址
			var_adr temp_adr = find_var(temp_name); 
			// 返回值写入v1寄存器
			mips_stream << "\tlw $v1, " << temp_adr.offset << "(" << temp_adr.reg << ")" << endl;
		}
	}
#endif

	// 取出$ra，保存在fp-4
	mips_stream << "\tlw $ra, -4($fp)" << endl;
	// 当前运行栈退栈
	mips_stream << "\taddi $sp, $fp, 4" << endl;
	// 恢复调用者活动记录基址
	mips_stream << "\tlw $fp, ($fp)" << endl;

	if(main_flag) {
		exit_m();
	}
	else {
		// 跳转
		mips_stream << "\tjr $ra" << endl;
		//mips_stream << "\tnop" << endl;
	}
}

void assign_m() {
#if OPTM == 0
	// 取出左值地址
	string var_name = code[code_no].res;
	var_adr left_adr = find_var(var_name);
	if(left_adr.reg == "not_alloc") {
		insert_var_info(var_name, 4);
		left_adr = find_var(var_name);
	}
	
	// 将右值的值赋给$t0
	// 右值（常量或变量）
	string right = code[code_no].num1;
	// 常量 li
	if(is_int(right)) {
		mips_stream << "\tli $t0, " << right << endl;
	}
	// 变量 lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t0, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
	}

	// $t0存入左值地址
	mips_stream << "\tsw $t0, " << left_adr.offset << "(" << left_adr.reg << ")" << endl;
#else 
	// 右值（常量或变量）
	string right = code[code_no].num1;
	string right_reg;
	// 常量 li
	if(is_int(right)) {
		mips_stream << "\tli $t0, " << right << endl;
		right_reg = "$t0";
	}
	else if(global_reg_tab.find(right) != global_reg_tab.end()) {
		right_reg = global_reg_tab[right];
	}
	// 变量 lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t0, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
		right_reg = "$t0";
	}
	// 左
	string var_name = code[code_no].res;
	if(global_reg_tab.find(var_name) != global_reg_tab.end()) {	
		mips_stream << "\tmove " << global_reg_tab[var_name] << ", " << right_reg << endl;
	}
	else {
		var_adr left_adr = find_var(var_name);
		if(left_adr.reg == "not_alloc") {
			insert_var_info(var_name, 4);
			left_adr = find_var(var_name);
		}
		mips_stream << "\tsw " << right_reg << ", " << left_adr.offset << "(" << left_adr.reg << ")" << endl;
	}
#endif
}

void assign_to_var_m() {
	// 数组赋给变量
	string arr_name = code[code_no].num1;
	string index = code[code_no].num2;

	// 取出左值地址
	string var_name = code[code_no].res;
	var_adr left_adr = find_var(var_name);
	if(left_adr.reg == "not_alloc") {
		insert_var_info(var_name, 4);
		left_adr = find_var(var_name);
	}
	// 将数组首地址放在$t0
	var_adr arr_adr = find_var(arr_name);
	// 全局变量
	if(arr_adr.reg == "$gp") {
		mips_stream << "\taddi $t0, $gp, " << arr_adr.offset << endl;
	}
	// 局部变量
	else {
		mips_stream << "\tsubi $t0, $fp, " << -arr_adr.offset << endl;
	}
#if OPTM == 0
	// 将数组元素的偏移放在$t1
	// 索引为常量
	if(is_int(index)) {
		mips_stream << "\tli $t1, " << index << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
	// 索引为变量
	else {
		var_adr index_adr = find_var(index);
		mips_stream << "\tlw $t1, " << index_adr.offset << "(" << index_adr.reg << ")" << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
#else
	// 将数组元素的偏移放在$t1
	// 索引为常量
	if(is_int(index)) {
		mips_stream << "\tli $t1, " << index << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
	else if(global_reg_tab.find(index) != global_reg_tab.end()) {
		mips_stream << "\tsll $t1, " << global_reg_tab[index] << ", 2" << endl;
	}
	// 索引为变量
	else {
		var_adr index_adr = find_var(index);
		mips_stream << "\tlw $t1, " << index_adr.offset << "(" << index_adr.reg << ")" << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
#endif
	// 全局变量加偏移
	if(arr_adr.reg == "$gp") {
		mips_stream << "\tadd $t0, $t0, $t1" << endl;
	}
	// 局部变量减偏移
	else {
		mips_stream << "\tsub $t0, $t0, $t1" << endl;
	}

	// 取出数组元素值放在$t1中
	mips_stream << "\tlw $t1, ($t0)" << endl;

	// $t1值存入变量地址
	mips_stream << "\tsw $t1, " << left_adr.offset << "(" << left_adr.reg << ")" << endl;

}

void assign_to_array_m() {
	// 变量赋给数组
	string arr_name = code[code_no].res;
	string index = code[code_no].num2;

	// 取出右值地址
	string right = code[code_no].num1;
#if OPTM == 0
	// 右值放在$t2
	// 常量 li
	if(is_int(right)) {
		mips_stream << "\tli $t2, " << right << endl;
	}
	// 变量 lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t2, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
	}
#else
	// 右值放在right_reg
	// 常量 li
	string right_reg = "$t2";
	if(is_int(right)) {
		mips_stream << "\tli $t2, " << right << endl;
	}
	else if(global_reg_tab.find(right) != global_reg_tab.end()) {
		right_reg = global_reg_tab[right];
	}
	// 变量 lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t2, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
	}
#endif

	// 将数组首地址放在$t0
	var_adr arr_adr = find_var(arr_name);
	// 全局变量
	if(arr_adr.reg == "$gp") {
		mips_stream << "\taddi $t0, $gp, " << arr_adr.offset << endl;
	}
	// 局部变量
	else {
		mips_stream << "\tsubi $t0, $fp, " << -arr_adr.offset << endl;
	}

	// 将数组元素的偏移放在$t1
	// 索引为常量
	if(is_int(index)) {
		mips_stream << "\tli $t1, " << index << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
	// 索引为变量
#if OPTM == 1
	else if(global_reg_tab.find(index) != global_reg_tab.end()) {
		mips_stream << "\tsll $t1, " << global_reg_tab[index] << ", 2" << endl;
	}
#endif
	else {
		var_adr index_adr = find_var(index);
		mips_stream << "\tlw $t1, " << index_adr.offset << "(" << index_adr.reg << ")" << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}

	// 全局变量加偏移
	if(arr_adr.reg == "$gp") {
		mips_stream << "\tadd $t0, $t0, $t1" << endl;
	}
	// 局部变量减偏移
	else {
		mips_stream << "\tsub $t0, $t0, $t1" << endl;
	}
#if OPTM == 0
	// $t2存入数组元素位置
	mips_stream << "\tsw $t2, ($t0)" << endl;
#else
	// right_reg 存入数组元素位置
	mips_stream << "\tsw " << right_reg << ", ($t0)" << endl;
#endif
}

void op_m(string op) {
	// 操作数1放入$t1，操作数2放入$t2，结果放入$t0
	string var_1 = code[code_no].num1;
	string var_2 = code[code_no].num2;
	// 取出左值地址
	string left = code[code_no].res;
	var_adr left_adr = find_var(left);
	if(left_adr.reg == "not_alloc") {
		insert_var_info(left, 4);
		left_adr = find_var(left);
	}

	// 操作数1放入num1_reg
	string num1_reg = "$t1";
	if(is_int(var_1)) {
		mips_stream << "\tli $t1, " << var_1 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_1) != global_reg_tab.end()) {
		num1_reg = global_reg_tab[var_1];
	}
#endif
	// 变量 lw
	else {
		var_adr var_1_adr = find_var(var_1);
		mips_stream << "\tlw $t1, " << var_1_adr.offset << "(" << var_1_adr.reg << ")" << endl;
	}

	// 操作数2放入num2_reg
	string num2_reg = "$t2";
	if(is_int(var_2)) {
		mips_stream << "\tli $t2, " << var_2 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_2) != global_reg_tab.end()) {
		num2_reg = global_reg_tab[var_2];
	}
#endif
	// 变量 lw
	else {
		var_adr var_2_adr = find_var(var_2);
		mips_stream << "\tlw $t2, " << var_2_adr.offset << "(" << var_2_adr.reg << ")" << endl;
	}
	
	// 运算
	mips_stream << "\t" << op << " $t0, " << num1_reg << ", " << num2_reg << endl;
	// 存结果
	mips_stream << "\tsw $t0, " << left_adr.offset << "(" << left_adr.reg << ")" << endl;
}

void condition_m(string op) {
	// 操作数1放入num1_reg，操作数2放入num2_reg
	string var_1 = code[code_no].num1;
	string var_2 = code[code_no].num2;

	// 操作数1放入num1_reg
	string num1_reg = "$t1";
	if(is_int(var_1)) {
		mips_stream << "\tli $t1, " << var_1 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_1) != global_reg_tab.end()) {
		num1_reg = global_reg_tab[var_1];
	}
#endif
	// 变量 lw
	else {
		var_adr var_1_adr = find_var(var_1);
		mips_stream << "\tlw $t1, " << var_1_adr.offset << "(" << var_1_adr.reg << ")" << endl;
	}

	// 操作数2放入num2_reg
	string num2_reg = "$t2";
	if(is_int(var_2)) {
		mips_stream << "\tli $t2, " << var_2 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_2) != global_reg_tab.end()) {
		num2_reg = global_reg_tab[var_2];
	}
#endif
	// 变量 lw
	else {
		var_adr var_2_adr = find_var(var_2);
		mips_stream << "\tlw $t2, " << var_2_adr.offset << "(" << var_2_adr.reg << ")" << endl;
	}

	// 比较 beg $t1, $t2, label
	mips_stream << "\t" << op << " " << num1_reg << ", " << num2_reg << ", " << code[code_no].res << endl;
	//mips_stream << "\tnop" << endl;
}

void jmp_m() {
	// 无条件跳转
	mips_stream << "\tj " << code[code_no].res << endl;
	//mips_stream << "\tnop" << endl;
}

void label_m() {
	// 打标签
	mips_stream << code[code_no].res << ":" << endl;
}

void scanf_m() {
	// 为$v0赋系统调用号，int为5，char为12
	if(code[code_no].num2 == "int") {
		mips_stream << "\tli $v0, 5" << endl;
	}
	else {
		mips_stream << "\tli $v0, 12" << endl;
	}

	mips_stream << "\tsyscall" << endl;
#if OPTM == 0
	var_adr read_adr = find_var(code[code_no].res);
	// $v0中的值写入变量地址
	mips_stream << "\tsw $v0, " << read_adr.offset << "(" << read_adr.reg << ")" << endl;
#else
	if(global_reg_tab.find(code[code_no].res) != global_reg_tab.end()) {
		mips_stream << "\tmove " << global_reg_tab[code[code_no].res] << ", $v0" << endl;
	}
	else {
		var_adr read_adr = find_var(code[code_no].res);
		// $v0中的值写入变量地址
		mips_stream << "\tsw $v0, " << read_adr.offset << "(" << read_adr.reg << ")" << endl;
	}
#endif
}

void printf_m() {
	// 打印字符串
	if(code[code_no].num1 != "") {
		// 取字符串首地址到$a0
		mips_stream << "\tla $a0, " << code[code_no].num1 << endl;
		// print string 系统调用号为4
		mips_stream << "\tli $v0, 4" << endl;
		mips_stream << "\tsyscall" << endl;
	}
	// 打印表达式
	if(code[code_no].num2 != "") {
		// 为$v0赋系统调用号，int为1，char为11
		if(code[code_no].res == "int") {
			mips_stream << "\tli $v0, 1" << endl;
		}
		else {
			mips_stream << "\tli $v0, 11" << endl;
		}
		// 要打印的值放入$a0
		// 常量
		if(is_int(code[code_no].num2)) {
			mips_stream << "\tli $a0, " << code[code_no].num2 << endl;
		}
#if OPTM == 1
		else if(global_reg_tab.find(code[code_no].num2) != global_reg_tab.end()) {
			mips_stream << "\tmove $a0, " << global_reg_tab[code[code_no].num2] << endl;
		}
#endif
		else {
			var_adr print_adr = find_var(code[code_no].num2);
			mips_stream << "\tlw $a0, " << print_adr.offset << "(" << print_adr.reg << ")" << endl;
		}
		mips_stream << "\tsyscall" << endl;
	}
	// 最后输出换行
	mips_stream << "\tli $v0, 11" << endl;
	mips_stream << "\tli $a0, 10" << endl;
	mips_stream << "\tsyscall" << endl;
}

void exit_m() {
	// 退出程序系统调用号10
	mips_stream << "\tli $v0, 10" << endl;
	mips_stream << "\tsyscall" << endl;
}

bool is_int(string str) {
	if(str[0] == '-' || str[0] >= '0' && str[0] <= '9') {
		return true;
	}
	return false;
}
