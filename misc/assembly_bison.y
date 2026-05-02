%{
int yylex(void);
int yywrap();
int yyerror();
%}


%start line

%union {char* field;}
%token<field> DIRECTIVE EQU COMMAND GP_REGISTER CS_REGISTER SYMBOL LABEL STRING LITERAL END
%type<field> line assembly_directive assembly_command expression equ_directive
%type<field> list_of_parameters operand signed_literal signed_symbol base term signed_term
%left '+' '-'
%right UMINUS UPLUS

%%
line : assembly_directive
     | assembly_command
     | LABEL assembly_directive
     | LABEL assembly_command
     | END { YYACCEPT; }
     | error '\n'
     ;

assembly_directive : DIRECTIVE
                   | DIRECTIVE list_of_parameters
                   | DIRECTIVE STRING
                   | equ_directive
                   ;

equ_directive: EQU SYMBOL ',' expression;

list_of_parameters : signed_symbol
                   | signed_literal
                   | list_of_parameters ',' signed_symbol
                   | list_of_parameters ',' signed_literal
                   ;

expression : term
           | expression '+' term
           | expression '-' term
           ;

term : SYMBOL
     | LITERAL
     | '(' expression ')'
     | signed_term
     ;

signed_term : '+' base
            | '-' base
            ;

base : SYMBOL
     | LITERAL
     | '(' expression ')'
     ;

signed_literal : LITERAL
               | '-' LITERAL
               ;

signed_symbol : SYMBOL
              | '-' SYMBOL
              ;

operand : LITERAL
        | SYMBOL
        | '$' LITERAL
        | '$' signed_literal
        | '$' SYMBOL
        | GP_REGISTER
        | '[' GP_REGISTER ']'
        | '[' GP_REGISTER '+' SYMBOL ']'
        | '[' GP_REGISTER '+' LITERAL ']'
        ;

assembly_command : COMMAND
                 | COMMAND operand
                 | COMMAND GP_REGISTER
                 | COMMAND operand GP_REGISTER
                 | COMMAND GP_REGISTER operand
                 | COMMAND GP_REGISTER GP_REGISTER
                 | COMMAND CS_REGISTER GP_REGISTER
                 | COMMAND GP_REGISTER CS_REGISTER
                 | COMMAND GP_REGISTER GP_REGISTER operand
                 ;

%%

