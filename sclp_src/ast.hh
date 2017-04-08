#ifndef AST_HH
#define AST_HH

#include<string>
#include<iostream>
#include<iomanip>
#include<typeinfo>
#include<list>
#include<map>
#include<set>
#include<queue>

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define AST_SUB_NODE_SPACE "               "
#define SA_SPACE "      "

using namespace std;

#ifndef SPIM_HH
#define SPIM_HH
typedef enum
{
	none,	/* dummy to indicate no register */
	zero,	/* constant register */
	v0,	/* expression result register */
	v1,	/* function result register */
	a0,	/* argument register */
	a1, a2, a3,
	t0,      /* temporary caller-save registers */
	t1, t2, t3, t4, t5, t6, t7, t8, t9, 
	s0,	/* temporary callee-save registers */ 
	s1, s2, s3, s4, s5, s6, s7,
	mfc,	/* float register to int register */
	mtc,	/* int register to float register */
	f0, 	/* floating point registers */
	f2, f4, f6, f8,
	f10, f12, f14, f16, f18,
	f20, f22, f24, f26, f28, f30,
	gp,	/* global data pointer register */
	sp,	/* stack pointer register */
	fp,	/* frame pointer register */
	ra	/* return address register */
} Spim_Register;
#endif


class Ast;
class Block;
class CFG;

class Block
{
public:
	list<Icode_Stmt *> instructions;
	list<int> previous;
	list<int> next;
	set<Spim_Register> In, Out;

	Block();
	~Block();
	void add_stmt(Icode_Stmt * stmt);
	void add_prev(int id);
	void add_succ(int id);
	void eliminate(vector<Block *> nodes);
	int get_size();
};

class CFG
{
	vector<Block *> nodes;
public:
	CFG(list<Icode_Stmt *> inst);
	~CFG();
	Block * get_node(int n);
	void optimise();
	list<Icode_Stmt *> toInstructionList();
	void dead_code_eliminate();
};

class Ast
{
protected:
	typedef enum
	{
		zero_arity = 0,
		unary_arity = 1,
		binary_arity = 2,
		ternary_arity = 3
	}Ast_Arity;

	Data_Type node_data_type;
	Ast_Arity ast_num_child;
	static int labelCounter;
	static int stringLabelCounter;
	static int optimize_flag;
	int lineno;
	string get_new_label(){

		return "label"+to_string(labelCounter++);
	}

	string get_new_string_label(){

		return "string"+to_string(stringLabelCounter++);
	}

public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual void set_data_type(Data_Type dt);

	virtual bool is_value_zero();

	virtual bool check_ast();
	virtual Symbol_Table_Entry & get_symbol_entry();

	virtual void print(ostream & file_buffer) = 0;
	virtual Code_For_Ast & compile() = 0;
	virtual Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
	virtual void print_assembly();
	virtual void print_icode();

};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Assignment_Ast();

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Name_Ast:public Ast
{
	Symbol_Table_Entry * variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line);
	~Name_Ast();

	Data_Type get_data_type();
	Symbol_Table_Entry & get_symbol_entry();
	void set_data_type(Data_Type dt);

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type, int line);
	~Number_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);
	bool is_value_zero();

	void print(ostream & file_buffer);
	Code_For_Ast & compile();
};

class Relational_Expr_Ast:public Ast
{
	Ast * lhs_condition;
	Ast * rhs_condition;
	Relational_Op rel_op;

public:
	Relational_Expr_Ast(Ast * lhs, Relational_Op rop, Ast * rhs, int line);
	~Relational_Expr_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);
	Code_For_Ast & compile();
};

class Boolean_Expr_Ast:public Ast
{
	Ast * lhs_op;
	Ast * rhs_op;
	Boolean_Op bool_op;

public:
	Boolean_Expr_Ast(Ast * lhs, Boolean_Op bop, Ast * rhs, int line);
	~Boolean_Expr_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Selection_Statement_Ast: public Ast {
protected:
	Ast* cond;
	Ast* then_part;
	Ast* else_part;
public:
	Selection_Statement_Ast(Ast * cond,Ast* then_part, Ast* else_part, int line);
	~Selection_Statement_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Iteration_Statement_Ast: public Ast {
protected:
	Ast* cond;
	Ast* body;
	bool is_do_form;
public:
	Iteration_Statement_Ast(Ast * cond, Ast* body, int line, bool do_form);
	~Iteration_Statement_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Arithmetic_Expr_Ast:public Ast
{
protected:
	Ast * lhs;
	Ast * rhs;

public:
	Arithmetic_Expr_Ast() {}
	~Arithmetic_Expr_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);
	bool check_ast();

	virtual void print(ostream & file_buffer) = 0;
	virtual Code_For_Ast & compile() = 0;
};

class Plus_Ast:public Arithmetic_Expr_Ast
{
public:
	Plus_Ast(Ast * l, Ast * r, int line);
	~Plus_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Minus_Ast:public Arithmetic_Expr_Ast
{
public:
	Minus_Ast(Ast * l, Ast * r, int line);
	~Minus_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Divide_Ast:public Arithmetic_Expr_Ast
{
public:
	Divide_Ast(Ast * l, Ast * r, int line);
	~Divide_Ast();

	void print(ostream & file_buffer);
	Code_For_Ast & compile();
};

class Mult_Ast:public Arithmetic_Expr_Ast
{
public:
	Mult_Ast(Ast * l, Ast * r, int line);
	~Mult_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Conditional_Operator_Ast: public Arithmetic_Expr_Ast
{
protected:
	Ast* cond;
public:
	Conditional_Operator_Ast(Ast* cond, Ast* l, Ast* r, int line);
	~Conditional_Operator_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class UMinus_Ast: public Arithmetic_Expr_Ast
{
public:
	UMinus_Ast(Ast * l, Ast * r, int line);
	~UMinus_Ast();
	
	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Arith_Func_Call: public Arithmetic_Expr_Ast
{
public:
	Arith_Func_Call(Ast * l, Ast * r, int line);
	~Arith_Func_Call();
	
	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};


class Sequence_Ast: public Ast{
	list<Ast *> statement_list;
	list<Icode_Stmt *> sa_icode_list;
public:
	Sequence_Ast(int line);
	~Sequence_Ast();
	void ast_push_back(Ast * ast);
	void print(ostream & file_buffer);
	Code_For_Ast & compile();
	void print_assembly(ostream & file_buffer);
	void print_icode(ostream & file_buffer);
};

class Return_Ast: public Ast{
	Ast * return_variable;

public:
	Return_Ast(Ast * temp_return, int line);
	void set_return_ast(Ast * temp_return);
	Ast * get_return_ast();
	~Return_Ast();
	Data_Type get_data_type();
	void print(ostream & file_buffer);
	Code_For_Ast & compile();
	void print_assembly(ostream & file_buffer);
	void print_icode(ostream & file_buffer);
};

class Func_Call_Ast: public Ast{
	Procedure * proc;
	list<Ast *> * args;
public:
	Func_Call_Ast(Procedure * _proc, list<Ast*> * _args,int line);
	~Func_Call_Ast();
	int get_size_of_value_type(Data_Type dt);
	Data_Type get_data_type();
	void print(ostream & file_buffer);
	Code_For_Ast & compile();
	void print_assembly(ostream & file_buffer);
	void print_icode(ostream & file_buffer);
};

class Print_Ast: public Ast{
	Ast * expr;
public:
	Print_Ast(Ast * _expr, int line);
	~Print_Ast();
	void print(ostream & file_buffer);
	Code_For_Ast & compile();
	void print_assembly(ostream & file_buffer);
	void print_icode(ostream & file_buffer);
};

class String_Ast:public Ast
{
public:
	string mystring;
	string mylabel;
	String_Ast(string stringvalue, int line);
	~String_Ast();

	bool is_empty();
	Data_Type get_data_type();

	void print(ostream & file_buffer);
	void print_assembly(ostream & file_buffer);
	void print_icode(ostream & file_buffer);
	Code_For_Ast & compile();
};


#endif
