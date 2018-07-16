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
	string reg;				// ��ַ�Ĵ�����ȫ�ֱ���Ϊ$gp���ֲ�����Ϊ$fp)
	int offset;				// ��Ի�ַ��ƫ�ƣ�ȫ�ֱ���Ϊ�����ֲ�����Ϊ����
};

var_adr find_var(string name);
void insert_global_var(string name, int size);
void insert_var_info(string name, int size);

void data_segment();			// �������ݶΣ�ȫ�ֱ������ַ�����
void text_segment();			// ���������
void assemble();				// ������ԪʽΪmips

// ����ÿ����Ԫʽ����дһ���������䷭��Ϊmips

void var_m();					// ����
void array_m();					// ����
void func_m();					// �������忪ʼ
void para_m();					// ��������
void end_m();					// �����������

void call_m();					// ��������
void valpara_m();				// ����ֵ����
void return_m();				// �������

void assign_m();				// ��ͨ��ֵ
void assign_to_var_m();			// ����Ԫ�ظ�ֵ������
void assign_to_array_m();		// ������ֵ������Ԫ��

void op_m(string op);			// ��������
void condition_m(string op);	// ������ת
void jmp_m();					// ��������ת
void label_m();					// ���ǩ

void scanf_m();					// �����
void printf_m();				// д���

void exit_m();					// �˳�����
bool is_int(string str);		// ����Ƿ���int����