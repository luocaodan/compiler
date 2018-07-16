#pragma once
#include "stdafx.h"
#include "midcode.h"
#include "grammar.h"
#include "optimize.h"

extern ofstream mips_stream;

struct var_info {
	string name;
	int offset;
};

struct var_adr {
	string reg;				// 基址寄存器（全局变量为$gp，局部变量为$fp)
	int offset;				// 相对基址的偏移（全局变量为正，局部变量为负）
};

var_adr find_var(string name);
void insert_global_var(string name, int size);
void insert_var_info(string name, int size);

void data_segment();			// 构建数据段（全局变量和字符串）
void text_segment();			// 构建代码段
void assemble();				// 翻译四元式为mips

// 对于每种四元式，编写一个函数将其翻译为mips

void var_m();					// 变量
void array_m();					// 数组
void func_m();					// 函数定义开始
void para_m();					// 函数参数
void end_m();					// 函数定义结束

void call_m();					// 函数调用
void valpara_m();				// 调用值参数
void return_m();				// 返回语句

void assign_m();				// 普通赋值
void assign_to_var_m();			// 数组元素赋值给变量
void assign_to_array_m();		// 变量赋值给数组元素

void op_m(string op);			// 四则运算
void condition_m(string op);	// 条件跳转
void jmp_m();					// 无条件跳转
void label_m();					// 打标签

void scanf_m();					// 读语句
void printf_m();				// 写语句

void exit_m();					// 退出程序
bool is_int(string str);		// 检查是否是int常量