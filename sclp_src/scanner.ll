%filenames="scanner"
%lex-source="scanner.cc"

alpha [a-zA-Z]
digit [0-9]
meta [(){};,]


%%

// use store_token_name(<TOKEN>) to correctly generate tokens file
//for     { store_token_name("FOR"); return Parser::FOR; }
//switch  { store_token_name("SWITCH"); return Parser::SWITCH; }
//goto    { store_token_name("GOTO"); return Parser::GOTO; }

int     { store_token_name("INTEGER"); return Parser::INTEGER; }
float   { store_token_name("FLOAT"); return Parser::FLOAT; }
void    { store_token_name("VOID"); return Parser::VOID; }
while   { store_token_name("WHILE"); return Parser::WHILE; }
do      { store_token_name("DO"); return Parser::DO; }
if      { store_token_name("IF"); return Parser::IF; }
else    { store_token_name("ELSE"); return Parser::ELSE; }

"="     { store_token_name("ASSIGN_OP"); return Parser::ASSIGN; }
"?"     { store_token_name("QMARK"); return Parser::QMARK; }
":"     { store_token_name("COLON"); return Parser::COLON; }
[-+*/]  { store_token_name("ARITHOP"); return matched()[0]; }

"<"     { store_token_name("LT"); return Parser::LT; }
"<="    { store_token_name("LTE"); return Parser::LTE; }
">"     { store_token_name("GT"); return Parser::GT; }
">="    { store_token_name("GTE"); return Parser::GTE; }
"=="    { store_token_name("EQ"); return Parser::EQ; }
"!="    { store_token_name("NEQ"); return Parser::NEQ; }

"&&"    { store_token_name("AND"); return Parser::AND; }
"||"    { store_token_name("OR"); return Parser::OR; }
"!"     { store_token_name("NOT"); return Parser::NOT; }

{meta}  { store_token_name("META CHAR"); return matched()[0]; }

{digit}+  {
    store_token_name("NUM");
    ParserBase::STYPE__ *val = getSval();
    val->int_value = atoi(matched().c_str());
    return Parser::INTEGER_NUMBER; 
}

(({digit}*\.{digit}+)|({digit}+\.{digit}*)|({digit}+))([eE][-+]?{digit}+)?    {
    store_token_name("FNUM");
    ParserBase::STYPE__ *val = getSval();
    val->float_value = atof(matched().c_str());
    return Parser::DOUBLE_NUMBER; 
}

({alpha}|\_)({alpha}|{digit}|\_)*    { 
    store_token_name("NAME");
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

^"//"*$ {
    if (command_options.is_show_tokens_selected())
        ignore_token();

}

.   { 
    string error_message;
    error_message =  "Illegal character `" + matched();
    error_message += "' on line " + lineNr();

    CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, error_message, lineNr());
}
