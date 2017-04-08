#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

#include "common-classes.hh"
#include "error-display.hh"
#include "icode.hh"
#include "reg-alloc.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "program.hh"

Machine_Description machine_desc_object;

//////////////////////////// Register Descriptor ///////////////////////////////

Register_Descriptor::Register_Descriptor(Spim_Register reg, string s, Register_Val_Type vt, Register_Use_Category uc)
{
	reg_id = reg;
	reg_name = s;
	value_type = vt;
	reg_use = uc; 
	used_for_expr_result = false;
	reg_occupied = false;
	used_for_fn_return = false;
}

Register_Use_Category Register_Descriptor::get_use_category()	{ return reg_use; }
Spim_Register Register_Descriptor::get_register()		{ return reg_id; }
string Register_Descriptor::get_name()				{ return reg_name; }
bool Register_Descriptor::is_symbol_list_empty()		{ return lra_symbol_list.empty(); }

bool Register_Descriptor::is_register_occupied()		{ return reg_occupied; }
void Register_Descriptor::set_register_occupied()		{ reg_occupied = true; }
void Register_Descriptor::reset_register_occupied()		{ reg_occupied = false; }

bool Register_Descriptor::is_used_for_expr_result()		{ return used_for_expr_result; }
void Register_Descriptor::reset_use_for_expr_result()		{ 
								// cout<<"reset "<<reg_name<<endl; 
								used_for_expr_result = false; 
								reg_occupied = false; 
								used_for_fn_return = false; }
void Register_Descriptor::set_use_for_expr_result()		{ used_for_expr_result = true; }

void Register_Descriptor::set_used_for_fn_return()		{ used_for_fn_return = true; }
void Register_Descriptor::reset_used_for_fn_return()		{ used_for_fn_return = false; }
bool Register_Descriptor::is_used_for_fn_return()		{ return used_for_fn_return; }

int Register_Descriptor::count_symbol_entry_in_list()
{
	int count = 0;

	list<Symbol_Table_Entry *>::iterator i;
	for(i = lra_symbol_list.begin(); i != lra_symbol_list.end(); i++)
		count++;

	return count;
}

template <Register_Use_Category dt>
bool Register_Descriptor::is_free()     
{
	if ((reg_use == dt) && 
	(lra_symbol_list.empty()) && 
	(is_used_for_expr_result() == false) && 
	(is_register_occupied() == false)) 
		return true;
	else 
		return false;
}

void Register_Descriptor::remove_symbol_entry_from_list(Symbol_Table_Entry & sym_entry)
{
	lra_symbol_list.remove(&sym_entry);
}

bool Register_Descriptor::find_symbol_entry_in_list(Symbol_Table_Entry & sym_entry)
{
	list<Symbol_Table_Entry *>::iterator i;
	for (i = lra_symbol_list.begin(); i != lra_symbol_list.end(); i++)
		if (**i == sym_entry)
			return true;

	return false;
}

void Register_Descriptor::clear_lra_symbol_list()
{
	list<Symbol_Table_Entry *>::iterator i;
	for (i = lra_symbol_list.begin(); i != lra_symbol_list.end(); i++)
	{
		Symbol_Table_Entry & sym_entry = **i;
		sym_entry.set_register(NULL);
	}

	lra_symbol_list.clear();
}

void Register_Descriptor::update_symbol_information(Symbol_Table_Entry & sym_entry)
{
	if (find_symbol_entry_in_list(sym_entry) == false)
		lra_symbol_list.push_back(&sym_entry);
}

/******************************* Machine Description *****************************************/

void Machine_Description::initialize_register_table()
{
	spim_register_table[zero] = new Register_Descriptor(zero, "zero", int_num, fixed_reg);
	spim_register_table[v0] = new Register_Descriptor(v0, "v0", int_num, gp_data);
	spim_register_table[v1] = new Register_Descriptor(v1, "v1", int_num, fn_result);
	spim_register_table[a0] = new Register_Descriptor(a0, "a0", int_num, argument);
	spim_register_table[a1] = new Register_Descriptor(a1, "a1", int_num, argument);
	spim_register_table[a2] = new Register_Descriptor(a2, "a2", int_num, argument);
	spim_register_table[a3] = new Register_Descriptor(a3, "a3", int_num, argument);
	spim_register_table[t0] = new Register_Descriptor(t0, "t0", int_num, gp_data);
	spim_register_table[t1] = new Register_Descriptor(t1, "t1", int_num, gp_data);
	spim_register_table[t2] = new Register_Descriptor(t2, "t2", int_num, gp_data);
	spim_register_table[t3] = new Register_Descriptor(t3, "t3", int_num, gp_data);
	spim_register_table[t4] = new Register_Descriptor(t4, "t4", int_num, gp_data);
	spim_register_table[t5] = new Register_Descriptor(t5, "t5", int_num, gp_data);
	spim_register_table[t6] = new Register_Descriptor(t6, "t6", int_num, gp_data);
	spim_register_table[t7] = new Register_Descriptor(t7, "t7", int_num, gp_data);
	spim_register_table[t8] = new Register_Descriptor(t8, "t8", int_num, gp_data);
	spim_register_table[t9] = new Register_Descriptor(t9, "t9", int_num, gp_data);
	spim_register_table[s0] = new Register_Descriptor(s0, "s0", int_num, gp_data);
	spim_register_table[s1] = new Register_Descriptor(s1, "s1", int_num, gp_data);
	spim_register_table[s2] = new Register_Descriptor(s2, "s2", int_num, gp_data);
	spim_register_table[s3] = new Register_Descriptor(s3, "s3", int_num, gp_data);
	spim_register_table[s4] = new Register_Descriptor(s4, "s4", int_num, gp_data);
	spim_register_table[s5] = new Register_Descriptor(s5, "s5", int_num, gp_data);
	spim_register_table[s6] = new Register_Descriptor(s6, "s6", int_num, gp_data);
	spim_register_table[s7] = new Register_Descriptor(s7, "s7", int_num, gp_data);
	spim_register_table[f0] = new Register_Descriptor(f0, "f0", float_num, fn_result);
	spim_register_table[f2] = new Register_Descriptor(f2, "f2", float_num, float_reg);
	spim_register_table[f4] = new Register_Descriptor(f4, "f4", float_num, float_reg);
	spim_register_table[f6] = new Register_Descriptor(f6, "f6", float_num, float_reg);
	spim_register_table[f8] = new Register_Descriptor(f8, "f8", float_num, float_reg);
	spim_register_table[f10] = new Register_Descriptor(f10, "f10", float_num, float_reg);
	spim_register_table[f12] = new Register_Descriptor(f12, "f12", float_num, float_reg);
	spim_register_table[f14] = new Register_Descriptor(f14, "f14", float_num, float_reg);
	spim_register_table[f16] = new Register_Descriptor(f16, "f16", float_num, float_reg);
	spim_register_table[f18] = new Register_Descriptor(f18, "f18", float_num, float_reg);
	spim_register_table[f20] = new Register_Descriptor(f20, "f20", float_num, float_reg);
	spim_register_table[f22] = new Register_Descriptor(f22, "f22", float_num, float_reg);
	spim_register_table[f24] = new Register_Descriptor(f24, "f24", float_num, float_reg);
	spim_register_table[f26] = new Register_Descriptor(f26, "f26", float_num, float_reg);
	spim_register_table[f28] = new Register_Descriptor(f28, "f28", float_num, float_reg);
	spim_register_table[f30] = new Register_Descriptor(f30, "f30", float_num, float_reg);
	spim_register_table[gp] = new Register_Descriptor(gp, "gp", int_num, pointer);
	spim_register_table[sp] = new Register_Descriptor(sp, "sp", int_num, pointer);
	spim_register_table[fp] = new Register_Descriptor(fp, "fp", int_num, pointer);
	spim_register_table[ra] = new Register_Descriptor(ra, "ra", int_num, ret_address);
}

//define addi below
void Machine_Description::initialize_instruction_table()
{
	spim_instruction_table[Tgt_Op::store] = new Instruction_Descriptor(Tgt_Op::store, "store", "sw", "", i_r_op_o1, a_op_o1_r);
	spim_instruction_table[Tgt_Op::load] = new Instruction_Descriptor(Tgt_Op::load, "load", "lw", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::la] = new Instruction_Descriptor(Tgt_Op::la, "loadAdd", "la", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::imm_load] = new Instruction_Descriptor(Tgt_Op::imm_load, "iLoad", "li", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::and_t] = new Instruction_Descriptor(Tgt_Op::and_t, "and", "and", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::or_t] = new Instruction_Descriptor(Tgt_Op::or_t, "or", "or", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::not_t] = new Instruction_Descriptor(Tgt_Op::not_t, "not", "sltu", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::mov] = new Instruction_Descriptor(Tgt_Op::mov, "move", "move", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::add] = new Instruction_Descriptor(Tgt_Op::add, "add", "add", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::sub] = new Instruction_Descriptor(Tgt_Op::sub, "sub", "sub", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::mult] = new Instruction_Descriptor(Tgt_Op::mult, "mul", "mul", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::divd] = new Instruction_Descriptor(Tgt_Op::divd, "div", "div", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::imm_add] = new Instruction_Descriptor(Tgt_Op::imm_add, "addi", "addi", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::uminus] = new Instruction_Descriptor(Tgt_Op::uminus, "uminus", "neg", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::store_d] = new Instruction_Descriptor(Tgt_Op::store_d, "store.d", "s.d", "", i_r_op_o1, a_op_o1_r);
	spim_instruction_table[Tgt_Op::load_d] = new Instruction_Descriptor(Tgt_Op::load_d, "load.d", "l.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::imm_load_d] = new Instruction_Descriptor(Tgt_Op::imm_load_d, "iLoad.d", "li.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::move_d] = new Instruction_Descriptor(Tgt_Op::move_d, "move.d", "mov.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::add_d] = new Instruction_Descriptor(Tgt_Op::add_d, "add.d", "add.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::sub_d] = new Instruction_Descriptor(Tgt_Op::sub_d, "sub.d", "sub.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::mult_d] = new Instruction_Descriptor(Tgt_Op::mult_d, "mul.d", "mul.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::div_d] = new Instruction_Descriptor(Tgt_Op::div_d, "div.d", "div.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::uminus_d] = new Instruction_Descriptor(Tgt_Op::uminus_d, "uminus.d", "neg.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[Tgt_Op::slt] = new Instruction_Descriptor(Tgt_Op::slt, "slt", "slt", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::sle] = new Instruction_Descriptor(Tgt_Op::sle, "sle", "sle", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::sgt] = new Instruction_Descriptor(Tgt_Op::sgt, "sgt", "sgt", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::sge] = new Instruction_Descriptor(Tgt_Op::sge, "sge", "sge", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::seq] = new Instruction_Descriptor(Tgt_Op::seq, "seq", "seq", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::sne] = new Instruction_Descriptor(Tgt_Op::sne, "sne", "sne", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[Tgt_Op::beq] = new Instruction_Descriptor(Tgt_Op::beq, "beq", "beq", "", i_op_o1_o2_st, a_op_o1_o2_st);
	spim_instruction_table[Tgt_Op::bne] = new Instruction_Descriptor(Tgt_Op::bne, "bne", "bne", "", i_op_o1_o2_st, a_op_o1_o2_st);
	spim_instruction_table[Tgt_Op::bgtz] = new Instruction_Descriptor(Tgt_Op::bgtz, "bgtz", "bgtz", "", i_op_o1_o2_st, a_op_o1_o2_st);
	spim_instruction_table[Tgt_Op::bgez] = new Instruction_Descriptor(Tgt_Op::bgez, "bgez", "bgez", "", i_op_o1_o2_st, a_op_o1_o2_st);
	spim_instruction_table[Tgt_Op::bltz] = new Instruction_Descriptor(Tgt_Op::bltz, "bltz", "bltz", "", i_op_o1_o2_st, a_op_o1_o2_st);
	spim_instruction_table[Tgt_Op::blez] = new Instruction_Descriptor(Tgt_Op::blez, "blez", "blez", "", i_op_o1_o2_st, a_op_o1_o2_st);
	spim_instruction_table[Tgt_Op::j] = new Instruction_Descriptor(Tgt_Op::j, "jump", "j", "", i_op_st, a_op_st);
	spim_instruction_table[Tgt_Op::jal] = new Instruction_Descriptor(Tgt_Op::jal, "jal", "jal", "", i_op_st, a_op_st);
	spim_instruction_table[Tgt_Op::syscall] = new Instruction_Descriptor(Tgt_Op::syscall, "syscall", "syscall", "", i_op_st, a_op_st);
	spim_instruction_table[Tgt_Op::label] = new Instruction_Descriptor(Tgt_Op::label, "", "", "", i_op_st, a_op_st);
	spim_instruction_table[Tgt_Op::ret_inst] = new Instruction_Descriptor(Tgt_Op::ret_inst, "return", "jr $31", "", i_op, a_op);
	spim_instruction_table[Tgt_Op::seq_d] = new Instruction_Descriptor(Tgt_Op::seq_d, "seq.d", "c.eq.d", "", i_o1_op_o2, a_op_o1_o2);
	spim_instruction_table[Tgt_Op::slt_d] = new Instruction_Descriptor(Tgt_Op::slt_d, "slt.d", "c.lt.d", "", i_o1_op_o2, a_op_o1_o2);
	spim_instruction_table[Tgt_Op::sle_d] = new Instruction_Descriptor(Tgt_Op::sle_d, "sle.d", "c.le.d", "", i_o1_op_o2, a_op_o1_o2);
	spim_instruction_table[Tgt_Op::bc1t] = new Instruction_Descriptor(Tgt_Op::bc1t, "bc1t", "bc1t", "", i_op_st, a_op_st);
	spim_instruction_table[Tgt_Op::bc1f] = new Instruction_Descriptor(Tgt_Op::bc1f, "bc1f", "bc1f", "", i_op_st, a_op_st);
}

void Machine_Description::validate_init_local_register_mapping_before_fn_call()
{
	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		Register_Descriptor * reg_desc = i->second;

		if (reg_desc->is_used_for_fn_return() == false)
		{
			if (reg_desc->get_use_category() == gp_data)
			{
				CHECK_INVARIANT((reg_desc->is_free<gp_data>()), "GP data registers should be free at the start of a basic block or after a function call");
			}

			else if (reg_desc->get_use_category() == float_reg)
			{
				CHECK_INVARIANT((reg_desc->is_free<float_reg>()), "Float data registers should be free at the start of a basic block or after a function call");
			}
		}
	}
}

void Machine_Description::validate_init_local_register_mapping()
{
	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		Register_Descriptor * reg_desc = i->second;

		if (reg_desc->get_use_category() == gp_data)
		{
			CHECK_INVARIANT((reg_desc->is_free<gp_data>()), "GP data registers should be free at the start of a basic block or after a function call");
		}

		else if (reg_desc->get_use_category() == float_reg)
		{
			CHECK_INVARIANT((reg_desc->is_free<float_reg>()), "Float data registers should be free at the start of a basic block or after a function call");
		}
	}
}

void Machine_Description::clear_local_register_mappings()
{
	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		Register_Descriptor * reg_desc = i->second;
		reg_desc->clear_lra_symbol_list();
		reg_desc->reset_register_occupied();
		reg_desc->reset_use_for_expr_result();
	}

	/* 
	Note that we do not need to save values at the end
	of a basic block because the values have already been
	saved for each assignment statement. Any optimization
	that tries to postpone the store statements may have to 
	consider storing all unstored values at the end of
	a basic block.
	*/
}

template <Register_Use_Category dt>
Register_Descriptor * Machine_Description::get_new_register()
{
	Register_Descriptor * reg_desc;
	int count = 0;

	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		reg_desc = i->second;

		CHECK_INVARIANT((reg_desc != NULL), "Null register descriptor in the register table");

		if (reg_desc->is_free<dt>()) 
		{
			reg_desc->set_register_occupied();
			return reg_desc;
		}
	}

	machine_desc_object.clear_reg_not_used_for_expr_result();

	count = machine_desc_object.count_free_register<dt>();
	if (count > 0)
		return get_new_register<dt>();
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
			"Error in get_new_reg or register requirements of input program cannot be met");
	}
}

template <Register_Use_Category dt>
int Machine_Description::count_free_register()
{
	Register_Descriptor * rdp = NULL;
	int count = 0;

	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		rdp = i->second;
		if (rdp->is_free<dt>())
			count++;
	}

	return count;
}

void Machine_Description::clear_reg_not_used_for_expr_result()
{
	map<Spim_Register, Register_Descriptor *>::iterator i;

	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		Register_Descriptor * rdp = i->second;

		if(!rdp->is_used_for_expr_result()) 
		{
			rdp->reset_register_occupied();  /* reset reg occupied i.e register is not anymore occupied */
			rdp->clear_lra_symbol_list();
			break;
		}
	}
}

template bool Register_Descriptor::is_free<gp_data>();
template bool Register_Descriptor::is_free<float_reg>();

template Register_Descriptor * Machine_Description::get_new_register<gp_data>();
template Register_Descriptor * Machine_Description::get_new_register<float_reg>();
template Register_Descriptor * Machine_Description::get_new_register<fn_result>();

template int Machine_Description::count_free_register<gp_data>();
template int Machine_Description::count_free_register<float_reg>();
