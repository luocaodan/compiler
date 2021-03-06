// compiler.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "insymbol.h"
#include "error.h"
#include "table.h"
#include "grammar.h"
#include "midcode.h"
#include "mips.h"
#include "optimize.h"

int main()
{
	string filename;
	cout << "input source file name(no space):";
	cin >> filename;
	fin.open(filename);
	midcode_stream.open("midcode.txt");
	mips_stream.open("mips.asm");

	if (!fin) {
		cout << "can't open file: " << filename << endl;
		system("pause");
		return 1;
	}

	streambuf *cout_buf = cout.rdbuf();
	streambuf *grammar_buf = grammar_stream.rdbuf();
	streambuf *error_buf = error_stream.rdbuf();

	sym_setup();
	program();

	if(err_flag) {
		error_stream.set_rdbuf(cout_buf);
		print_error();
		cout.flush();
		system("pause");
		return 1;
	}

#if OPTM == 1
	// 消除公共子表达式
	gen_opt_code();
	// 常量合并和传播
	const_combine();
#endif
	print_midcode();
	assemble();
	midcode_stream.close();
	mips_stream.close();
	fin.close();

	system("pause");
    return 0;
}