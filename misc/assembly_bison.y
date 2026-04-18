%{
int yylex(void);
int yywrap();
int yyerror();
%}


%start line

%union {char* field;}
%token<field> DIRECTIVE EQU COMMAND GP_REGISTER CS_REGISTER SYMBOL LABEL STRING LITERAL
%type<field> line assembly_directive assembly_command expression equ_directive
%type<field> list_of_operands
%left '+' '-'


%%
line : assembly_directive
     | assembly_command
     | LABEL assembly_directive
     | LABEL assembly_command
     | error '\n'
     ;

assembly_directive : DIRECTIVE
                   | DIRECTIVE list_of_operands
                   | DIRECTIVE STRING
                   | equ_directive
                   ;

equ_directive: DIRECTIVE SYMBOL ',' expression;

list_of_operands : expression
                 | list_of_operands ',' expression
                 ;


expression : SYMBOL
           | LITERAL
           | expression '+' expression
           | expression '-' expression 
           ;

assembly_command : COMMAND
                 | COMMAND list_of_operands
                 | COMMAND GP_REGISTER
                 | COMMAND list_of_operands GP_REGISTER
                 | COMMAND GP_REGISTER list_of_operands
                 | COMMAND GP_REGISTER GP_REGISTER
                 | COMMAND CS_REGISTER GP_REGISTER
                 | COMMAND GP_REGISTER CS_REGISTER
                 | COMMAND GP_REGISTER GP_REGISTER list_of_operands
                 ;

%%

