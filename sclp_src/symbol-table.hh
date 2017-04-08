#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <string>
#include <list>

using namespace std;

class Symbol_Table;
class Symbol_Table_Entry;

typedef enum
{
	void_data_type,
	int_data_type,
	double_data_type,
	string_data_type,
	func_data_type
} Data_Type;

typedef enum
{
	global,
	local,
	formal
} Table_Scope;

typedef enum
{
	less_equalto,
	less_than,
	greater_than,
	greater_equalto,
	equalto,
	not_equalto
} Relational_Op;

typedef enum
{
	boolean_not,
	boolean_or,
	boolean_and
} Boolean_Op;

// Compile
typedef enum
{
	fp_ref,
	sp_ref
} Offset_With_Ref_To;

//////////////////////////////////////////////////////////////

class Symbol_Table
{
	list<Symbol_Table_Entry *> variable_table;
	Table_Scope scope;

	// compile
	int size_in_bytes;		// size of list
	int start_offset_of_first_symbol;

public:
	Symbol_Table();
	~Symbol_Table();

	bool is_empty();
	void operator==(Symbol_Table & list);

	Table_Scope get_table_scope();
	void set_table_scope(Table_Scope list_scope);

	void print(ostream & file_buffer);

	void copyfrom(Symbol_Table * new_table);

	void push_symbol(Symbol_Table_Entry * variable);

	// DONT DO THIS:
		// if a variable of proc_name exists already, then don't push 
		// and "remove everything" concerned about the proc and the symbol table and 
		// DONOT RAISE AN ERROR (as per reference binary)
	// DO THIS:
		// simply append, 
	void append_proc_decls(Symbol_Table * procs_table);

	bool variable_in_symbol_list_check(string variable);
	bool variable_in_formal_list_check(string variable);

	string get_variable_in_formal_list(int position);
	Symbol_Table_Entry & get_symbol_table_entry(string variable_name);
	void global_list_in_proc_map_check();

	void print_debug();

	list<Symbol_Table_Entry *> & get_table();

	// compile
private:
	int get_size_of_value_type(Data_Type dt);

public:
	void set_start_offset_of_first_symbol(int n);
	int get_start_offset_of_first_symbol();

	void assign_offsets();
	int get_size();
	void set_size(int n);

	void print_assembly(ostream & file_buffer);
};

class Symbol_Table_Entry
{
	string variable_name;
	Data_Type variable_data_type;
	Table_Scope scope;
	Procedure * proc; //only if variable_data_type = func_data_type

	int lineno;

	// compile
	int start_offset;
	int end_offset;
	Register_Descriptor * register_description;
	Offset_With_Ref_To ref_off;
public:
	string global_string;

	Symbol_Table_Entry();
	Symbol_Table_Entry(string & name, Data_Type new_data_type, int line, Offset_With_Ref_To ro = fp_ref);
	~Symbol_Table_Entry();

	Offset_With_Ref_To get_ref_offset();

	int get_lineno();

	bool operator==(Symbol_Table_Entry & entry);

	void set_symbol_scope(Table_Scope sp);
	Table_Scope get_symbol_scope();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);
	string get_variable_name();
	void set_proc(Procedure * new_proc);
	Procedure * get_proc();

	void set_procedure(Procedure * inst_proc);

	//compile
	void set_start_offset(int num);
	int get_start_offset();
	void set_end_offset(int num);
	int get_end_offset();

	Register_Descriptor * get_register();
	void set_register(Register_Descriptor * reg);
	void free_register(Register_Descriptor * destination_reg_descr);
	void update_register(Register_Descriptor * result_reg_descr);
};

#endif
