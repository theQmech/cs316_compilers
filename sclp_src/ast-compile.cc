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

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_assembly()
{
	stringstream msg;
	msg << "No print_assembly() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_icode()
{
	stringstream msg;
	msg << "No print_icode() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Code_For_Ast & Assignment_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();
	CHECK_INVARIANT(load_register, "Load register cannot be null in Assignment_Ast");
	load_register->set_use_for_expr_result();

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	CHECK_INVARIANT((load_register != NULL), "Load register cannot be null in Assignment_Ast");
	load_register->reset_use_for_expr_result();

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}


/////////////////////////////////////////////////////////////////

Code_For_Ast & Name_Ast::compile()
{
	CHECK_INVARIANT(node_data_type==int_data_type || node_data_type==double_data_type, 
		"Invalid DATA_TYPE in Name_Ast::compile()");

	Register_Descriptor * reg;
	if (node_data_type == int_data_type)
		reg = machine_desc_object.get_new_register<gp_data>();
	else if (node_data_type == double_data_type)
		reg = machine_desc_object.get_new_register<float_reg>();

	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	Icode_Stmt * ic_stmt;
	if (node_data_type == int_data_type)
		ic_stmt = new Move_IC_Stmt(Tgt_Op::load, opd, result_opd);
	else if (node_data_type == double_data_type)
		ic_stmt = new Move_IC_Stmt(Tgt_Op::load_d, opd, result_opd);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();
	ic_list.push_back(ic_stmt);

	Code_For_Ast * ret_val;
	ret_val = new Code_For_Ast(ic_list, reg);
	return *ret_val;
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	Ics_Opd * result_opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Ics_Opd * opd = new Register_Addr_Opd(store_register);

	Icode_Stmt * ic_stmt;
	if (node_data_type == int_data_type)
		ic_stmt = new Move_IC_Stmt(Tgt_Op::store, opd, result_opd);
	else if (node_data_type == double_data_type)
		ic_stmt = new Move_IC_Stmt(Tgt_Op::store_d, opd, result_opd);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();
	ic_list.push_back(ic_stmt);

	Code_For_Ast * ret_val;
	ret_val = new Code_For_Ast(ic_list, store_register);
	return *ret_val;
}


///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	CHECK_INVARIANT(node_data_type==int_data_type || node_data_type==double_data_type, 
		"Invalid DATA_TYPE in Name_Ast::compile()");

	Register_Descriptor * reg;
	Ics_Opd *opd;
	if (node_data_type == int_data_type){
		reg = machine_desc_object.get_new_register<gp_data>();
		opd = new Const_Opd<int>(constant);
	}
	else if (node_data_type == double_data_type){
		reg = machine_desc_object.get_new_register<float_reg>();
		opd = new Const_Opd<double>(constant);
	}
	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	Icode_Stmt * ic_stmt;
	if (node_data_type == int_data_type)
		ic_stmt = new Move_IC_Stmt(Tgt_Op::imm_load, opd, result_opd);
	else if (node_data_type == double_data_type)
		ic_stmt = new Move_IC_Stmt(Tgt_Op::imm_load_d, opd, result_opd);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();
	ic_list.push_back(ic_stmt);

	Code_For_Ast * ret_val;
	ret_val = new Code_For_Ast(ic_list, reg);
	return *ret_val;
}

///////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Relational_Expr_Ast::compile()
{
	CHECK_INVARIANT((lhs_condition != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs_condition != NULL), "Rhs cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & lhs_stmt = lhs_condition->compile();
	Register_Descriptor * lhs_reg = lhs_stmt.get_reg();
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
	lhs_reg->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs_condition->compile();
	Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
	rhs_reg->set_use_for_expr_result();

	Register_Descriptor * reg;
	reg = machine_desc_object.get_new_register<gp_data>();
	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	lhs_reg->reset_use_for_expr_result();
	rhs_reg->reset_use_for_expr_result();

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	Icode_Stmt * new_ic;
	switch(rel_op){
		case less_equalto:
			new_ic = new Compute_IC_Stmt(Tgt_Op::sle, lhs_opd, rhs_opd, result_opd);
			break;
		case less_than:
			new_ic = new Compute_IC_Stmt(Tgt_Op::slt, lhs_opd, rhs_opd, result_opd);
			break;
		case greater_than:
			new_ic = new Compute_IC_Stmt(Tgt_Op::sgt, lhs_opd, rhs_opd, result_opd);
			break;
		case greater_equalto:
			new_ic = new Compute_IC_Stmt(Tgt_Op::sge, lhs_opd, rhs_opd, result_opd);
			break;
		case equalto:
			new_ic = new Compute_IC_Stmt(Tgt_Op::seq, lhs_opd, rhs_opd, result_opd);
			break;
		case not_equalto:
			new_ic = new Compute_IC_Stmt(Tgt_Op::sne, lhs_opd, rhs_opd, result_opd);
			break;
		default:
			new_ic = NULL;
			break;
	}
	if (new_ic)
		ic_list.push_back(new_ic);

	Code_For_Ast * rel_stmt;
	if (ic_list.empty() == false)
		rel_stmt = new Code_For_Ast(ic_list, reg);

	return *rel_stmt;
}

//////////////////////////////////////////////////////////////////////

Code_For_Ast & Boolean_Expr_Ast::compile()
{
	if (bool_op == boolean_not){
		CHECK_INVARIANT((rhs_op != NULL), "Rhs cannot be null in Assignment_Ast");
		machine_desc_object.clear_local_register_mappings();

		Ics_Opd *one_opd, *new_one_opd;
		Icode_Stmt * pre_not_stmt;
		Register_Descriptor * one_reg;
		one_reg = machine_desc_object.get_new_register<gp_data>();
		one_reg->set_use_for_expr_result();
		Ics_Opd * const_one_opd = new Const_Opd<int>(1);
		one_opd = new Register_Addr_Opd(one_reg);
		pre_not_stmt = new Move_IC_Stmt(Tgt_Op::imm_load, const_one_opd, one_opd);

		Code_For_Ast & rhs_stmt = rhs_op->compile();
		Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
		Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
		rhs_reg->set_use_for_expr_result();

		list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();
		ic_list.push_back(pre_not_stmt);
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

		Register_Descriptor * reg = machine_desc_object.get_new_register<gp_data>();
		Ics_Opd * result_opd = new Register_Addr_Opd(reg);

		one_reg->reset_use_for_expr_result();
		rhs_reg->reset_use_for_expr_result();

		Icode_Stmt * new_ic;
		new_one_opd = new Register_Addr_Opd(one_reg);
		new_ic = new Compute_IC_Stmt(Tgt_Op::not_t, rhs_opd, new_one_opd, result_opd);
		if (new_ic)
			ic_list.push_back(new_ic);

		Code_For_Ast * bool_stmt;
		if (ic_list.empty() == false)
			bool_stmt = new Code_For_Ast(ic_list, reg);

		return *bool_stmt;
	}

	else{

		CHECK_INVARIANT((lhs_op != NULL), "Lhs cannot be null in Assignment_Ast");
		CHECK_INVARIANT((rhs_op != NULL), "Rhs cannot be null in Assignment_Ast");
		machine_desc_object.clear_local_register_mappings();

		Code_For_Ast & lhs_stmt = *new Code_For_Ast();
		Register_Descriptor * lhs_reg;
		Ics_Opd * lhs_opd;
		if (ast_num_child == 2){
			lhs_stmt = lhs_op->compile();
			lhs_reg = lhs_stmt.get_reg();
			lhs_opd = new Register_Addr_Opd(lhs_reg);
			lhs_reg->set_use_for_expr_result();
		}

		Code_For_Ast & rhs_stmt = rhs_op->compile();
		Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
		Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
		rhs_reg->set_use_for_expr_result();

		list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

		Register_Descriptor * reg = machine_desc_object.get_new_register<gp_data>();
		Ics_Opd * result_opd = new Register_Addr_Opd(reg);

		lhs_reg->reset_use_for_expr_result();
		rhs_reg->reset_use_for_expr_result();

		Icode_Stmt * new_ic;
		switch(bool_op){
			case boolean_or:
				new_ic = new Compute_IC_Stmt(Tgt_Op::or_t, lhs_opd, rhs_opd, result_opd);
				break;
			case boolean_and:
				new_ic = new Compute_IC_Stmt(Tgt_Op::and_t, lhs_opd, rhs_opd, result_opd);
				break;
			default:
				CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Boolean_Expr_Ast invalid");
				new_ic = NULL;
				break;
		}
		if (new_ic)
			ic_list.push_back(new_ic);

		Code_For_Ast * rel_stmt;
		if (ic_list.empty() == false)
			rel_stmt = new Code_For_Ast(ic_list, reg);

		return *rel_stmt;
	}
}
///////////////////////////////////////////////////////////////////////

Code_For_Ast & Selection_Statement_Ast::compile()
{
	CHECK_INVARIANT((cond != NULL),"Condition cannot be null in Assignment_Ast");
	CHECK_INVARIANT((then_part != NULL), "then_part cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & cond_stmt = cond->compile();
	Register_Descriptor * cond_reg = cond_stmt.get_reg();
	Ics_Opd * cond_opd = new Register_Addr_Opd(cond_reg);
	cond_reg->set_use_for_expr_result();

	Code_For_Ast & then_stmt = then_part->compile();
	Code_For_Ast & else_stmt = *new Code_For_Ast();
	if (else_part)
		else_stmt = else_part->compile();

	cond_reg->reset_use_for_expr_result();

	Register_Descriptor *zero_reg = new Register_Descriptor(
		Spim_Register::zero, "zero", 
		Register_Val_Type::int_num, 
		Register_Use_Category::fixed_reg
	);
	Ics_Opd * zero_opd = new Register_Addr_Opd(zero_reg);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();
	string label_else = get_new_label();
	string label_last = get_new_label();

	ic_list.splice(ic_list.end(), cond_stmt.get_icode_list());
	ic_list.push_back(new Control_Flow_IC_Stmt(Tgt_Op::beq, cond_opd, zero_opd, label_else));
	ic_list.splice(ic_list.end(), then_stmt.get_icode_list());
	ic_list.push_back(new Control_Flow_IC_Stmt(Tgt_Op::j, NULL, NULL, label_last));
	ic_list.push_back(new Label_IC_Stmt(Tgt_Op::label, NULL, label_else));
	if (else_part)
		ic_list.splice(ic_list.end(), else_stmt.get_icode_list());
	ic_list.push_back(new Label_IC_Stmt(Tgt_Op::label, NULL, label_last));

	Code_For_Ast * sel_stmt;
	if (ic_list.empty() == false)
		sel_stmt = new Code_For_Ast(ic_list, NULL);

	return *sel_stmt;
}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Iteration_Statement_Ast::compile()
{
	CHECK_INVARIANT((cond != NULL),"Condition cannot be null in Assignment_Ast");
	CHECK_INVARIANT((body != NULL),"Condition cannot be null in Assignment_Ast");

	Code_For_Ast & cond_stmt = cond->compile();
	Register_Descriptor * cond_reg = cond_stmt.get_reg();
	Ics_Opd * cond_opd = new Register_Addr_Opd(cond_reg);
	cond_reg->set_use_for_expr_result();

	string label_body = get_new_label();
	string label_cond = get_new_label();

	Code_For_Ast & body_stmt = body->compile();

	Register_Descriptor *zero_reg = new Register_Descriptor(
		Spim_Register::zero, "zero", 
		Register_Val_Type::int_num, 
		Register_Use_Category::fixed_reg
	);
	Ics_Opd * zero_opd = new Register_Addr_Opd(zero_reg);

	cond_reg->reset_use_for_expr_result();

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();

	if (!is_do_form)
		ic_list.push_back(new Control_Flow_IC_Stmt(Tgt_Op::j, NULL, NULL, label_cond));
	ic_list.push_back(new Label_IC_Stmt(Tgt_Op::label, NULL, label_body));
	ic_list.splice(ic_list.end(), body_stmt.get_icode_list());
	ic_list.push_back(new Label_IC_Stmt(Tgt_Op::label, NULL, label_cond));
	ic_list.splice(ic_list.end(), cond_stmt.get_icode_list());
	ic_list.push_back(new Control_Flow_IC_Stmt(Tgt_Op::bne, cond_opd, zero_opd, label_body));

	Code_For_Ast * iter_stmt;
	if (ic_list.empty() == false)
		iter_stmt = new Code_For_Ast(ic_list, NULL);

	return *iter_stmt;
}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Plus_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_reg = lhs_stmt.get_reg();
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
	lhs_reg->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
	rhs_reg->set_use_for_expr_result();

	Register_Descriptor * reg;
	if (node_data_type == int_data_type)
		reg = machine_desc_object.get_new_register<gp_data>();
	else if (node_data_type == double_data_type)
		reg = machine_desc_object.get_new_register<float_reg>();

	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	Icode_Stmt * new_ic;
	if (node_data_type == int_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::add, lhs_opd, rhs_opd, result_opd);
	else if (node_data_type == double_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::add_d, lhs_opd, rhs_opd, result_opd);
	if (new_ic)
		ic_list.push_back(new_ic);

	lhs_reg->reset_use_for_expr_result();
	rhs_reg->reset_use_for_expr_result();

	Code_For_Ast * plus_stmt;
	if (ic_list.empty() == false)
		plus_stmt = new Code_For_Ast(ic_list, reg);

	return *plus_stmt;
}

/////////////////////////////////////////////////////////////////

Code_For_Ast & Minus_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_reg = lhs_stmt.get_reg();
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
	lhs_reg->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
	rhs_reg->set_use_for_expr_result();

	Register_Descriptor * reg;
	if (node_data_type == int_data_type)
		reg = machine_desc_object.get_new_register<gp_data>();
	else if (node_data_type == double_data_type)
		reg = machine_desc_object.get_new_register<float_reg>();

	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	Icode_Stmt * new_ic;
	if (node_data_type == int_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::sub, lhs_opd, rhs_opd, result_opd);
	else if (node_data_type == double_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::sub_d, lhs_opd, rhs_opd, result_opd);
	if (new_ic)
		ic_list.push_back(new_ic);

	lhs_reg->reset_use_for_expr_result();
	rhs_reg->reset_use_for_expr_result();

	Code_For_Ast * sub_stmt;
	if (ic_list.empty() == false)
		sub_stmt = new Code_For_Ast(ic_list, reg);

	return *sub_stmt;
}

//////////////////////////////////////////////////////////////////

Code_For_Ast & Mult_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_reg = lhs_stmt.get_reg();
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
	lhs_reg->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
	rhs_reg->set_use_for_expr_result();

	Register_Descriptor * reg;
	if (node_data_type == int_data_type)
		reg = machine_desc_object.get_new_register<gp_data>();
	else if (node_data_type == double_data_type)
		reg = machine_desc_object.get_new_register<float_reg>();

	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	Icode_Stmt * new_ic;
	if (node_data_type == int_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::mult, lhs_opd, rhs_opd, result_opd);
	else if (node_data_type == double_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::mult_d, lhs_opd, rhs_opd, result_opd);
	if (new_ic)
		ic_list.push_back(new_ic);

	lhs_reg->reset_use_for_expr_result();
	rhs_reg->reset_use_for_expr_result();

	Code_For_Ast * mult_stmt;
	if (ic_list.empty() == false)
		mult_stmt = new Code_For_Ast(ic_list, reg);

	return *mult_stmt;
}

////////////////////////////////////////////////////////////////////

Code_For_Ast & Conditional_Operator_Ast::compile()
{
	CHECK_INVARIANT((cond != NULL),"Condition cannot be null in Assignment_Ast");
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & cond_stmt = cond->compile();
	Register_Descriptor * cond_reg = cond_stmt.get_reg();
	Ics_Opd * cond_opd = new Register_Addr_Opd(cond_reg);
	cond_reg->set_use_for_expr_result();

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_reg = lhs_stmt.get_reg();
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
	lhs_reg->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
	rhs_reg->set_use_for_expr_result();

	Register_Descriptor *zero_reg = new Register_Descriptor(
		Spim_Register::zero, "zero", 
		Register_Val_Type::int_num, 
		Register_Use_Category::fixed_reg
	);
	Ics_Opd * zero_opd = new Register_Addr_Opd(zero_reg);

	string label_else = get_new_label();
	string label_last = get_new_label();

	Register_Descriptor * reg;
	if (node_data_type == int_data_type)
		reg = machine_desc_object.get_new_register<gp_data>();
	else if (node_data_type == double_data_type)
		reg = machine_desc_object.get_new_register<float_reg>();

	cond_reg->reset_use_for_expr_result();
	lhs_reg->reset_use_for_expr_result();
	rhs_reg->reset_use_for_expr_result();

	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();
	ic_list.splice(ic_list.end(), cond_stmt.get_icode_list());
	ic_list.push_back(new Control_Flow_IC_Stmt(Tgt_Op::beq, cond_opd, zero_opd, label_else));

	if (lhs_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), lhs_stmt.get_icode_list());
		ic_list.push_back(new Compute_IC_Stmt(Tgt_Op::or_t, lhs_opd, zero_opd, result_opd));
	}
	ic_list.push_back(new Control_Flow_IC_Stmt(Tgt_Op::j, NULL, NULL, label_last));
	ic_list.push_back(new Label_IC_Stmt(Tgt_Op::label, NULL, label_else));

	if (rhs_stmt.get_icode_list().empty() == false){
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(new Compute_IC_Stmt(Tgt_Op::or_t, rhs_opd, zero_opd, result_opd));
	}
	ic_list.push_back(new Label_IC_Stmt(Tgt_Op::label, NULL, label_last));

	Code_For_Ast * div_stmt;
	if (ic_list.empty() == false)
		div_stmt = new Code_For_Ast(ic_list, reg);

	return *div_stmt;
}


////////////////////////////////////////////////////////////////////

Code_For_Ast & Divide_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_reg = lhs_stmt.get_reg();
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
	lhs_reg->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_reg = rhs_stmt.get_reg();
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_reg);
	rhs_reg->set_use_for_expr_result();

	Register_Descriptor * reg;
	if (node_data_type == int_data_type)
		reg = machine_desc_object.get_new_register<gp_data>();
	else if (node_data_type == double_data_type)
		reg = machine_desc_object.get_new_register<float_reg>();

	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	Icode_Stmt * new_ic;
	if (node_data_type == int_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::divd, lhs_opd, rhs_opd, result_opd);
	else if (node_data_type == double_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::div_d, lhs_opd, rhs_opd, result_opd);
	if (new_ic)
		ic_list.push_back(new_ic);

	lhs_reg->reset_use_for_expr_result();
	rhs_reg->reset_use_for_expr_result();

	Code_For_Ast * div_stmt;
	if (ic_list.empty() == false)
		div_stmt = new Code_For_Ast(ic_list, reg);

	return *div_stmt;
}


//////////////////////////////////////////////////////////////////////

Code_For_Ast & UMinus_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	machine_desc_object.clear_local_register_mappings();

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_reg = lhs_stmt.get_reg();
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
	lhs_reg->set_use_for_expr_result();

	Register_Descriptor * reg;
	if (node_data_type == int_data_type)
		reg = machine_desc_object.get_new_register<gp_data>();
	else if (node_data_type == double_data_type)
		reg = machine_desc_object.get_new_register<float_reg>();

	Ics_Opd * result_opd = new Register_Addr_Opd(reg);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>();

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	Icode_Stmt * new_ic;
	if (node_data_type == int_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::uminus, lhs_opd, NULL, result_opd);
	else if (node_data_type == double_data_type)
		new_ic = new Compute_IC_Stmt(Tgt_Op::uminus_d, lhs_opd, NULL, result_opd);
	if (new_ic)
		ic_list.push_back(new_ic);

	lhs_reg->reset_use_for_expr_result();

	Code_For_Ast * div_stmt;
	if (ic_list.empty() == false)
		div_stmt = new Code_For_Ast(ic_list, reg);

	return *div_stmt;
}

//////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Sequence_Ast::compile()
{
	bool flag = (optimize_flag==0);
	++optimize_flag;

	machine_desc_object.clear_local_register_mappings();
	for(list<Ast *>::iterator it=statement_list.begin();
			it!=statement_list.end(); ++it){
		Code_For_Ast curr = (*it)->compile();
		sa_icode_list.splice(sa_icode_list.end(), curr.get_icode_list());

		Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
		Ics_Opd * result_opd = new Register_Addr_Opd(reg);

		Icode_Stmt * ic_stmt;
		if (node_data_type == int_data_type)
			ic_stmt = new Move_IC_Stmt(Tgt_Op::load, opd, result_opd);
		else if (node_data_type == double_data_type)
			ic_stmt = new Move_IC_Stmt(Tgt_Op::load_d, opd, result_opd);



	}

	if (flag){
		CFG * cfg = new CFG(sa_icode_list);
		// cfg->optimise();
		sa_icode_list = cfg->toInstructionList();
	}

	Code_For_Ast *ret_val;
	ret_val = new Code_For_Ast(sa_icode_list, NULL);
	return *ret_val;
}

void Sequence_Ast::print_assembly(ostream & file_buffer)
{
	for (list<Icode_Stmt *>::iterator it=sa_icode_list.begin();
			it!=sa_icode_list.end(); ++it){
		(*it)->print_assembly(file_buffer);
	}
}

void Sequence_Ast::print_icode(ostream & file_buffer)
{
	for (list<Icode_Stmt *>::iterator it=sa_icode_list.begin();
			it!=sa_icode_list.end(); ++it){
		(*it)->print_icode(file_buffer);
	}
}

//////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Return_Ast::compile(){
	// CHECK_INVARIANT();
	// return_variable is null iff return type is void_data_type

	list<Icode_Stmt *> icode_list;
	Register_Descriptor * reg;

	if (return_variable != NULL){
		Code_For_Ast & expr_stmt = return_variable->compile();
		reg = expr_stmt.get_reg();
		// Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_reg);
		// lhs_reg->set_use_for_expr_result();
		
		add the storing to v1 statement
	}

	Icode_Stmt * new_ic = new Control_Flow_IC_Stmt(ret_inst, NULL, NULL, "");
	icode_list.push_back(new_ic);

	Code_For_Ast *ret_val = new Code_For_Ast(icode_list, reg);
	return ret_val;
}

void Return_Ast::print_assembly(ostream & file_buffer){

}

void Return_Ast::print_icode(ostream & file_buffer){

}

//////////////////////////////////////////////////////////////////////////////

template class Number_Ast<double>;
template class Number_Ast<int>;


//////////////////////////////////////////////////////////////////////////////
////////// CODE FOR OPTIMIZATIONS ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////


Block::Block(){

}
void Block::add_stmt(Icode_Stmt * stmt){
	instructions.push_back(stmt);
}
void Block::add_prev(int id){
	previous.push_back(id);
}
void Block::add_succ(int id){
	next.push_back(id);
}
int Block::get_size(){
	return instructions.size();
}
Block::~Block(){
	previous.clear();
	next.clear();
}

//////////////////////////////////////////////////////

CFG::CFG(list<Icode_Stmt *> inst){
	vector<string> goto_labels;
	map<string, int> M;

	nodes.push_back(new Block());
	goto_labels.push_back("");
	int block_idx = 0;

	for(list<Icode_Stmt *>::iterator it = inst.begin();
			it!=inst.end(); ++it){

		Tgt_Op stmt_op = (*it)->get_op().get_op();
		if (stmt_op == Tgt_Op::beq || stmt_op == Tgt_Op::bne || 
			stmt_op == Tgt_Op::bgtz || stmt_op == Tgt_Op::bgez || 
			stmt_op == Tgt_Op::bltz || stmt_op == Tgt_Op::blez){

			nodes[block_idx]->add_stmt(*it);
			goto_labels[block_idx] = ( (Control_Flow_IC_Stmt *)(*it) )->get_Offset();

			nodes.push_back(new Block());
			goto_labels.push_back("");
			++block_idx;

			nodes[block_idx]->add_prev(block_idx-1);
			nodes[block_idx-1]->add_succ(block_idx);
		}
		else if(stmt_op == Tgt_Op::j){
			nodes[block_idx]->add_stmt(*it);
			goto_labels[block_idx] = ( (Control_Flow_IC_Stmt *)(*it) )->get_Offset();

			nodes.push_back(new Block());
			goto_labels.push_back("");
			++block_idx;
		}
		else if (stmt_op == Tgt_Op::label){
			nodes.push_back(new Block());
			goto_labels.push_back("");
			++block_idx;

			nodes[block_idx]->add_stmt(*it);
			M[( (Label_IC_Stmt *)(*it) )->get_offset()] = block_idx;
		}
		else{
			nodes[block_idx]->add_stmt(*it);
		}
	}

	// cout<<block_idx<<endl;

	if (nodes[block_idx]->get_size() == 0){
		delete nodes[block_idx];
		nodes.pop_back();
		--block_idx;
	}

	for(int i=0; i<nodes.size(); ++i){
		if (goto_labels[i].length()>0){
			nodes[i]->add_succ(M[goto_labels[i]]);
			nodes[M[goto_labels[i]]]->add_prev(i);
		}
	}

}

CFG::~CFG(){
	for (auto it = nodes.begin(); it != nodes.end(); ++it){
		delete (*it);
	}
	nodes.clear();
}

void CFG::optimise(){
	int c = nodes.size();
	for (int it = 0; it != c; ++it){
		if (nodes[it]->previous.size()==0){
			if (it!=0){
				nodes.erase(nodes.begin()+it);
				it--;
				c = nodes.size();
			}
		}
	}
}

void Block::eliminate(vector<Block *> nodes){
	Out.clear();
	for (auto it=next.begin(); it!=next.end(); ++it)
		Out.insert(nodes[*it]->In.begin(), nodes[*it]->In.end());

	list<set<Spim_Register> > live_vars(instructions.size()+1, set<Spim_Register>());
	*live_vars.rbegin() = Out;

	auto it_code = instructions.rbegin();
	auto it_vars = live_vars.rbegin(); ++it_vars;

	while(it_code != instructions.rend()){
		// *it_vars = // replace by expression;

		// comapare *it_vars and *(it_vars-1)
		//erase return iterator to next element in list(in this case previous)
		if (true){
			it_code = list<Icode_Stmt *>::reverse_iterator(instructions.erase(it_code.base()));
			it_vars = list<set<Spim_Register> >::reverse_iterator(live_vars.erase(it_vars.base()));
			// list<Icode_Stmt *>::const_reverse_iterator const_it_code = it_code;
			// it_code = instructions.erase(const_it_code.base());
			// list<set<Spim_Register> >::const_reverse_iterator const_it_vars = it_vars;
			// it_vars = live_vars.erase(const_it_vars);
		}
		// else{
			++it_code;
			++it_vars;
		// }
	}

	In = *(live_vars.begin());
}

void CFG::dead_code_eliminate(){
	int n = nodes.size();
	vector<bool> in_queue(n, false);

	queue<int> Q;
	Q.push(n-1);
	in_queue[n-1] = true;

	while(!Q.empty()){
		int curr = Q.front();
		Q.pop();
		in_queue[curr] = false;

		set<Spim_Register> old_in = nodes[curr]->In;
		nodes[curr]->eliminate(nodes);
		if (old_in != nodes[curr]->In){
			for(auto it = nodes[curr]->previous.begin(); it!=nodes[curr]->previous.end(); ++it){
				if (!in_queue[*it])
					Q.push(*it);
			}
		}

	}

}


list<Icode_Stmt *> CFG::toInstructionList(){
	list<Icode_Stmt *> inst;

	for (int it1 = 0; it1 != nodes.size(); ++it1)
		inst.splice(inst.end(), nodes[it1]->instructions);

	return inst;
}
/////////////////////////////////////////////////////////////