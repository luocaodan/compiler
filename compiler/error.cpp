#include "stdafx.h"
#include "error.h"

ofstream error_stream;
vector<compile_err> errs;
int ll = 1;
int cc = -1;
int errpos = 0;
bool err_flag = false;

void error(int n) {
	err_flag = true;
	compile_err err;
	err.line = ll;
	err.start = errpos;
	err.end = cc;
	err.err_code = n;
	errs.push_back(err);
}

void error1(int n) {
	err_flag = true;
	compile_err err;
	err.line = ll;
	err.start = cc - cur_sym.first.size() + 1;
	err.end = cc + 1;
	err.err_code = n;
	errs.push_back(err);
}

void print_error() {
	if(errs.size() != 0){
		for(size_t i = 0;i < errs.size();i++) {
			error_stream << "error:" << error_msg(errs[i].err_code) << " in line " << errs[i].line << " position " << errs[i].start+1 << endl;
		}
		error_stream << endl;
		errs.clear();
	}
}

string error_msg(int n) {
	string msg[60];

	msg[0] = "δ֪�ַ�";
	msg[1] = "���λ��ַ�����������Χ";
	msg[2] = "�ַ��������ַ�̫��";
	msg[3] = "ȱ���ҵ�����";
	msg[4] = "ȱ����˫����";
	msg[5] = "�ַ�����������";
	msg[6] = "ȱ��\"}\"";
	msg[7] = "��ʶ���ض���";
	msg[8] = "ȱ������˵����";
	msg[9] = "ȱ��\"]\"";
	msg[10] = "ȱ�������С";
	msg[11] = "��ʶ��δ����";
	msg[12] = "ȱ��\";\"";
	msg[13] = "ȱ��\"(\"";			 // if while ��������
	msg[14] = "ȱ���������ʽ";
	msg[15] = "ȱ��\")\"";
	msg[16] = "if ���ȱ�� else";
	msg[17] = "case ���ȱ��\":\"";
	msg[18] = "case �﷨����";
	msg[19] = "�Ƿ���default";
	msg[20] = "������������";
	msg[21] = "������������";
	msg[22] = "����û�з���ֵ";
	msg[23] = "void ������Ӧ�÷���ֵ";
	msg[24] = "ȱ��main���������δ���ķ���ʽ����";
	msg[25] = "�ڴ�һ����ʶ��";
	msg[26] = "��������ȱ�ٸ�ֵ����";
	msg[27] = "������ֵ����";
	msg[28] = "ȱ��\"{\"";
	msg[29] = "�������������";
	msg[30] = "���ͳ���������ǰ��0";
	msg[31] = "��ֵ����﷨����";
	msg[32] = "���Ӵ���";
	msg[33] = "���������﷨����";
	msg[34] = "�������޷���ֵ�����μ�����";
	msg[35] = "����δ����";
	msg[36] = "���Ͳ�ƥ��";

	return msg[n];
}

void skip(set<symbol> symset, int n) {
	error1(n);
	//getsym();
	while(symset.find(cur_sym.second) == symset.end() && cur_sym.second != endsy) {
		getsym();
	}
}

void skip(symbol expected, int n) {
	error1(n);
	//getsym();
	while(cur_sym.second != expected && cur_sym.second != endsy) {
		getsym();
	}
}

bool test(symbol expected, symbol des, int n) {
	if(cur_sym.second != expected) {
		skip(des, n);
		return false;
	}
	return true;
}

bool test(symbol expected, set<symbol> symset, int n) {
	if(cur_sym.second != expected) {
		getsym();
		skip(symset, n);
		return false;
	}
	return true;
}

bool test(set<symbol> expected_set, symbol des, int n) {
	if(expected_set.find(cur_sym.second)==expected_set.end()) {
		getsym();
		skip(des, n);
		return false;
	}
	return true;
}

bool test(set<symbol> set1, set<symbol> set2, int n) {
	if(set1.find(cur_sym.second)==set1.end()) {
		skip(set2, n);
		return false;
	}
	return true;
}