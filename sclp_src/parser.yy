%scanner scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
    pair<Data_Type, string> * decl;
    Symbol_Table * symbol_table;
    Symbol_Table_Entry * symbol_entry;
    list<Symbol_Table_Entry *> * list_symbol_entry;
    Procedure * procedure;
    //ADD CODE HERE
    int int_value;
    float float_value;
    std::string * string_value;
    Ast * ast;
    Sequence_Ast * sequence_ast;
    Func_Call_Ast * func_call_ast;
    list<Ast *> * list_ast;
    Return_Ast * return_ast;
    Print_Ast * print_ast;
    String_Ast * string_ast;
    list<string> * list_string;
    pair<Data_Type, list<string>*> * new_decl;
};

//ADD TOKENS HEREs
%token <int_value> INTEGER_NUMBER
%token <float_value> DOUBLE_NUMBER
%token <string_value> STRING
%token <string_value> NAME
%token VOID INTEGER FLOAT ASSIGN WHILE IF DO PRINT
// %token FOR GOTO SWITCH 
// %token COLON QMARK
// %token LT LTE GT GTE EQ NEQ AND OR NOT

%token QMARK COLON RETURN
%left OR
%left AND
%left EQ NEQ
%left LT LTE GT GTE
%left '+' '-'
%left '*' '/'
%right NOT
%right UMINUS
%nonassoc '(' ELSE
%nonassoc '{'

%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> variable_declaration_list
%type <symbol_table> procedure_declaration_list
%type <symbol_table> parameter_list
%type <symbol_entry> procedure_declaration
%type <symbol_entry> parameter
%type <list_symbol_entry> variable_declaration
%type <new_decl> declaration
//ADD CODE HERE
%type <ast> variable
%type <ast> constant
%type <ast> operand
%type <ast> expression_term
%type <ast> assignment_statement
%type <ast> arith_expression
%type <ast> bool_expression
%type <ast> postblock
%type <ast> block
%type <sequence_ast> statement_list
%type <return_ast> return_stmt
%type <print_ast> print_stmt
%type <ast> print_argument
%type <list_string> variable_list
%type <list_ast> arguments
%type <func_call_ast> func_call


%start program

%%

program:
    declaration_list procedure_definition_list
    {
    if (NOT_ONLY_PARSE)
    {
        CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

        program_object.set_procedure(current_procedure, get_line_number());
        program_object.global_list_in_proc_check();
    }
    }
;

declaration_list:
    procedure_declaration_list
    {
    if (NOT_ONLY_PARSE)
    {
        Symbol_Table * global_table = $1;
        program_object.set_global_table(*global_table);
    }
    }
|
    variable_declaration_list
    procedure_declaration_list
    {
    if (NOT_ONLY_PARSE)
    {
        Symbol_Table * global_table = $1;

        CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");
        CHECK_INVARIANT($2->variable_in_symbol_list_check("main"),
            "Procedure main is not defined");

        list<Symbol_Table_Entry *> list_global_vars = $1->get_table();
        for (list<Symbol_Table_Entry *>::iterator it = list_global_vars.begin(); 
            it!=list_global_vars.end(); ++it){
            (*it)->set_symbol_scope(global);
        }

        global_table->append_proc_decls($2);

        program_object.set_global_table(*global_table);
    }
    }
|
    procedure_declaration_list
    variable_declaration_list
    {
    if (NOT_ONLY_PARSE)
    {
        Symbol_Table * global_table = $2;

        CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");
        CHECK_INVARIANT($1->variable_in_symbol_list_check("main"),
            "Procedure main is not defined");

        list<Symbol_Table_Entry *> list_global_vars = $2->get_table();
        for (list<Symbol_Table_Entry *>::iterator it = list_global_vars.begin(); 
            it!=list_global_vars.end(); ++it){
            (*it)->set_symbol_scope(global);
        }

        global_table->append_proc_decls($1);

        program_object.set_global_table(*global_table);
    }
    }
;

procedure_declaration_list:
    procedure_declaration
    {
        $$ = new Symbol_Table();
        $$->push_symbol($1);
    }
|
    procedure_declaration_list procedure_declaration
    {
        $$ = $1;
        CHECK_INVARIANT(!($$->variable_in_symbol_list_check($2->get_variable_name())),
            "Overloading of the procedure is not allowed");
        $$->push_symbol($2);
    }
;

procedure_declaration:
    VOID NAME '(' parameter_list ')' ';'
    {
    if (NOT_ONLY_PARSE)
    {

        CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
        CHECK_INPUT(!($4->variable_in_symbol_list_check(*$2)), 
            "Procedure name cannot be same as formal parameter name", get_line_number());
        $$ = new Symbol_Table_Entry(*$2, func_data_type, get_line_number());
        $$->set_symbol_scope(global);
        Procedure * proc = new Procedure(void_data_type, *$2, get_line_number());
        proc->set_local_list($4);
        program_object.insert_proc_in_map(proc);
        $$->set_proc(proc);
    }
    }
|
    INTEGER NAME '(' parameter_list ')' ';'
    {
    if (NOT_ONLY_PARSE)
    {
        CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
        CHECK_INPUT(!($4->variable_in_symbol_list_check(*$2)), 
            "Procedure name cannot be same as formal parameter name", get_line_number());
        $$ = new Symbol_Table_Entry(*$2, func_data_type, get_line_number());
        $$->set_symbol_scope(global);
        Procedure * proc = new Procedure(int_data_type, *$2, get_line_number());
        proc->set_local_list($4);
        program_object.insert_proc_in_map(proc);
        $$->set_proc(proc);
    }
    }
|
    FLOAT NAME '(' parameter_list ')' ';'
    {
    if (NOT_ONLY_PARSE)
    {
        CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
        CHECK_INPUT(!($4->variable_in_symbol_list_check(*$2)), 
            "Procedure name cannot be same as formal parameter name", get_line_number());
        $$ = new Symbol_Table_Entry(*$2, func_data_type, get_line_number());
        $$->set_symbol_scope(global);
        Procedure * proc = new Procedure(double_data_type, *$2, get_line_number());
        proc->set_local_list($4);
        program_object.insert_proc_in_map(proc);
        $$->set_proc(proc);
    }
    }
;

parameter_list:
{
    $$ = new Symbol_Table();
}
|
parameter
{
    $$ = new Symbol_Table();
    $1->set_symbol_scope(formal);
    $$->push_symbol($1);
}
|
parameter_list ',' parameter
{
    $$ = $1;
    CHECK_INPUT(!($$->variable_in_symbol_list_check($3->get_variable_name())),
        "Formal Parameter declared twice", get_line_number());
    $3->set_symbol_scope(formal);
    $$->push_symbol($3);
}
;

parameter:
INTEGER NAME
{
    $$ = new Symbol_Table_Entry(*$2, int_data_type, get_line_number());
    $$->set_symbol_scope(formal);
}
|
FLOAT NAME
{
    $$ = new Symbol_Table_Entry(*$2, double_data_type, get_line_number());
    $$->set_symbol_scope(formal);
}
;

procedure_definition_list:
    procedure_definition
    {

    }
|
    procedure_definition_list procedure_definition
    {

    }
;

procedure_definition:
    NAME '(' parameter_list ')'
    {
    if (NOT_ONLY_PARSE)
    {
        CHECK_INPUT(($1 != NULL), "Procedure name cannot be null", get_line_number());

        CHECK_INVARIANT(program_object.variable_in_symbol_list_check(*$1), 
            "Procedure prototype cannot be null");
        Symbol_Table_Entry proc_entry = program_object.get_symbol_table_entry(*$1);
        CHECK_INPUT(proc_entry.get_data_type()==func_data_type,
            "Procedure name cannot be same as global variable", get_line_number());
        // CHECK_INPUT(proc_entry.get_proc()->match_prototype(*$3),
        //     "Procedure prototype doesn't match", get_line_number());

        current_procedure = proc_entry.get_proc();
    }
    }

    '{' optional_variable_declaration_list
    {
    if (NOT_ONLY_PARSE)
    {

        CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

        Symbol_Table * local_table = $7;
        if (local_table == NULL)
            local_table = new Symbol_Table();
        list<Symbol_Table_Entry *> local_table_list = local_table->get_table();

        for (list<Symbol_Table_Entry *>::iterator it = local_table_list.begin(); 
            it!=local_table_list.end(); ++it){
            CHECK_INPUT(!(current_procedure->variable_in_symbol_list_check((*it)->get_variable_name())),
                "Formal parameter and local variable name cannot be same", get_line_number());
            (*it)->set_symbol_scope(local);
            current_procedure->push_symbol(*it);
        }
    }
    }

    statement_list return_stmt '}'
    {
    if (NOT_ONLY_PARSE)
    {
        Sequence_Ast* seq = $9;
        Return_Ast * ret = $10;

        CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");
        CHECK_INVARIANT((seq != NULL), "statement list cannot be null");
        CHECK_INVARIANT((ret != NULL), "return statement cannot be null");

        CHECK_INPUT(current_procedure->get_return_type()==ret->get_data_type(),
            "More than one return type not allowed\n", get_line_number());

        seq->ast_push_back(ret);

        current_procedure->set_sequence_ast(*seq);
    }
    }
;

optional_variable_declaration_list:
    {
    if (NOT_ONLY_PARSE)
    {
        $$ = NULL;
    }
    }
|
    variable_declaration_list
    {
    if (NOT_ONLY_PARSE)
    {
        CHECK_INVARIANT(($1 != NULL), "Declaration statement list cannot be null here");

        $$ = $1;
    }
    }
;

variable_declaration_list:
    variable_declaration
    {
    if (NOT_ONLY_PARSE)
    {
        list<Symbol_Table_Entry *> * decl_stmt_list = $1;

        CHECK_INVARIANT((decl_stmt_list != NULL), "Non-terminal declaration statement list cannot be null");

        Symbol_Table * decl_list = new Symbol_Table();

        for (list<Symbol_Table_Entry *>::iterator it = decl_stmt_list->begin();
            it != decl_stmt_list->end(); it++){

            Symbol_Table_Entry * decl_stmt = *it;
            CHECK_INVARIANT((decl_stmt != NULL), "Non-terminal declaration statement cannot be null");

            string decl_name = decl_stmt->get_variable_name();

            if(current_procedure != NULL)
            {
                CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
                    "Variable name cannot be same as procedure name", get_line_number());
            }
            CHECK_INPUT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
                "Variable is declared twice", get_line_number());
            decl_list->push_symbol(decl_stmt);
        }
        delete decl_stmt_list;
        $$ = decl_list;
    }
    }
|
    variable_declaration_list variable_declaration
    {
    if (NOT_ONLY_PARSE)
    {
        // if declaration is local then no need to check in global list
        // if declaration is global then this list is global list

        list<Symbol_Table_Entry *> * decl_stmt_list = $2;

        CHECK_INVARIANT((decl_stmt_list != NULL), "Non-terminal declaration statement list cannot be null");

        Symbol_Table * decl_list = $1;

        for (list<Symbol_Table_Entry *>::iterator it = decl_stmt_list->begin();
            it != decl_stmt_list->end(); it++){

            Symbol_Table_Entry * decl_stmt = *it;
            CHECK_INVARIANT((decl_stmt != NULL), "Non-terminal declaration statement cannot be null");

            string decl_name = decl_stmt->get_variable_name();
            CHECK_INPUT ((program_object.variable_proc_name_check(decl_name) == false),
                "Variable name cannot be same as the procedure name", get_line_number());
            if(current_procedure != NULL)
            {
                CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
                    "Variable name cannot be same as procedure name", get_line_number());
            }
            CHECK_INPUT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
                "Variable is declared twice", get_line_number());
            decl_list->push_symbol(decl_stmt);
        }
        delete decl_stmt_list;
        $$ = decl_list;
    }
    }
;

variable_declaration:
    declaration ';'
    {
    if (NOT_ONLY_PARSE)
    {
        pair<Data_Type, list<string>*> * decl = $1;

        CHECK_INVARIANT((decl != NULL), "Declaration cannot be null");

        Data_Type type = decl->first;
        list<string> * decl_list = decl->second;

        list<Symbol_Table_Entry*> * decl_entry_list = new list<Symbol_Table_Entry*>();

        for (list<string>::iterator it = decl_list->begin();
                it != decl_list->end(); it++){
            decl_entry_list->push_back(new Symbol_Table_Entry(*it, type, get_line_number()));
        }

        $$ = decl_entry_list;

    }
    }
;

declaration:
    INTEGER variable_list
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        CHECK_INVARIANT(($2 != NULL), "Name List cannot be null");

        list<string> * name_list = $2;
        Data_Type type = int_data_type;

        pair<Data_Type, list<string>*> * declar = 
        new pair<Data_Type, list<string>*>(type, name_list);

        $$ = declar;
    }
    }
|
    FLOAT variable_list
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        CHECK_INVARIANT(($2 != NULL), "Name List cannot be null");

        list<string> * name_list = $2;
        Data_Type type = double_data_type;

        pair<Data_Type, list<string>*> * declar = 
        new pair<Data_Type, list<string>*>(type, name_list);

        $$ = declar;
    }
    }
;

variable_list:
    NAME
    {
        CHECK_INVARIANT(($1 != NULL), "Name cannot be null");
        string name = *$1;
        list<string> * retval = new list<string>();
        retval->push_front(name);
        $$ = retval;
    }
|
    NAME ',' variable_list
    {
        CHECK_INVARIANT(($1 != NULL), "Name cannot be null");
        string name = *$1;
        $3->push_front(name);
        $$ = $3;
    }
;

statement_list:
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        Sequence_Ast * stmt_list = new Sequence_Ast(get_line_number());
        $$ = stmt_list;
    }
    }
|
    statement_list assignment_statement
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        $1->ast_push_back($2);
        $$ = $1;
    }
    }
|
    statement_list func_call ';'
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        $1->ast_push_back($2);
        $$ = $1;
    }
    }
|
    statement_list block
    {
        $1->ast_push_back($2);
        $$ = $1;
    }
|
    statement_list return_stmt
    {
        $1->ast_push_back($2);
        $$ = $1;
    }
|
    statement_list print_stmt
    {
        $1->ast_push_back($2);
        $$ = $1;
    }
;
print_stmt:
    PRINT '(' print_argument ')' ';'
    {
        $$ = new Print_Ast( $3, get_line_number());
    }
;

print_argument:
    arith_expression
    {
    if (NOT_ONLY_PARSE){
        $$ = $1;
    }
    }
|
    STRING
    {
        $$ = new String_Ast(*$1, get_line_number());
    }
;
func_call:
    NAME '(' arguments ')'
{
    CHECK_INVARIANT($3!=NULL, "func_call arguments cannot be NULL")
    string func_name = *$1;
    //check if prototype and definition exist in global map
    CHECK_INPUT(program_object.variable_proc_name_check(func_name),
        "Function prototype of the called function cannot be null", get_line_number());
    Procedure * proc = program_object.get_proc(func_name);
    list<Ast*> * args = $3;
    CHECK_INVARIANT(proc!=NULL, "func_call proc can't be NULL");
    CHECK_INVARIANT(args!=NULL, "func_call args can't be NULL");
    int argc=0;
    for(list<Ast*>::iterator it = args->begin(); it!=args->end(); ++it, ++argc){
        CHECK_INPUT((*it)->get_data_type()==proc->get_formal_by_index(argc).get_data_type(),
            "Actual and formal parameters data types are not matching", get_line_number());
    }
    $$ = new Func_Call_Ast(proc, args, get_line_number());
}
;

arguments:
{
    $$ = new list<Ast*>();
}
|
arith_expression
{
    $$ = new list<Ast*>();
    $$->push_back($1);
}
|
arguments ',' arith_expression
{
    $$ = $1;
    $$->push_back($3);
}
;

return_stmt:
    RETURN ';'
    {
        $$ = new Return_Ast(NULL, get_line_number());
    }
|
    RETURN expression_term ';'
    {
        $$ = new Return_Ast($2, get_line_number());
    }
;

block:
    WHILE '(' bool_expression ')' postblock
    {
        $$ = (Ast*) new Iteration_Statement_Ast($3, $5, get_line_number(), false);
        $$->check_ast();
    }
|
    DO postblock WHILE '(' bool_expression ')' ';'
    {
        $$ = (Ast*) new Iteration_Statement_Ast($5, $2, get_line_number(), true);
        $$->check_ast();
    }
|
    IF '(' bool_expression ')' postblock
    {
        Sequence_Ast * empty = new Sequence_Ast(get_line_number());
        $$ = (Ast*) new Selection_Statement_Ast($3, $5, (Ast*)empty, get_line_number());
        $$->check_ast();
    }
|
    IF '(' bool_expression ')' postblock ELSE postblock
    {
        $$ = (Ast*) new Selection_Statement_Ast($3, $5, $7, get_line_number());
        $$->check_ast();
    }
|
    '{' statement_list '}'
    {
        $$ = $2;
    }
;

postblock:
    assignment_statement
    {
        $$ = $1;
    }
|
    block
    {
        $$ = $1;
    }
|
    return_stmt
    {
        $$ = $1;
    }
    print_stmt
    {
        $$ = $1;
    }
;

// Make sure to call check_ast in assignment_statement and arith_expression
// Refer to error_display.hh for displaying semantic errors if any
assignment_statement:
    variable ASSIGN arith_expression ';'
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        $$ = new Assignment_Ast($1, $3, get_line_number());
        $$->check_ast();
    }
    }
;

arith_expression:
    //ADD RELEVANT CODE ALONG WITH GRAMMAR RULES HERE
    // SUPPORT binary +, -, *, / operations, unary -, and allow parenthesization
    // i.e. E -> (E)
    // Connect the rules with the remaining rules given below
    operand '+' operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Plus_Ast($1, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand '-' operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Minus_Ast($1, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand '*' operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Mult_Ast($1, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand '/' operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Divide_Ast($1, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    '-' operand %prec UMINUS
    {
        if (NOT_ONLY_PARSE){
            $$ = new UMinus_Ast($2, NULL, get_line_number());
            $$->check_ast();
        }
    }
|
    '(' operand ')'
    {
        if (NOT_ONLY_PARSE){
            $$ = $2;
        }
    }
|
    expression_term
    {
        if (NOT_ONLY_PARSE){
            $$ = (Arithmetic_Expr_Ast *) $1;
        }
    }
|
    bool_expression QMARK operand COLON operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Conditional_Operator_Ast($1, $3, $5, get_line_number());
            $$->check_ast();
        }
    }
|
    func_call
    {
        if (NOT_ONLY_PARSE){
            $$ = new Arith_Func_Call($1, NULL, get_line_number());
            $$->check_ast();
        }
    }
;

bool_expression:
    operand LT operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Relational_Expr_Ast($1, less_than, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand LTE operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Relational_Expr_Ast($1, less_equalto, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand GT operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Relational_Expr_Ast($1, greater_than, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand GTE operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Relational_Expr_Ast($1, greater_equalto, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand NEQ operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Relational_Expr_Ast($1, not_equalto, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    operand EQ operand
    {
        if (NOT_ONLY_PARSE){
            $$ = new Relational_Expr_Ast($1, equalto, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    bool_expression AND bool_expression
    {
        if (NOT_ONLY_PARSE){
            $$ = new Boolean_Expr_Ast($1, boolean_and, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    bool_expression OR bool_expression
    {
        if (NOT_ONLY_PARSE){
            $$ = new Boolean_Expr_Ast($1, boolean_or, $3, get_line_number());
            $$->check_ast();
        }
    }
|
    NOT bool_expression
    {
        if (NOT_ONLY_PARSE){
            $$ = new Boolean_Expr_Ast(NULL, boolean_not, $2, get_line_number());
            $$->check_ast();
        }
    }
|
    '(' bool_expression ')'
    {
        $$ = $2;
    }
;

operand:
    arith_expression
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        CHECK_INVARIANT($1!=NULL, "q1");
        $$ = $1;
    }
    }
;

expression_term:
    variable
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        CHECK_INVARIANT($1!=NULL, "q2");
        $$=$1;
    }
    }
|
    constant
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        CHECK_INVARIANT($1!=NULL, "q3");
        $$=$1;
    }
    }
;

variable:
    NAME
    {
    if (NOT_ONLY_PARSE)
    {
        Symbol_Table_Entry * var_table_entry;

        CHECK_INVARIANT(($1 != NULL), "Variable name cannot be null");

        if (current_procedure->variable_in_symbol_list_check(*$1))
             var_table_entry = &(current_procedure->get_symbol_table_entry(*$1));

        else if (program_object.variable_in_symbol_list_check(*$1))
            var_table_entry = &(program_object.get_symbol_table_entry(*$1));

        else
            CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Variable has not been declared", get_line_number());

        $$ = new Name_Ast(*$1, *var_table_entry, get_line_number());
        delete $1;
    }
    }
;

constant:
    INTEGER_NUMBER
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        $$ = new Number_Ast<int>($1, int_data_type, get_line_number());
    }
    }
|
    DOUBLE_NUMBER
    {
    if (NOT_ONLY_PARSE)
    {
        //ADD CODE HERE
        $$ = new Number_Ast<double>($1, double_data_type, get_line_number());
    }
    }
;
