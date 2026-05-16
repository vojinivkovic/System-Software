%{
#include "../assembler.hpp"
#include "../aux/instructions_functions.hpp"
#include "../aux/directive_functions.hpp"
#include "../aux/expression_tokens.hpp"
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

static int resolveLiteral(const std::string& literal)
{
  int operand;
  size_t pos = 0;
  if(literal[pos] == '-' || literal[0] == '+')
  {
    pos++;
  }

  switch (literal[pos + 1])
  {
    case 'x': 
    case 'X':
      operand = stoi(literal, nullptr, 16);
      break;
    case 'b':
    case 'B':
      if(literal[0] == '-')
      {
        operand = - stoi(literal.substr(3), nullptr, 2);
      }
      else if (literal[0] == '+')
      {
        operand = stoi(literal.substr(3), nullptr, 2);
      }
      else
      {
        operand = stoi(literal.substr(2), nullptr, 2);
      }
      break;
    default:
      operand = stoi(literal, nullptr, 10);
      break;
    }
    return operand;
}

std::vector<Argument> args;
std::vector<MacroParameter> params;
std::string instr;
std::string directive;
%}


%start line

%union {char* field;}
%token<field> DIRECTIVE EQU COMMAND GP_REGISTER CS_REGISTER SYMBOL LABEL STRING LITERAL END
%type<field> line assembly_directive assembly_command  equ_directive
%type<field> list_of_parameters signed_literal signed_symbol 
%type<Argument> operand
%type<std::vector<Token>> expression base term signed_term
%left '+' '-'
%right UMINUS UPLUS

%%
line : assembly_directive
     | assembly_command
     | LABEL {
          Assembler::getCurrentSection()->defineSymbol($1);
          free($1);              
     }
     assembly_directive
     | LABEL {
          Assembler::getCurrentSection()->defineSymbol($1);
          free($1);              
     }
     assembly_command
     | END { YYACCEPT; }
     | error '\n'
     ;

assembly_directive : DIRECTIVE {
                         directive = $1;
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         free($1);
                    }
                   | DIRECTIVE list_of_parameters {
                         directive = $1;
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         params.clear();
                         free($1);
                    }
                   | DIRECTIVE STRING {
                         directive = $1;
                         params.push_back(MacroParameter{MacroParameterType::String, $2, 0, {}});
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         params.clear();
                         free($1);
                         free($2);
                    }

                   | equ_directive
                   ;

equ_directive: EQU SYMBOL ',' expression {
                         directive = $1;
                         params.push_back(MacroParameter{MacroParameterType::Symbol, $2, 0, {}});
                         params.push_back(MacroParameter{MacroParameterType::Expression, "", 0, $3});
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         params.clear();
                         free($1);
                         free($2);
                    }

list_of_parameters : SYMBOL {
                         params.push_back(MacroParameter{MacroParameterType::Symbol, $1, 0, {}});
                         free($1);
                    }    
                   | signed_literal {
                         int value = resolveLiteral($1);
                         free($1);
                         params.push_back(MacroParameter{MacroParameterType::Literal, "", value, {}});
                   }
                   | list_of_parameters ',' SYMBOL {
                         params.push_back(MacroParameter{MacroParameterType::Symbol, $3, 0, {}});
                         free($3);
                    } 
                   | list_of_parameters ',' signed_literal {
                         int value = resolveLiteral($3);
                         free($3);
                         params.push_back(MacroParameter{MacroParameterType::Literal, "", value, {}});
                   }
                   ;

expression : term {
               $$ = std::move($1);
          }
           | expression '+' term {
               $$ = std::move($1);
               $$.push_back(Token{TokenType::BINPLUS, 0, ""});
               $$.insert($$.end(), $3.begin(), $3.end());

           }
           | expression '-' term {
               $$ = std::move($1);
               $$.push_back(Token{TokenType::BINMINUS, 0, ""});
               $$.insert($$.end(), $3.begin(), $3.end());
           }
           ;

term : SYMBOL {
          $$ = {Token{TokenType::SYMBOL, 0, $1}};
          free($1);
     }
     | LITERAL {
          int value = resolveLiteral($1);
          $$ = {Token{TokenType::LITERAL, value, ""}};
          free($1);
     }
     | '(' expression ')' {
          $$ = {Token{TokenType::LPARENTHESES, 0,  ""}};
          $$.insert($$.end(), $2.begin(), $2.end());
          $$.push_back(Token{TokenType::RPARENTHESES, 0, ""});
     }
     | signed_term {
          $$ = std::move($1);
     }
     ;

signed_term : '+' base {
               $$ = {Token{TokenType::UNPLUS, 0, ""}};
               $$.insert($$.end(), $2.begin(), $2.end());
               }
            | '-' base {
               $$ = {Token{TokenType::UNMINUS, 0, ""}};
               $$.insert($$.end(), $2.begin(), $2.end());
               }
            ;

base : SYMBOL {
          $$ = {Token{TokenType::SYMBOL, 0, $1}};
          free($1);
     }
     | LITERAL {
          int value = resolveLiteral($1);
          $$ = {Token{TokenType::LITERAL, value, ""}};
          free($1);
     }
     | '(' expression ')' {
          $$ = {Token{TokenType::LPARENTHESES, 0,  ""}};
          $$.insert($$.end(), $2.begin(), $2.end());
          $$.push_back(Token{TokenType::RPARENTHESES, 0, ""});
     }
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

