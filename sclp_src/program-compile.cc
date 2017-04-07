#include <iostream>
#include <fstream>
#include <typeinfo>

using namespace std;

#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "icode.hh"
#include "reg-alloc.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"

// compile
void Program::compile(){
	machine_desc_object.initialize_instruction_table();
	machine_desc_object.initialize_register_table();

	global_symbol_table.assign_offsets();

	list<Symbol_Table_Entry *> global_list = global_symbol_table.get_table();
	for (list<Symbol_Table_Entry *>::iterator it = global_list.begin();
		it!=global_list.end(); ++it){
		(*it)->set_symbol_scope(global);
		if((*it)->get_data_type()==func_data_type && (*it)->get_proc()!=NULL)
			(*it)->get_proc()->compile();
	}
}

void Program::print_assembly(){

	ostream & file_buffer = command_options.get_output_buffer();

	file_buffer<<"\n\t.data"<<endl;
	list<Symbol_Table_Entry *> global_list = global_symbol_table.get_table();
	for (list<Symbol_Table_Entry *>::iterator it = global_list.begin();
		it!=global_list.end(); ++it){
		if((*it)->get_data_type()==int_data_type)
			file_buffer<<(*it)->get_variable_name()<<":\t.word 0\n";
		if((*it)->get_data_type()==double_data_type)
			file_buffer<<(*it)->get_variable_name()<<":\t.space 8\n";
	}

	for (map<string, Procedure *>::iterator it = proc_map.begin();
		it != proc_map.end(); ++it){
		it->second->print_assembly(file_buffer);
		cout<<endl;
	}
}