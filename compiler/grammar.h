#pragma once
#include "stdafx.h"
#include "error.h"
#include "table.h"
#include "insymbol.h"
#include "midcode.h"
#include <limits>

extern table sym_tab;				// ���ű�
extern map<string, string> str_tab;	// �ַ���������
extern ofstream grammar_stream;		// �﷨���������
extern bool grammar_flag;			// Ϊtrue���ӡ�﷨�������

void program();																	// ����
void const_decl(int flag);														// ����˵�� flag 1 Ϊȫ�� 0 Ϊ�ֲ�
void var_decl(int flag);														// ����˵�������ؿ��ܶ����ĺ�����Ϣ flag 1 Ϊȫ�� 0 Ϊ�ֲ�
void func_def();																// �з���ֵ��������
void proc_def();																// �޷���ֵ��������
void main_func();																// ������
void const_def(int flag);														// ��������
int constant_liter(int sign);													// ������
void var_def(int flag);															// ��������
string decl_head();																// ����ͷ��
void parameters();																// ������
void compound_stat();															// �������
void stat_column();																// �����
void statements();																// ���
void if_stat();																	// if���
void while_stat();																// while���
string condition();																// ����
void scanf_stat();																// �����
void printf_stat();																// д���
void case_stat();																// switch ���
void case_label(string expr_var, string switch_end);							// �����
void one_case(string expr_var, string switch_end);								// ��������
void default_stat(string switch_end);											// ȱʡ
void return_stat();																// �������
string call(bool factor_flag);													// �������
void assign_stat();																// ��ֵ���
void value_para(string func_name);												// ֵ������
void expression();																// ���ʽ
void term();																	// ��
void factor();																	// ����