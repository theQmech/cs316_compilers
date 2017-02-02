%filenames="scanner"
%lex-source="scanner.cc"

alpha [a-zA-Z]
digit [0-9]
meta [(){};]


%%

// use store_token_name(<TOKEN>) to correctly generate tokens file

void    { return Parser::VOID; }
int     { return Parser::INTEGER; }
float   { return Parser::FLOAT; }

"="     { return Parser::ASSIGNOP; }
[-+*/]  { return Parser::ARITHOP; }

{meta}  {
    return matched()[0];
}

[-]?{digit}+  {
    ParserBase::STYPE__ *val = getSval();
    val->int_value = atoi(matched().c_str());
    return Parser::INTNUM; 
}

[-+]?({digit}*\.{digit}+)(e[-+]?{digit}+)?    {
    ParserBase::STYPE__ *val = getSval();
    val->float_value = atoi(matched().c_str());
    return Parser::FNUM; 
}

({alpha}|\_)({alpha}|{digit}|\_)*    { 
    ParserBase::STYPE__ *val = getSval();
    val->string_value = new std::string(matched());
    return Parser::NAME; 
}

\n              |
";;".*          |
[ \t]*";;".*    |
[ \t]*"//".*    |
[ \t]   {
    if (command_options.is_show_tokens_selected())
        ignore_token();
}

.   { 
    string error_message;
    error_message =  "Illegal character `" + matched();
    error_message += "' on line " + lineNr();

    CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, error_message, lineNr());
}
