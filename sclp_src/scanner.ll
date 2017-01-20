%filenames="scanner"
%lex-source="scanner.cc"

alpha [a-zA-Z]
digit [0-9]
paranthesis [(){}]

%%
//ADD YOUR CODE HERE

void    { return Parser::VOID; }
int     { return Parser::INTEGER; }
=       { return Parser::ASSIGN; }
return  { return Parser::RETURN; }

[-]?{digit}+  {
            ParserBase::STYPE__ *val = getSval();
            val->integer_value = atoi(matched().c_str());
            return Parser::INTEGER_NUMBER; 
}

({alpha}|\_)({alpha}|{digit}|\_)*    { 
            ParserBase::STYPE__ *val = getSval();
            val->string_value = new std::string(matched());
            return Parser::NAME; 
}

{paranthesis}   {
    return matched()[0];
}

\;  {return matched()[0];}

\n          |
";;".*      |
[ \t]*";;".*    |
[ \t]*"//".*    |
[ \t]       {
            if (command_options.is_show_tokens_selected())
                ignore_token();
        }

.       { 
            string error_message;
            error_message =  "Illegal character `" + matched();
            error_message += "' on line " + lineNr();
            
            CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, error_message, lineNr());
        }
