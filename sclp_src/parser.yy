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
    Name_Ast * name_ast;
    Assignment_Ast * assign_ast;
    Arithmetic_Expr_Ast * arith_ast;
    Sequence_Ast * sequence_Ast;
    list<string> * list_string;
    pair<Data_Type, list<string>*> * new_decl;
};

//ADD TOKENS HEREs
%token <int_value> INTEGER_NUMBER
%token <float_value> DOUBLE_NUMBER
%token <string_value> NAME
%token VOID INTEGER FLOAT ASSIGN WHILE IF DO ELSE
%token FOR GOTO SWITCH COLON QMARK
%token LT LTE GT GTE EQ NEQ AND OR NOT

%left '+' '-'
%left '*' '/'
%right UMINUS
%nonassoc '('

%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> variable_declaration_list
%type <list_symbol_entry> variable_declaration
%type <new_decl> declaration
//ADD CODE HERE
%type <ast> variable
%type <ast> constant
%type <ast> operand
%type <ast> expression_term
%type <ast> assignment_statement
%type <ast> arith_expression
%type <sequence_Ast> statement_list
%type <list_string> variable_list

%start program

%%

program:
    declaration_list procedure_definition
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
    procedure_declaration
    {
    if (NOT_ONLY_PARSE)
    {
        Symbol_Table * global_table = new Symbol_Table();
        program_object.set_global_table(*global_table);
    }
    }
|
    variable_declaration_list
    procedure_declaration
    {
    if (NOT_ONLY_PARSE)
    {
        Symbol_Table * global_table = $1;

        CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

        program_object.set_global_table(*global_table);
    }
    }
|
    procedure_declaration
    variable_declaration_list
    {
    if (NOT_ONLY_PARSE)
    {
        Symbol_Table * global_table = $2;

        CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

        program_object.set_global_table(*global_table);
    }
    }
;

procedure_declaration:
    VOID NAME '(' ')' ';'
    {
    if (NOT_ONLY_PARSE)
    {
        CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
        CHECK_INVARIANT((*$2 == "main"), "Procedure name must be main in declaration");
    }
    }
;

procedure_definition:
    NAME '(' ')'
    {
    if (NOT_ONLY_PARSE)
    {
        CHECK_INVARIANT(($1 != NULL), "Procedure name cannot be null");
        CHECK_INVARIANT((*$1 == "main"), "Procedure name must be main");

        string proc_name = *$1;

        current_procedure = new Procedure(void_data_type, proc_name, get_line_number());

        CHECK_INPUT ((program_object.variable_in_symbol_list_check(proc_name) == false),
            "Procedure name cannot be same as global variable", get_line_number());
    }
    }

    '{' optional_variable_declaration_list
    {
    if (NOT_ONLY_PARSE)
    {

        CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

        Symbol_Table * local_table = $6;

        if (local_table == NULL)
            local_table = new Symbol_Table();

        current_procedure->set_local_list(*local_table);
    }
    }

    statement_list '}'
    {
    if (NOT_ONLY_PARSE)
    {
        Sequence_Ast* seq = $8;

        CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");
        CHECK_INVARIANT((seq != NULL), "statement list cannot be null");

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
            CHECK_INPUT ((program_object.variable_proc_name_check(decl_name) == false),
                "Variable name cannot be same as the procedure name", get_line_number());
            if(current_procedure != NULL)
            {
                CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
                    "Variable name cannot be same as procedure name", get_line_number());
            }
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
    statement_list block
    {
        
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
            $$ = (Arithmetic_Expr_Ast *) $2;
            $$->check_ast();
        }
    }
|
    expression_term
    {
        if (NOT_ONLY_PARSE){
            $$ = (Arithmetic_Expr_Ast *) $1;
        }
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
