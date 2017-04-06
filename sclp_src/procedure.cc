#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"
#include "parser.h"
#include "reg-alloc.hh"

Procedure::Procedure(Data_Type proc_return_type, string proc_name, int line){
	return_type = proc_return_type;
	name = proc_name;
	lineno = line;
}

Procedure::~Procedure(){
	printf("Procedure::~Procedure not defined\n");
}

string Procedure::get_proc_name(){
	return name;
}

void Procedure::set_sequence_ast(Sequence_Ast & sa){
	sequence_ast = &sa;
}

void Procedure::set_local_list(Symbol_Table & new_list){
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);
}

Data_Type Procedure::get_return_type(){
	return return_type;
}

Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name){
	return local_symbol_table.get_symbol_table_entry(variable_name);
}

bool Procedure::match_prototype(Symbol_Table &new_sym_table){
	(local_symbol_table==new_sym_table);
	return true;
	printf("Procedure::match_prototype not defined yet\n");
}

void Procedure::push_symbol(Symbol_Table_Entry * variable){
	local_symbol_table.push_symbol(variable);
	return;
	printf("Procedure::push_symbol not defined yet\n");
}


void Procedure::print(ostream & file_buffer){
	printf("Procedure::print not defined\n");
}

void Procedure::print_sym(ostream & file_buffer){
	printf("Procedure::print_sym not defined\n");
}

bool Procedure::variable_in_symbol_list_check(string variable){
	return local_symbol_table.variable_in_symbol_list_check(variable);
}

// compile
void Procedure::compile(){
	printf("Procedure::compile not defined\n");
}

void Procedure::print_icode(ostream & file_buffer){
	printf("Procedure::print_icode not defined\n");
}

void Procedure::print_assembly(ostream & file_buffer){
	printf("Procedure::print_assembly not defined\n");
}

void Procedure::print_prologue(ostream & file_buffer){
	printf("Procedure::print_prologue not defined\n");
}

void Procedure::print_epilogue(ostream & file_buffer){
	printf("Procedure::print_epilogue not defined\n");
}
