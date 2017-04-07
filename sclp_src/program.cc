#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"
#include "parser.h"
#include "reg-alloc.hh"

Program program_object;

Program::Program(){
}

Program::~Program(){
}

void Program::delete_all(){
	delete procedure;
	// for(map<string, Procedure *>::iterator it = proc_map.begin();
	// 	it!=proc_map.end(); ++it){
	// 	delete it->second;
	// }
}

void Program::set_procedure(Procedure * proc, int line){
	if (procedure != NULL)
		delete procedure;
	procedure = proc;
}

void Program::set_global_table(Symbol_Table & new_global_table){
	global_symbol_table = new_global_table;
}

void Program::insert_proc_in_map(Procedure * proc){
	CHECK_INVARIANT(proc!=NULL, "Program::insert_proc_in_map proc should not be null\n");
	proc_map[proc->get_proc_name()] = proc;
}

Symbol_Table_Entry & Program::get_symbol_table_entry(string variable){
	return global_symbol_table.get_symbol_table_entry(variable);
}

void Program::print_sym(){
	printf("Program::print_sym not defined yet\n");
}

void Program::print(){
	printf("Program::print not defined yet\n");
}

bool Program::variable_proc_name_check(string symbol){
	list<Symbol_Table_Entry *> global_list = global_symbol_table.get_table();
	for (list<Symbol_Table_Entry *>::iterator it = global_list.begin();
		it!=global_list.end(); ++it){
		if((*it)->get_data_type()==func_data_type && (*it)->get_proc()!=NULL){
			if ((*it)->get_variable_name() == symbol)
				return true;
		}
	}
	return false;
}

bool Program::variable_in_proc_map_check(string symbol){
	return (proc_map.find(symbol) != proc_map.end());
}

bool Program::variable_in_symbol_list_check(string variable){
	return global_symbol_table.variable_in_symbol_list_check(variable);
}

void Program::global_list_in_proc_check(){
	// printf("Program::global_list_in_proc_check not defined yet\n");
}

Procedure * Program::get_proc(string name){
	if (proc_map.find(name)!=proc_map.end())
		return proc_map[name];
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Program::get_proc() proc must exist\n");
}
