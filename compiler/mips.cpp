#include "stdafx.h"
#include "mips.h"

ofstream mips_stream;
static size_t code_no;					// �����м�����λ��
static vector<var_info> vars;			// ��ű���������Դ���еı��������ɵ���ʱ�����������ֺ����$fp��ƫ��
static vector<var_info> global_vars;	// ȫ�ֱ���
static int dx = 8;						// ����ջ�ڱ������$fp��ƫ�ƣ���ʼ��Ϊ8��($fp) �����߻�ַ  4($fp) ��ǰ�������ص�ַ
static int global_dx = 0;				// ȫ�ֱ������$gp��ƫ��
static int valpara_count = 0;			// ��ǰֵ�θ���
static bool main_flag;					// ���main����

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
	// �ھֲ���������ʱ�����в���
	for(size_t i = 0;i < len;i++) {
		if(vars[i].name == name) {
			temp.reg = "$fp";
			temp.offset = -vars[i].offset;
			return temp;
		}
	}
	// ��ȫ�ֱ����в���
	len = global_vars.size();
	for(size_t i = 0;i < len;i++) {
		if(global_vars[i].name == name) {
			temp.reg = "$gp";
			temp.offset = global_vars[i].offset;
			return temp;
		}
	}
	temp.reg = "not_alloc";				// δ�ҵ���Ϊ��û��Ϊ�Ѿ����ٺõĿռ������ʱ����
	return temp;
}

void data_segment() {					// ���ݶ�
	mips_stream << ".data" << endl;
	mips_stream << "$global:\t.space\t4" << endl;
	// ȫ�ֱ���
	size_t index = 0;
	while(code[index].op != "end") {
		string var_name = code[index].res;
		// ����
		if(code[index].op == "var") {
			mips_stream << var_name << ":\t.space\t4" << endl;
			insert_global_var(var_name, 4);
		}
		// ����
		else {
			int array_space = atoi(code[index].num2.c_str());
			array_space *= 4;
			mips_stream << var_name << ":\t.space\t" << array_space << endl;
			insert_global_var(var_name, array_space);
		}
		index++;
	}
	code_no = index + 1;   // �Ӻ�������ʼ
	// �ַ�������
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
	mips_stream << "la $gp, $global" << endl;			// $gpָ��ȫ�ֱ�������ַ��data�δ�$global��ʼ��
	mips_stream << "addi $gp, $gp, 4" << endl;
	mips_stream << "subi $sp, $sp, 4" << endl;	// main����sp-4������prev abpλ��
	mips_stream << "move $fp, $sp" << endl;		// ����main������ַ
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
	// ����Ƿ�Ϊmain����
	if(func_name == "main") {
		main_flag = true;
	}
	int index = sym_tab.find_func(func_name);

	// ������ʼdx��ʼ��Ϊ8
	dx = 8;
	// ������ǩ
	mips_stream << func_name << ":" << endl;
	// ���浱ǰ�����ķ��ص�ַ
	mips_stream << "\tsubi $sp, $sp, 4" << endl;
	mips_stream << "\tsw $ra, ($sp)" << endl;
	// �ں�����ʼΪ�������ֲ���������ʱ��������ÿռ�
	// ����
	int para_space =  sym_tab.tab[index].size * 4;
	// �ֲ�����
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
	// ��ʱ����
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

	// ���ÿռ�
	mips_stream << "\tsubi $sp, $sp, " << space_sum << endl;

#if OPTM == 1
	// ���ü���
	global_reg_alloc(code_no);
	// ��������Ĵ���
	// para_reg_alloc(code_no);
#endif
}

void para_m() {
	string para_name = code[code_no].res;
	insert_var_info(para_name, 4);
}

void end_m() {
	// ����������������Ϣ������Ϊ��
	vars.clear();

	// ȡ��$ra��������fp-4
	mips_stream << "\tlw $ra, -4($fp)" << endl;
	// ��ǰ����ջ��ջ
	mips_stream << "\taddi $sp, $fp, 4" << endl;
	// �ָ������߻��¼��ַ
	mips_stream << "\tlw $fp, ($fp)" << endl;
	// ��ת
	mips_stream << "\tjr $ra" << endl;
	//mips_stream << "\tnop" << endl;

#if OPTM == 1
	// ���ȫ�ּĴ��������
	global_reg_tab.clear();
#endif
}

void call_m() {
	// ֵ�θ�������Ϊ0
	valpara_count = 0;

#if OPTM == 1
	// ����ȫ�ּĴ���
	for(map<string, string>::iterator it = global_reg_tab.begin();it != global_reg_tab.end();it++) {
		string name = it->first;
		var_adr reg_adr = find_var(name);
		mips_stream << "\tsw " << global_reg_tab[name] << ", " << reg_adr.offset << "(" << reg_adr.reg << ")" << endl;
	}
#endif

	string func_name = code[code_no].num1;
	// ��������ߵ�$fp
	mips_stream << "\tsubi $sp, $sp, 4" << endl;
	mips_stream << "\tsw $fp, ($sp)" << endl;
	// ���ñ����ú�����fp
	mips_stream << "\tmove $fp, $sp" << endl;
	// jal 
	mips_stream << "\tjal " << func_name << endl;
	//mips_stream << "\tnop" << endl;

	// �������÷���ֵ��������
	if(code[code_no].res != "") {
		// ������ʱ����
		insert_var_info(code[code_no].res, 4);
		var_adr temp_adr = find_var(code[code_no].res);
		// ����ֵ��$v1�Ĵ�����д�뵽������Ӧλ��
		mips_stream << "\tsw $v1, " << temp_adr.offset << "(" << temp_adr.reg << ")" << endl;
	}
#if OPTM == 1
	// ����ȡ��ȫ�ּĴ�����ֵ
	for(map<string, string>::iterator it = global_reg_tab.begin();it != global_reg_tab.end();it++) {
		string name = it->first;
		var_adr reg_adr = find_var(name);
		mips_stream << "\tlw " << global_reg_tab[name] << ", " << reg_adr.offset << "(" << reg_adr.reg << ")" << endl;
	}
#endif
}

void valpara_m() {
	valpara_count++;
	// (sp) �洢��ǰ�������һ��������sp-4Ϊ�����ߵ�fp�����¼����ַ��, sp-8Ϊ�����ú���ra��sp-12Ϊ��һ������
	
#if OPTM == 0
	// ȡ������ֵ��$t0
	string valpara_name = code[code_no].res;
	// ����
	if(is_int(valpara_name)) {
		int para_value = atoi(valpara_name.c_str());
		mips_stream << "\tli $t0, " << para_value << endl;
	}
	// ����
	else {
		var_adr para_adr = find_var(valpara_name);

		mips_stream << "\tlw $t0, " << para_adr.offset << "(" << para_adr.reg << ")" << endl;
	}
	// ��$t0��ֵд�뵽�βα���λ��
	int pos = -(valpara_count*4 + 8);
	mips_stream << "\tsw $t0, " << pos << "($sp)" << endl;
#else
	// �������λ��
	int pos = -(valpara_count*4 + 8);
	string valpara_name = code[code_no].res;
	// ����ȡ��$t1
	if(is_int(valpara_name)) {
		int para_value = atoi(valpara_name.c_str());
		mips_stream << "\tli $t0, " << para_value << endl;
		mips_stream << "\tsw $t0, " << pos << "($sp)" << endl;
	}
	// �����˼Ĵ���
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
	// �з���ֵ
#if OPTM == 0
	if(temp_name != "") {
		if(is_int(temp_name)) {
			int ret_value = atoi(temp_name.c_str());
			mips_stream << "\tli $v1, " << ret_value << endl;
		}
		else {
			// ����Ѱַ
			var_adr temp_adr = find_var(temp_name); 
			// ����ֵд��v1�Ĵ���
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
			// ����Ѱַ
			var_adr temp_adr = find_var(temp_name); 
			// ����ֵд��v1�Ĵ���
			mips_stream << "\tlw $v1, " << temp_adr.offset << "(" << temp_adr.reg << ")" << endl;
		}
	}
#endif

	// ȡ��$ra��������fp-4
	mips_stream << "\tlw $ra, -4($fp)" << endl;
	// ��ǰ����ջ��ջ
	mips_stream << "\taddi $sp, $fp, 4" << endl;
	// �ָ������߻��¼��ַ
	mips_stream << "\tlw $fp, ($fp)" << endl;

	if(main_flag) {
		exit_m();
	}
	else {
		// ��ת
		mips_stream << "\tjr $ra" << endl;
		//mips_stream << "\tnop" << endl;
	}
}

void assign_m() {
#if OPTM == 0
	// ȡ����ֵ��ַ
	string var_name = code[code_no].res;
	var_adr left_adr = find_var(var_name);
	if(left_adr.reg == "not_alloc") {
		insert_var_info(var_name, 4);
		left_adr = find_var(var_name);
	}
	
	// ����ֵ��ֵ����$t0
	// ��ֵ�������������
	string right = code[code_no].num1;
	// ���� li
	if(is_int(right)) {
		mips_stream << "\tli $t0, " << right << endl;
	}
	// ���� lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t0, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
	}

	// $t0������ֵ��ַ
	mips_stream << "\tsw $t0, " << left_adr.offset << "(" << left_adr.reg << ")" << endl;
#else 
	// ��ֵ�������������
	string right = code[code_no].num1;
	string right_reg;
	// ���� li
	if(is_int(right)) {
		mips_stream << "\tli $t0, " << right << endl;
		right_reg = "$t0";
	}
	else if(global_reg_tab.find(right) != global_reg_tab.end()) {
		right_reg = global_reg_tab[right];
	}
	// ���� lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t0, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
		right_reg = "$t0";
	}
	// ��
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
	// ���鸳������
	string arr_name = code[code_no].num1;
	string index = code[code_no].num2;

	// ȡ����ֵ��ַ
	string var_name = code[code_no].res;
	var_adr left_adr = find_var(var_name);
	if(left_adr.reg == "not_alloc") {
		insert_var_info(var_name, 4);
		left_adr = find_var(var_name);
	}
	// �������׵�ַ����$t0
	var_adr arr_adr = find_var(arr_name);
	// ȫ�ֱ���
	if(arr_adr.reg == "$gp") {
		mips_stream << "\taddi $t0, $gp, " << arr_adr.offset << endl;
	}
	// �ֲ�����
	else {
		mips_stream << "\tsubi $t0, $fp, " << -arr_adr.offset << endl;
	}
#if OPTM == 0
	// ������Ԫ�ص�ƫ�Ʒ���$t1
	// ����Ϊ����
	if(is_int(index)) {
		mips_stream << "\tli $t1, " << index << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
	// ����Ϊ����
	else {
		var_adr index_adr = find_var(index);
		mips_stream << "\tlw $t1, " << index_adr.offset << "(" << index_adr.reg << ")" << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
#else
	// ������Ԫ�ص�ƫ�Ʒ���$t1
	// ����Ϊ����
	if(is_int(index)) {
		mips_stream << "\tli $t1, " << index << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
	else if(global_reg_tab.find(index) != global_reg_tab.end()) {
		mips_stream << "\tsll $t1, " << global_reg_tab[index] << ", 2" << endl;
	}
	// ����Ϊ����
	else {
		var_adr index_adr = find_var(index);
		mips_stream << "\tlw $t1, " << index_adr.offset << "(" << index_adr.reg << ")" << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
#endif
	// ȫ�ֱ�����ƫ��
	if(arr_adr.reg == "$gp") {
		mips_stream << "\tadd $t0, $t0, $t1" << endl;
	}
	// �ֲ�������ƫ��
	else {
		mips_stream << "\tsub $t0, $t0, $t1" << endl;
	}

	// ȡ������Ԫ��ֵ����$t1��
	mips_stream << "\tlw $t1, ($t0)" << endl;

	// $t1ֵ���������ַ
	mips_stream << "\tsw $t1, " << left_adr.offset << "(" << left_adr.reg << ")" << endl;

}

void assign_to_array_m() {
	// ������������
	string arr_name = code[code_no].res;
	string index = code[code_no].num2;

	// ȡ����ֵ��ַ
	string right = code[code_no].num1;
#if OPTM == 0
	// ��ֵ����$t2
	// ���� li
	if(is_int(right)) {
		mips_stream << "\tli $t2, " << right << endl;
	}
	// ���� lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t2, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
	}
#else
	// ��ֵ����right_reg
	// ���� li
	string right_reg = "$t2";
	if(is_int(right)) {
		mips_stream << "\tli $t2, " << right << endl;
	}
	else if(global_reg_tab.find(right) != global_reg_tab.end()) {
		right_reg = global_reg_tab[right];
	}
	// ���� lw
	else {
		var_adr right_adr = find_var(right);
		mips_stream << "\tlw $t2, " << right_adr.offset << "(" << right_adr.reg << ")" << endl;
	}
#endif

	// �������׵�ַ����$t0
	var_adr arr_adr = find_var(arr_name);
	// ȫ�ֱ���
	if(arr_adr.reg == "$gp") {
		mips_stream << "\taddi $t0, $gp, " << arr_adr.offset << endl;
	}
	// �ֲ�����
	else {
		mips_stream << "\tsubi $t0, $fp, " << -arr_adr.offset << endl;
	}

	// ������Ԫ�ص�ƫ�Ʒ���$t1
	// ����Ϊ����
	if(is_int(index)) {
		mips_stream << "\tli $t1, " << index << endl;
		mips_stream << "\tsll $t1, $t1, 2" << endl;
	}
	// ����Ϊ����
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

	// ȫ�ֱ�����ƫ��
	if(arr_adr.reg == "$gp") {
		mips_stream << "\tadd $t0, $t0, $t1" << endl;
	}
	// �ֲ�������ƫ��
	else {
		mips_stream << "\tsub $t0, $t0, $t1" << endl;
	}
#if OPTM == 0
	// $t2��������Ԫ��λ��
	mips_stream << "\tsw $t2, ($t0)" << endl;
#else
	// right_reg ��������Ԫ��λ��
	mips_stream << "\tsw " << right_reg << ", ($t0)" << endl;
#endif
}

void op_m(string op) {
	// ������1����$t1��������2����$t2���������$t0
	string var_1 = code[code_no].num1;
	string var_2 = code[code_no].num2;
	// ȡ����ֵ��ַ
	string left = code[code_no].res;
	var_adr left_adr = find_var(left);
	if(left_adr.reg == "not_alloc") {
		insert_var_info(left, 4);
		left_adr = find_var(left);
	}

	// ������1����num1_reg
	string num1_reg = "$t1";
	if(is_int(var_1)) {
		mips_stream << "\tli $t1, " << var_1 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_1) != global_reg_tab.end()) {
		num1_reg = global_reg_tab[var_1];
	}
#endif
	// ���� lw
	else {
		var_adr var_1_adr = find_var(var_1);
		mips_stream << "\tlw $t1, " << var_1_adr.offset << "(" << var_1_adr.reg << ")" << endl;
	}

	// ������2����num2_reg
	string num2_reg = "$t2";
	if(is_int(var_2)) {
		mips_stream << "\tli $t2, " << var_2 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_2) != global_reg_tab.end()) {
		num2_reg = global_reg_tab[var_2];
	}
#endif
	// ���� lw
	else {
		var_adr var_2_adr = find_var(var_2);
		mips_stream << "\tlw $t2, " << var_2_adr.offset << "(" << var_2_adr.reg << ")" << endl;
	}
	
	// ����
	mips_stream << "\t" << op << " $t0, " << num1_reg << ", " << num2_reg << endl;
	// ����
	mips_stream << "\tsw $t0, " << left_adr.offset << "(" << left_adr.reg << ")" << endl;
}

void condition_m(string op) {
	// ������1����num1_reg��������2����num2_reg
	string var_1 = code[code_no].num1;
	string var_2 = code[code_no].num2;

	// ������1����num1_reg
	string num1_reg = "$t1";
	if(is_int(var_1)) {
		mips_stream << "\tli $t1, " << var_1 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_1) != global_reg_tab.end()) {
		num1_reg = global_reg_tab[var_1];
	}
#endif
	// ���� lw
	else {
		var_adr var_1_adr = find_var(var_1);
		mips_stream << "\tlw $t1, " << var_1_adr.offset << "(" << var_1_adr.reg << ")" << endl;
	}

	// ������2����num2_reg
	string num2_reg = "$t2";
	if(is_int(var_2)) {
		mips_stream << "\tli $t2, " << var_2 << endl;
	}
#if OPTM == 1
	else if(global_reg_tab.find(var_2) != global_reg_tab.end()) {
		num2_reg = global_reg_tab[var_2];
	}
#endif
	// ���� lw
	else {
		var_adr var_2_adr = find_var(var_2);
		mips_stream << "\tlw $t2, " << var_2_adr.offset << "(" << var_2_adr.reg << ")" << endl;
	}

	// �Ƚ� beg $t1, $t2, label
	mips_stream << "\t" << op << " " << num1_reg << ", " << num2_reg << ", " << code[code_no].res << endl;
	//mips_stream << "\tnop" << endl;
}

void jmp_m() {
	// ��������ת
	mips_stream << "\tj " << code[code_no].res << endl;
	//mips_stream << "\tnop" << endl;
}

void label_m() {
	// ���ǩ
	mips_stream << code[code_no].res << ":" << endl;
}

void scanf_m() {
	// Ϊ$v0��ϵͳ���úţ�intΪ5��charΪ12
	if(code[code_no].num2 == "int") {
		mips_stream << "\tli $v0, 5" << endl;
	}
	else {
		mips_stream << "\tli $v0, 12" << endl;
	}

	mips_stream << "\tsyscall" << endl;
#if OPTM == 0
	var_adr read_adr = find_var(code[code_no].res);
	// $v0�е�ֵд�������ַ
	mips_stream << "\tsw $v0, " << read_adr.offset << "(" << read_adr.reg << ")" << endl;
#else
	if(global_reg_tab.find(code[code_no].res) != global_reg_tab.end()) {
		mips_stream << "\tmove " << global_reg_tab[code[code_no].res] << ", $v0" << endl;
	}
	else {
		var_adr read_adr = find_var(code[code_no].res);
		// $v0�е�ֵд�������ַ
		mips_stream << "\tsw $v0, " << read_adr.offset << "(" << read_adr.reg << ")" << endl;
	}
#endif
}

void printf_m() {
	// ��ӡ�ַ���
	if(code[code_no].num1 != "") {
		// ȡ�ַ����׵�ַ��$a0
		mips_stream << "\tla $a0, " << code[code_no].num1 << endl;
		// print string ϵͳ���ú�Ϊ4
		mips_stream << "\tli $v0, 4" << endl;
		mips_stream << "\tsyscall" << endl;
	}
	// ��ӡ���ʽ
	if(code[code_no].num2 != "") {
		// Ϊ$v0��ϵͳ���úţ�intΪ1��charΪ11
		if(code[code_no].res == "int") {
			mips_stream << "\tli $v0, 1" << endl;
		}
		else {
			mips_stream << "\tli $v0, 11" << endl;
		}
		// Ҫ��ӡ��ֵ����$a0
		// ����
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
	// ����������
	mips_stream << "\tli $v0, 11" << endl;
	mips_stream << "\tli $a0, 10" << endl;
	mips_stream << "\tsyscall" << endl;
}

void exit_m() {
	// �˳�����ϵͳ���ú�10
	mips_stream << "\tli $v0, 10" << endl;
	mips_stream << "\tsyscall" << endl;
}

bool is_int(string str) {
	if(str[0] == '-' || str[0] >= '0' && str[0] <= '9') {
		return true;
	}
	return false;
}
