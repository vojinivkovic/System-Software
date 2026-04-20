%{
int yylex(void);
int yywrap();
int yyerror();
%}


%start line

%union {char* field;}
%token<field> DIRECTIVE EQU COMMAND GP_REGISTER CS_REGISTER SYMBOL LABEL STRING LITERAL END
%type<field> line assembly_directive assembly_command expression equ_directive
%type<field> list_of_parameters operand
%left '+' '-'


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

equ_directive: DIRECTIVE SYMBOL ',' expression;

list_of_parameters : expression
                 | parameters ',' expression
                 ;


expression : SYMBOL
           | LITERAL
           | expression '+' expression
           | expression '-' expression 
           ;

operand : LITERAL
        | SYMBOL
        | '$' LITERAL
        | '$' SYMBOL
        | GP_REGISTER
        | '[' GP_REGISTER ']'
        | '[' GP_REGISTER '+' LITERAL ']'
        | '[' GP_REGISTER '+' SYMBOL ']'


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

