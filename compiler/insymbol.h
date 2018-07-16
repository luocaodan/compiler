#pragma once
#include "stdafx.h"

enum symbol {
	intcon, charcon, stringcon,							// const
	add, sub, times, idiv, becomes,						// +-*/=
	eql, neq, gtr, geq, lss, leq,						// == != > >= < <=
	lparent, rparent, lbrack, rbrack, lbrace, rbrace,   // ( ) [ ] { }
	comma, semicolon, colon,							// , ; :
	voidsy, constsy, intsy, charsy, mainsy, returnsy,	// void const int char main main return
	ifsy, elsesy, whilesy, switchsy, casesy, defaultsy, // if else while switch case default
	scanfsy, printfsy,									// scanf printf
	ident,
	endsy,                                               // EOF
	errorsy
};

extern ifstream fin;
extern pair<string, symbol> cur_sym;

void sym_setup();
char nextch();
void ungetch(char ch);
void getsym();
void reset(streamoff index);
void ident_sym();
void intcon_sym();
void strcon_sym();
void charcon_sym();
void op_sym();
void error(string msg);
void print_sym();
string id_to_lower(string id);