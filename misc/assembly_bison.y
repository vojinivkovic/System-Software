%{
#include "../assembler.hpp"
#include "../aux/instructions_functions.hpp"
#include <string.h>
#include <cstdint>
#include <cstdlib>

int yylex(void);
int yywrap();
int yyerror();


static uint8_t parseRegister(const char* variable)
{
     if(variable == "sp")
     {    
          return 14;
     }
     else if (variable == "pc")
     {
          return 15;
     }
     else
     {
          return static_cast<uint8_t>(std::stoul(variable));
     }
}

static uint8_t parseSPRegister(const char* variable)
{
     if(variable == "status")
     {
          return 0;
     }
     else if(variable == "handler")
     {
          return 1;
     }
     else 
     {
          return 2;
     }
}

std::vector<Argument> args;
std::string instr;
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

signed_literal : LITERAL { 
                         $$ = $1;
                    }
               | '-' LITERAL {
                         size_t len = strlen($2);
                         $$ = (char*)malloc((len + 2) * sizeof(char));
                         $$[0] = '-';
                         strcpy($$ + 1, $2);

                         free($2);
                    }
               | '+' LITERAL {
                         size_t len = strlen($2);
                         $$ = (char*)malloc((len + 2) * sizeof(char));
                         $$[0] = '+';
                         strcpy($$ + 1, $2);

                         free($2);
                    }

               ;

signed_symbol : SYMBOL { $$ = $1;}
              | '-' SYMBOL {
                         size_t len = strlen($2);
                         $$ = (char*)malloc((len + 2) * sizeof(char));
                         $$[0] = '-';
                         strcpy($$ + 1, $2);

                         free($2);
                    }
              ;

operand : LITERAL {
                    $$ = Argument{ArgumentType::OperandLiteral, 
                    AddressingType::MemoryDirect, 0, $1}; 
                    free($1);
                }
        | SYMBOL {
                    $$ = Argument{ArgumentType::OperandSymbol, 
                    AddressingType::MemoryDirect, 0, $1}; 
                    free($1);
               }
        | '$' LITERAL {
                    $$ = Argument{ArgumentType::OperandLiteral, 
                    AddressingType::Immediate, 0, $2}; 
                    free($2);
               }
        | '$' signed_literal {
               $$ = Argument{ArgumentType::OperandLiteral, 
                    AddressingType::Immediate, 0, $2}; 
                    free($2);
               }
        | '$' SYMBOL {
               $$ = Argument{ArgumentType::OperandSymbol, 
                    AddressingType::Immediate, 0, $2}; 
                    free($2);
               }
        | GP_REGISTER {
               uint8_t registerNum = parseRegister($1);
               
               $$ = Argument{ArgumentType::Register, 
                    AddressingType::RegisterDirect, registerNum, ""}; 
                    free($1); 
               }
        | '[' GP_REGISTER ']' {
               uint8_t registerNum = parseRegister($2);
               
               $$ = Argument{ArgumentType::Register, 
                    AddressingType::RegisterIndirect, registerNum, ""}; 
                    free($2); 
               }

        | '[' GP_REGISTER '+' SYMBOL ']' {
               uint8_t registerNum = parseRegister($2);
               
               $$ = Argument{ArgumentType::RegisterAndSymbol, 
                    AddressingType::RegisterIndirect, registerNum, $4}; 
                    free($2);
                    free(%4); 
               }
        | '[' GP_REGISTER '+' LITERAL ']' {
               uint8_t registerNum = parseRegister($2);
               
               $$ = Argument{ArgumentType::RegisterAndLiteral, 
                    AddressingType::RegisterIndirect, registerNum, $4}; 
                    free($2);
                    free(%4); 
               }
          
        ;

assembly_command : COMMAND {
                         instr = $1;
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                    }
                 | COMMAND operand { 
                         instr = $1;
                         args.push_back($2);
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                         args.clear();
                    }
                 | COMMAND GP_REGISTER {
                         instr = $1;
                         registerNum = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                         free($2);
                         args.clear();
                    }
                 | COMMAND operand GP_REGISTER {
                         instr = $1;
                         registerNum = parseRegister($3);
                         args.push_back($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                         free($3);
                         args.clear();
                    }
                 | COMMAND GP_REGISTER operand {
                         instr = $1;
                         registerNum = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         args.push_back($3);
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         args.clear();
                    }
                 | COMMAND GP_REGISTER GP_REGISTER {
                         instr = $1;

                         registerNum1 = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         registerNum2 = parseRegister($3);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($3);
                         args.clear();
                    }
                 | COMMAND CS_REGISTER GP_REGISTER {
                         instr = $1;

                         registerNum1 = parseSPRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         registerNum2 = parseRegister($3);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($3);
                         args.clear();
                    }

                 | COMMAND GP_REGISTER CS_REGISTER {
                         instr = $1;

                         registerNum1 = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         registerNum2 = parseSPRegister($3);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($3);
                         args.clear();
                    }
                 | COMMAND GP_REGISTER GP_REGISTER operand {
                    {
                         instr = $1;

                         registerNum1 = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         registerNum2 = parseRegister($3);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});

                         args.push_back($4);
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($3);
                         args.clear();
                    }
                 }
                 ;

%%

