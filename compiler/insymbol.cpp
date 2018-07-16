#include "stdafx.h"
#include "insymbol.h"
#include "error.h"



ifstream fin;
pair<string, symbol> cur_sym;

map<string, symbol> key;

map<char, symbol> spss;

string symbol_print[39] = {
	"intcon", "charcon", "strcon",
	"plus", "minus", "times", "idiv", "becomes",
	"eql", "neq", "gtr", "geq", "lss", "leq",
	"lparent", "rparent", "lbrack", "rbrack", "lbrace", "rbrace",
	"comma", "semicolon", "colon",
	"voidsy", "constsy", "intsy", "charsy", "mainsy", "returnsy",
	"ifsy", "elsesy", "whilesy", "switchsy", "casesy", "defaultsy",
	"scanfsy", "printfsy",
	"ident",
	"endsy"
};

void sym_setup() {
	key.insert(make_pair("void", voidsy));
	key.insert(make_pair("const", constsy));
	key.insert(make_pair("int", intsy));
	key.insert(make_pair("char", charsy));
	key.insert(make_pair("if", ifsy));
	key.insert(make_pair("else", elsesy));
	key.insert(make_pair("while", whilesy));
	key.insert(make_pair("switch", switchsy));
	key.insert(make_pair("case", casesy));
	key.insert(make_pair("default", defaultsy));
	key.insert(make_pair("main", mainsy));
	key.insert(make_pair("return", returnsy));
	key.insert(make_pair("scanf", scanfsy));
	key.insert(make_pair("printf", printfsy));

	spss.insert(make_pair('+', add));
	spss.insert(make_pair('-', sub));
	spss.insert(make_pair('*', times));
	spss.insert(make_pair('/', idiv));
	spss.insert(make_pair('(', lparent));
	spss.insert(make_pair(')', rparent));
	spss.insert(make_pair('[', lbrack));
	spss.insert(make_pair(']', rbrack));
	spss.insert(make_pair('{', lbrace));
	spss.insert(make_pair('}', rbrace));
	spss.insert(make_pair(',', comma));
	spss.insert(make_pair(';', semicolon));
	spss.insert(make_pair(':', colon));
}

char nextch() {  // 读到文件尾返回\0
	char ch = '\0';
	fin.get(ch);
	if(ch == '\t') {
		cc += 4;
	}
	else {
		cc++;
	}
	return ch;
}

void ungetch(char ch) {
	fin.putback(ch);
	if(ch == '\t') {
		cc -= 4;
	}
	else {
		cc--;
	}
}

void reset(streamoff index) {
	fin.seekg(index, fin.beg);
}

void getsym() {
	char ch = nextch();
	
	while (ch == ' ' || ch == '\t' || ch == '\n') {
		if(ch == '\n') {
			ll++;
			cc = -1;
			errpos = 0;
		}
		ch = nextch();
	}

	if (ch == '\0') {
		cur_sym = make_pair("", endsy);
	}

	else if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_') {
		ungetch(ch);
		ident_sym();
	}
	else if (ch >= '0' && ch <= '9') {
		ungetch(ch);
		intcon_sym();
	}
	else if (ch == '\'') {
		charcon_sym();
	}
	else if (ch == '"') {
		strcon_sym();
	}
	else if (ch == '<' || ch == '>' || ch == '!' || ch == '=') {
		ungetch(ch);
		op_sym();
	}
	else if (spss.find(ch) != spss.end()) {
		string id;
		id.push_back(ch);
		cur_sym = make_pair(id, spss[ch]);
	}
	else {
		errpos = cc;
		error(Unknow_char);
		cur_sym = make_pair("", errorsy);
	}
}

void ident_sym() {
	string id;
	symbol sym;
	char temp = nextch();
	while (temp >= 'a' && temp <= 'z' || temp >= 'A' && temp <= 'Z' || temp >= '0' && temp <= '9' || temp == '_') {
		id.push_back(temp);
		temp = nextch();
	}
	ungetch(temp);
	string id_lower = id_to_lower(id);
	if (key.find(id_lower) == key.end()) {
		sym = ident;
	}
	else {
		sym = key[id_lower];
	}
	cur_sym = make_pair(id_lower, sym);
}

void intcon_sym() {
	string id;
	symbol sym = intcon;
	char temp = nextch();
	
	while (temp >= '0' && temp <= '9') {
		id.push_back(temp);
		temp = nextch();
	}
	ungetch(temp);
	cur_sym = make_pair(id, sym);
}

void charcon_sym() {
	string id;
	symbol sym = charcon;
	char temp = nextch();
	id.push_back(temp);
	temp = nextch();
	if (temp != '\'') {
		errpos = cc - 2;
		while (temp != '\'' && temp != '\n' && temp != ';' && temp != ',' && fin) {
			temp = nextch();
		}
		if (temp != '\'') {
			error(Miss_single_quote);
			ungetch(temp);
		}
		else {
			error(Charcon_too_much_char);
		}
	}
	cur_sym = make_pair(id, sym);
}

void strcon_sym() {
	string id;
	symbol sym = stringcon;
	char temp = nextch();
	while (temp >= 32 && temp <= 126 && temp != 34) {
		id.push_back(temp);
		temp = nextch();
	}
	if (temp != '"') {
		errpos = cc - id.size() - 1;
		error(Miss_double_quote);
		ungetch(temp);
	}
	if(id.size() > 512) {
		errpos = cc - id.size() - 1;
		error(Str_const_too_long);
	}
	cur_sym = make_pair(id, sym);
}

void op_sym() {
	string id;
	symbol sym;
	char temp = nextch();
	id.push_back(temp);
	if (temp == '!') {
		temp = nextch();
		if (temp != '=') {
			errpos = cc - 1;
			ungetch(temp);
			error(Unknow_char);
			sym = errorsy;
		}
		else {
			id.push_back('=');
			sym = neq;
		}
	}
	else if (temp == '<') {
		temp = nextch();
		if (temp == '=') {
			id.push_back('=');
			sym = leq;
		}
		else {
			ungetch(temp);
			sym = lss;
		}
	}
	else if (temp == '>') {
		temp = nextch();
		if (temp == '=') {
			id.push_back('=');
			sym = geq;
		}
		else {
			ungetch(temp);
			sym = gtr;
		}
	}
	else {
		temp = nextch();
		if (temp == '=') {
			id.push_back('=');
			sym = eql;
		}
		else {
			ungetch(temp);
			sym = becomes;
		}
	}
	cur_sym = make_pair(id, sym);
}

void print_sym() {
	static int count = 1;
	cout << count << " " << symbol_print[cur_sym.second] << " " << cur_sym.first<< endl;
	count++;
}

string id_to_lower(string id) {
	string id_lower;
	for (string::iterator it = id.begin(); it != id.end(); it++) {
		if (*it >= 'A' && *it <= 'Z') {
			id_lower.push_back(tolower(*it));
		}
		else {
			id_lower.push_back(*it);
		}
	}
	return id_lower;
}