#include<iostream>
#include<fstream>
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"procedure.hh"
#include"program.hh"

Ast::Ast()
{}

int Ast::labelCounter=0;
int Ast::optimize_flag=0;

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

bool Ast::is_value_zero()
{
	stringstream msg;
	msg << "No is_value_zero() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_data_type(Data_Type dt)
{
	stringstream msg;
	msg << "No set_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	//ADD CODE HERE
	lhs = temp_lhs;
	rhs = temp_rhs;
	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	//ADD CODE HERE
	delete lhs;
	delete rhs;
}

bool Assignment_Ast::check_ast()
{
	if (ast_num_child==binary_arity || ast_num_child==unary_arity)
		CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");
	if (ast_num_child==binary_arity)
		CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");

	// use typeid(), get_data_type()
	//ADD CODE HERE
	Data_Type l = lhs->get_data_type();
	Data_Type r = rhs->get_data_type();

	CHECK_INPUT((l==r), "Assignment statement data type not compatible", lineno);

	node_data_type = l;
	return true;

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible", lineno);
}

void Assignment_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer <<"\n" << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	variable_symbol_entry = &var_entry;
	node_data_type = var_entry.get_data_type();

	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");
	//ADD CODE HERE
	lineno = line;

}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	// refer to functions for Symbol_Table_Entry 
	//ADD CODE HERE

	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	//ADD CODE HERE
	return *variable_symbol_entry;
}

void Name_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	variable_symbol_entry->set_data_type(dt);
}

void Name_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer << "Name : " << variable_symbol_entry->get_variable_name();
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	// use Ast_arity from ast.hh
	//ADD CODE HERE
	constant = number;
	node_data_type = constant_data_type;
	ast_num_child = zero_arity;
	lineno = line;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	//ADD CODE HERE
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

template <class DATA_TYPE>
bool Number_Ast<DATA_TYPE>::is_value_zero()
{
	//ADD CODE HERE
	return (constant == 0);
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer << "Num : " << std::fixed << std::setprecision(2) << constant;
}

///////////////////////////////////////////////////////////////////////////////

Arithmetic_Expr_Ast::~Arithmetic_Expr_Ast(){
	if (!lhs)
		delete lhs;
	if (!rhs)
		delete rhs;
}


Data_Type Arithmetic_Expr_Ast::get_data_type()
{
	//ABoD CODE HERE
	return node_data_type;
}

void Arithmetic_Expr_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

bool Arithmetic_Expr_Ast::check_ast()
{
	// use get_data_type(), typeid()
	//ADD CODE HERE

	if (ast_num_child==zero_arity){
		return true;
	}
	if (ast_num_child==unary_arity){
		CHECK_INVARIANT((lhs != NULL), "Lhs of Arithmetic_Expr_Ast cannot be null");
		node_data_type = lhs->get_data_type();
		return true;
	}
	if (ast_num_child==binary_arity){
		CHECK_INPUT((lhs->get_data_type()==rhs->get_data_type()), "Arithmetic statement data type not compatible", lineno);
		CHECK_INVARIANT((lhs != NULL), "Lhs of Arithmetic_Expr_Ast cannot be null");
		CHECK_INVARIANT((rhs != NULL), "Rhs of Arithmetic_Expr_Ast cannot be null");
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Arithmetic statement data type not compatible", lineno);
}

/////////////////////////////////////////////////////////////////////

Conditional_Operator_Ast::Conditional_Operator_Ast(Ast* cond, Ast* l, Ast* r, int line):
cond(cond){
	lhs = l;
	rhs = r;
	lineno = line;
	node_data_type = l->get_data_type();
	ast_num_child = binary_arity;
}
Conditional_Operator_Ast::~Conditional_Operator_Ast(){
	if (!cond)
		delete cond;
	// if (!lhs)
	// 	delete lhs;
	// if (!rhs)
	// 	delete rhs;
}

void Conditional_Operator_Ast::print(ostream & file_buffer){
	file_buffer<<"\n"<<AST_NODE_SPACE<<"Arith: Conditional\n";

	file_buffer<<AST_SUB_NODE_SPACE<<"COND (";
	cond->print(file_buffer);
	file_buffer<<")\n";

	file_buffer<<AST_SUB_NODE_SPACE<<"LHS (";
	lhs->print(file_buffer);
	file_buffer<<")\n";

	file_buffer<<AST_SUB_NODE_SPACE<<"RHS (";
	rhs->print(file_buffer);
	file_buffer<<")";

}

/////////////////////////////////////////////////////////////////////

Plus_Ast::Plus_Ast(Ast * l, Ast * r, int line)
{
	// set arity and data type
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
	lineno = line;
}

void Plus_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer <<"\n" << AST_NODE_SPACE << "Arith: PLUS\n";

	file_buffer << AST_SUB_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_SUB_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

/////////////////////////////////////////////////////////////////

Minus_Ast::Minus_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
	lineno = line;
}

void Minus_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer <<"\n" << AST_NODE_SPACE << "Arith: MINUS\n";

	file_buffer << AST_SUB_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_SUB_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

//////////////////////////////////////////////////////////////////

Mult_Ast::Mult_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
	lineno = line;
}

void Mult_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer <<"\n" << AST_NODE_SPACE << "Arith: MULT\n";

	file_buffer << AST_SUB_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_SUB_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

////////////////////////////////////////////////////////////////////

Divide_Ast::Divide_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
	lineno = line;
}

void Divide_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer <<"\n" << AST_NODE_SPACE << "Arith: DIV\n";

	file_buffer << AST_SUB_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_SUB_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

//////////////////////////////////////////////////////////////////////

UMinus_Ast::UMinus_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = unary_arity;
	lineno = line;
}

void UMinus_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer <<"\n" << AST_NODE_SPACE << "Arith: UMINUS\n";

	file_buffer << AST_SUB_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";
}

///////////////////////////////////////////////////////////////////////////////

Relational_Expr_Ast::Relational_Expr_Ast(Ast * lhs, Relational_Op rop, Ast * rhs, int line):
lhs_condition(lhs),rhs_condition(rhs),rel_op(rop){
	lineno = line;
	node_data_type = int_data_type;
	ast_num_child = binary_arity;
}

Relational_Expr_Ast::~Relational_Expr_Ast(){
	if (lhs_condition)
		delete lhs_condition;
	if (rhs_condition)
		delete rhs_condition;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	//ADD CODE HERE
	return node_data_type;
}

void Relational_Expr_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

bool Relational_Expr_Ast::check_ast()
{
	// use get_data_type(), typeid()
	//ADD CODE HERE

	if (ast_num_child==binary_arity){
		CHECK_INPUT((lhs_condition->get_data_type()==rhs_condition->get_data_type()), "Relational statement data type not compatible", lineno);
		CHECK_INVARIANT((lhs_condition != NULL), "Lhs of Relational_Expr_Ast cannot be null");
		CHECK_INVARIANT((rhs_condition != NULL), "Rhs of Relational_Expr_Ast cannot be null");
		return true;
	}

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Relational statement data type not compatible", lineno);
}

void Relational_Expr_Ast::print(ostream &file_buffer){
	string D[] = {"LE", "LT", "GT", "GE", "EQ", "NE"};
	file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: "<<D[(int)rel_op]<<"\n";

	file_buffer<<AST_SUB_NODE_SPACE<<"LHS (";
	lhs_condition->print(file_buffer);
	file_buffer<<")\n";

	file_buffer<<AST_SUB_NODE_SPACE<<"RHS (";
	rhs_condition->print(file_buffer);
	file_buffer<<")";

}

/////////////////////////////////////////////////////////////////////

Boolean_Expr_Ast::Boolean_Expr_Ast(Ast * lhs, Boolean_Op bop, Ast * rhs, int line):
lhs_op(lhs),rhs_op(rhs),bool_op(bop){
	lineno = line;
	node_data_type = int_data_type;
	if (lhs && rhs)
		ast_num_child=binary_arity;
	else
		ast_num_child=unary_arity;
}

Boolean_Expr_Ast::~Boolean_Expr_Ast(){
	if (lhs_op)
		delete lhs_op;
	if (rhs_op)
		delete rhs_op;
}

Data_Type Boolean_Expr_Ast::get_data_type()
{
	//ADD CODE HERE
	return node_data_type;
}

void Boolean_Expr_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

bool Boolean_Expr_Ast::check_ast()
{
	// use get_data_type(), typeid()
	//ADD CODE HERE

	if (ast_num_child==unary_arity){
		CHECK_INVARIANT((rhs_op != NULL), "Rhs of Boolean_Expr_Ast cannot be null");
		return true;
	}
	if (ast_num_child==binary_arity){
		CHECK_INPUT((lhs_op->get_data_type()==rhs_op->get_data_type()), "Boolean statement data type not compatible", lineno);
		CHECK_INVARIANT((lhs_op != NULL), "Lhs of Boolean_Expr_Ast cannot be null");
		CHECK_INVARIANT((rhs_op != NULL), "Rhs of Boolean_Expr_Ast cannot be null");
		return true;
	}

	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Boolean statement data type not compatible", lineno);
}

void Boolean_Expr_Ast::print(ostream &file_buffer){
	string D[] = {"NOT", "OR", "AND"};
	file_buffer<<"\n"<<AST_NODE_SPACE<<"Condition: "<<D[bool_op]<<"\n";

	if (lhs_op){
		file_buffer<<AST_SUB_NODE_SPACE<<"LHS (";
		lhs_op->print(file_buffer);
		file_buffer<<")\n";
	}

	if (rhs_op){
		file_buffer<<AST_SUB_NODE_SPACE<<"RHS (";
		rhs_op->print(file_buffer);
		file_buffer<<")";
	}

}

/////////////////////////////////////////////////////////////////////

Iteration_Statement_Ast::Iteration_Statement_Ast(Ast * cond, Ast* body, int line, bool do_form):
cond(cond),body(body){
	is_do_form = do_form;
	lineno = line;
}

void Iteration_Statement_Ast::print(ostream & file_buffer){
	file_buffer<<"\n";

	if (is_do_form){
		file_buffer<<AST_SPACE<<"DO (";
		body->print(file_buffer);
		file_buffer<<")\n";

		file_buffer<<AST_SPACE<<"WHILE CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")";
	}
	else{
		file_buffer<<AST_SPACE<<"WHILE : \n";

		file_buffer<<AST_SPACE<<"CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")\n";

		file_buffer<<AST_SPACE<<"BODY (";
		body->print(file_buffer);
		file_buffer<<")";
	}

}

Iteration_Statement_Ast::~Iteration_Statement_Ast(){
	if (!cond)
		delete cond;
	if (!body)
		delete body;
}

Data_Type Iteration_Statement_Ast::get_data_type(){
	return node_data_type;
}
void Iteration_Statement_Ast::set_data_type(Data_Type dt){
	node_data_type = dt;
}
bool Iteration_Statement_Ast::check_ast(){
	return (cond && body);
}

/////////////////////////////////////////////////////////////////////

Selection_Statement_Ast::Selection_Statement_Ast(Ast * cond,Ast* then_part, Ast* else_part, int line):
cond(cond),then_part(then_part),else_part(else_part){
	lineno = line;
}

Selection_Statement_Ast::~Selection_Statement_Ast(){
	if (!cond)
		delete cond;
	if (!then_part)
		delete then_part;
	if (!else_part)
		delete else_part;
}

Data_Type Selection_Statement_Ast::get_data_type(){
	return node_data_type;
}
void Selection_Statement_Ast::set_data_type(Data_Type dt){
	node_data_type = dt;
}

bool Selection_Statement_Ast::check_ast(){
	return (cond && then_part);
}

void Selection_Statement_Ast::print(ostream & file_buffer){
	file_buffer<<"\n"<<AST_SPACE<<"IF : ";
	file_buffer<<"\n"<<AST_SPACE<<"CONDITION (";
	cond->print(file_buffer);
	file_buffer<<")\n";

	file_buffer<<AST_SPACE<<"THEN (";
	then_part->print(file_buffer);
	file_buffer<<")\n"<<AST_SPACE<<"ELSE (";
	else_part->print(file_buffer);
	file_buffer<<")";
}

/////////////////////////////////////////////////////////////////////

Sequence_Ast::Sequence_Ast(int line)
{
	lineno = line;
}

Sequence_Ast::~Sequence_Ast()
{
}
void Sequence_Ast::ast_push_back(Ast * ast){
	statement_list.push_back(ast);
}
void Sequence_Ast::print(ostream & file_buffer){
	file_buffer<<"\n"<<SA_SPACE<<"Sequence Ast:\n";
	for (list<Ast*>::iterator it=statement_list.begin(); it!=statement_list.end(); ++it){
		(*it)->print(file_buffer);
	}
}

/////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(Ast * temp_return, int line){
	lineno = line;
	return_variable = temp_return;
	if (return_variable == NULL){
		ast_num_child = zero_arity;
		node_data_type = void_data_type;
	}
	else{
		ast_num_child = unary_arity;
		node_data_type = return_variable->get_data_type();
	}
}

void Return_Ast::set_return_ast(Ast * temp_return){
	return_variable = temp_return;
	if (return_variable == NULL)
		node_data_type = void_data_type;
	else
		node_data_type = return_variable->get_data_type();
}

Ast * Return_Ast::get_return_ast(){
	return return_variable;
}

Return_Ast::~Return_Ast(){}

void Return_Ast::print(ostream & file_buffer){
	printf("Return_Ast::print not defined\n");
}

/////////////////////////////////////////////////////////////////////

template class Number_Ast<double>;
template class Number_Ast<int>;
