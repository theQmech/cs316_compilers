#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"
#include "parser.h"
#include "reg-alloc.hh"

void Procedure::compile(){
	local_symbol_table.assign_offsets();
	sequence_ast->compile();
}

void Procedure::print_icode(ostream & file_buffer){
	printf("Procedure::print_icode not defined\n");
}

void Procedure::print_assembly(ostream & file_buffer){
	print_prologue(file_buffer);
	// Print the procedure
	sequence_ast->print_assembly(file_buffer);
	file_buffer<<"\tj epilogue_"<<name<<endl;
	print_epilogue(file_buffer);
}

void Procedure::print_prologue(ostream & file_buffer){
	file_buffer<<"\n\t.text \t\t\t# The .text assembler directive indicates\n";
	file_buffer<<"\t.globl "<<name<<"\t\t# The following is the code (as oppose to data)\n";
	file_buffer<<name<<":\t\t\t\t# .globl makes main know to the \n";
	file_buffer<<"\t\t\t\t# outside of the program.\n";
	file_buffer<<"# Prologue begins \n";
	file_buffer<<"\tsw $ra, 0($sp)\t\t# Save the return address\n";
	file_buffer<<"\tsw $fp, -4($sp)\t\t# Save the frame pointer\n";
	file_buffer<<"\tsub $fp, $sp, 8\t\t# Update the frame pointer\n";
	// Print the local variable in stack
	file_buffer<<"\n";
	// if (name == "main")
	// 	file_buffer<<"\tsub $sp, $sp, "<<8+local_symbol_table.get_size()<<endl;
	// else
	file_buffer<<"\tsub $sp, $sp, "<<8+local_symbol_table.get_size()<<"\t\t# Make space for the locals"<<endl;
	file_buffer<<"# Prologue ends\n\n";
}

void Procedure::print_epilogue(ostream & file_buffer){
	file_buffer<<"\n# Epilogue Begins\n";
	file_buffer<<"epilogue_"<<name<<":\n";
	// if (name == "main")
	// 	file_buffer<<"\tadd $sp, $sp, "<<8+local_symbol_table.get_size()<<endl;
	// else
	file_buffer<<"\tadd $sp, $sp, "<<8+local_symbol_table.get_size()<<endl;
	file_buffer<<"\tlw $fp, -4($sp)  \n";
	file_buffer<<"\tlw $ra, 0($sp)   \n";
	file_buffer<<"\tjr        $31\t\t# Jump back to the called procedure\n";
	file_buffer<<"# Epilogue Ends\n";
}