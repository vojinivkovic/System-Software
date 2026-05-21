%{
#include "assembler.hpp"
#include "aux/instructions_functions.hpp"
#include "aux/directive_functions.hpp"
#include "aux/expression_tokens.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>

int yylex(void);
int yywrap();
int yyerror(const char* s);


static uint8_t parseRegister(const char* variable)
{
     if(std::strcmp(variable, "sp") == 0)
     {    
          return 14;
     }
     else if (std::strcmp(variable, "pc") == 0)
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
     if(std::strcmp(variable, "status") == 0)
     {
          return 0;
     }
     else if(std::strcmp(variable, "handler") == 0)
     {
          return 1;
     }
     else 
     {
          return 2;
     }
}

static std::uint32_t resolveLiteral(const std::string& literal)
{
     if (literal.empty()) 
     {
          throw std::invalid_argument("Empty literal");
     }

    std::size_t pos = 0;
    bool negative = false;

    if (literal[pos] == '+' || literal[pos] == '-') 
    {
          negative = (literal[pos] == '-');
          pos++;
          if (pos >= literal.size()) 
          {
               throw std::invalid_argument("Literal contains only a sign");
          }
     }

    int base = 10;
    std::size_t digitsPos = pos;

    if (pos + 1 < literal.size() &&
        literal[pos] == '0' &&
        (literal[pos + 1] == 'x' || literal[pos + 1] == 'X'))
    {
        base = 16;
        digitsPos = pos + 2;
    }

    else if (pos + 1 < literal.size() &&
             literal[pos] == '0' &&
             (literal[pos + 1] == 'b' || literal[pos + 1] == 'B'))
    {
        base = 2;
        digitsPos = pos + 2;
    }

    if (digitsPos >= literal.size()) {
        throw std::invalid_argument("Missing digits in literal: " + literal);
    }

    std::uint64_t value;
    if (base == 2) 
    {
        value = std::stoull(literal.substr(digitsPos), nullptr, 2);
    }
    else
    {
          value = std::stoull(literal, nullptr, base);
    }

    if(negative)
    {
          value = static_cast<std::uint64_t>(static_cast<std::uint32_t>(-static_cast<std::int64_t>(value)));
    }

    if (value > std::numeric_limits<std::uint32_t>::max()) 
    {
        throw std::out_of_range("Literal does not fit in 32 bits: " + literal);
    }
    return static_cast<std::uint32_t>(value);
}

std::vector<Argument> args;
std::vector<MacroParameter> params;
std::string instr;
std::string directive;
%}


%start program

%code requires {
    #include <vector>
    #include "aux/expression_tokens.hpp"
    #include "aux/instructions_functions.hpp"
}

%union {
    char* field;
    Argument* argument;
    std::vector<Token>* tokenVector;
}

%debug

%token<field> DIRECTIVE EQU COMMAND GP_REGISTER CS_REGISTER SYMBOL LABEL STRING LITERAL END
%type<field> line assembly_directive assembly_command equ_directive
%type<field> list_of_parameters signed_literal
%type<argument> operand
%type<tokenVector> expression base term signed_term
%left '+' '-'
%right UMINUS UPLUS

%%
program
    : /* empty */
    | program '\n'
    | program line '\n'
    | program END '\n'   { YYACCEPT; }
    ;

line : assembly_directive
     | assembly_command
     | LABEL {
          Assembler::getCurrentSection()->defineSymbol($1);
          free($1);
      }
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
                         params.push_back(MacroParameter{MacroParameterType::Expression, "", 0, std::move(*$4)});
                         Assembler::getCurrentSection()->executeDirective(directive, params);
                         params.clear();
                         free($1);
                         free($2);
                         delete $4;
                    }

list_of_parameters : SYMBOL {
                         params.push_back(MacroParameter{MacroParameterType::Symbol, $1, 0, {}});
                         free($1);
                    }    
                   | signed_literal {
                         std::uint32_t raw = resolveLiteral($1);
                         std::int32_t value = static_cast<std::int32_t>(raw); 
                         free($1);
                         params.push_back(MacroParameter{MacroParameterType::Literal, "", value, {}});
                   }
                   | list_of_parameters ',' SYMBOL {
                         params.push_back(MacroParameter{MacroParameterType::Symbol, $3, 0, {}});
                         free($3);
                    } 
                   | list_of_parameters ',' signed_literal {
                         std::uint32_t raw = resolveLiteral($3);
                         std::int32_t value = static_cast<std::int32_t>(raw); 
                         free($3);
                         params.push_back(MacroParameter{MacroParameterType::Literal, "", value, {}});
                   }
                   ;

expression : term {
               $$ = $1;
          }
           | expression '+' term {
               $$ = $1;
               $$->push_back(Token{TokenType::BINPLUS, 0, ""});
               $$->insert($$->end(), $3->begin(), $3->end());
               delete $3;
           }
           | expression '-' term {
               $$ = $1;
               $$->push_back(Token{TokenType::BINMINUS, 0, ""});
               $$->insert($$->end(), $3->begin(), $3->end());
               delete $3;
           }
           ;

term : SYMBOL {
          $$ = new std::vector<Token>{Token{TokenType::SYMBOL, 0, $1}};
          free($1);
     }
     | LITERAL {
          std::uint32_t raw = resolveLiteral($1);
          std::int32_t value = static_cast<std::int32_t>(raw); 
          $$ = new std::vector<Token>{Token{TokenType::LITERAL, value, ""}};
          free($1);
     }
     | '(' expression ')' {
          $$ = new std::vector<Token>{Token{TokenType::LPARENTHESES, 0,  ""}};
          $$->insert($$->end(), $2->begin(), $2->end());
          $$->push_back(Token{TokenType::RPARENTHESES, 0, ""});
          delete $2;
     }
     | signed_term {
          $$ = $1;
     }
     ;

signed_term : '+' base {
               $$ = new std::vector<Token>{Token{TokenType::UNPLUS, 0, ""}};
               $$->insert($$->end(), $2->begin(), $2->end());
               delete $2;
               }
            | '-' base {
               $$ = new std::vector<Token>{Token{TokenType::UNMINUS, 0, ""}};
               $$->insert($$->end(), $2->begin(), $2->end());
               delete $2;
               }
            ;

base : SYMBOL {
          $$ = new std::vector<Token>{Token{TokenType::SYMBOL, 0, $1}};
          free($1);
     }
     | LITERAL {
          std::uint32_t raw = resolveLiteral($1);
          std::int32_t value = static_cast<std::int32_t>(raw); 
          $$ = new std::vector<Token>{Token{TokenType::LITERAL, value, ""}};
          free($1);
     }
     | '(' expression ')' {
          $$ = new std::vector<Token>{Token{TokenType::LPARENTHESES, 0,  ""}};
          $$->insert($$->end(), $2->begin(), $2->end());
          $$->push_back(Token{TokenType::RPARENTHESES, 0, ""});
          delete $2;
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
                    $$ = new Argument{ArgumentType::OperandLiteral, 
                    AddressingType::MemoryDirect, 0, $1}; 
                    free($1);
                }
        | SYMBOL {
                    $$ = new Argument{ArgumentType::OperandSymbol, 
                    AddressingType::MemoryDirect, 0, $1}; 
                    free($1);
               }
        | '$' LITERAL {
                    $$ = new Argument{ArgumentType::OperandLiteral, 
                    AddressingType::Immediate, 0, $2}; 
                    free($2);
               }
        | '$' signed_literal {
               $$ = new Argument{ArgumentType::OperandLiteral, 
                    AddressingType::Immediate, 0, $2}; 
                    free($2);
               }
        | '$' SYMBOL {
               $$ = new Argument{ArgumentType::OperandSymbol, 
                    AddressingType::Immediate, 0, $2}; 
                    free($2);
               }
        | GP_REGISTER {
               uint8_t registerNum = parseRegister($1);
               
               $$ = new Argument{ArgumentType::Register, 
                    AddressingType::RegisterDirect, registerNum, ""}; 
                    free($1); 
               }
        | '[' GP_REGISTER ']' {
               uint8_t registerNum = parseRegister($2);
               
               $$ = new Argument{ArgumentType::Register, 
                    AddressingType::RegisterIndirect, registerNum, ""}; 
                    free($2); 
               }

        | '[' GP_REGISTER '+' SYMBOL ']' {
               uint8_t registerNum = parseRegister($2);
               
               $$ = new Argument{ArgumentType::RegisterAndSymbol, 
                    AddressingType::RegisterIndirect, registerNum, $4}; 
                    free($2);
                    free($4); 
               }
        | '[' GP_REGISTER '+' LITERAL ']' {
               uint8_t registerNum = parseRegister($2);
               
               $$ = new Argument{ArgumentType::RegisterAndLiteral, 
                    AddressingType::RegisterIndirect, registerNum, $4}; 
                    free($2);
                    free($4); 
               }
          
        ;

assembly_command : COMMAND {
                         args.clear();
                         instr = $1;
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                         
                    }
                 | COMMAND operand { 
                         instr = $1;
                         args.push_back(*$2);
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                         delete $2;
                         args.clear();
                    }
                 | COMMAND GP_REGISTER {
                         instr = $1;
                         uint8_t registerNum = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                         free($2);
                         args.clear();
                    }
                 | COMMAND operand ',' GP_REGISTER {
                         instr = $1;
                         uint8_t registerNum = parseRegister($4);
                         args.push_back(*$2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         free($1);
                         free($4);
                         delete $2;
                         args.clear();
                    }
                 | COMMAND GP_REGISTER ',' operand {
                         instr = $1;
                         uint8_t registerNum = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum, ""});
                         args.push_back(*$4);
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         delete $4;
                         args.clear();
                    }
                 | COMMAND GP_REGISTER ',' GP_REGISTER {
                         instr = $1;

                         uint8_t registerNum1 = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                        uint8_t registerNum2 = parseRegister($4);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($4);
                         args.clear();
                    }
                 | COMMAND CS_REGISTER ',' GP_REGISTER {
                         instr = $1;

                         uint8_t registerNum1 = parseSPRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         uint8_t registerNum2 = parseRegister($4);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($4);
                         args.clear();
                    }

                 | COMMAND GP_REGISTER ',' CS_REGISTER {
                         instr = $1;

                         uint8_t registerNum1 = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         uint8_t registerNum2 = parseSPRegister($4);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});
                         
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($4);
                         args.clear();
                    }
                 | COMMAND GP_REGISTER ',' GP_REGISTER ',' operand {
                         instr = $1;

                         uint8_t registerNum1 = parseRegister($2);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum1, ""});
                         
                         uint8_t registerNum2 = parseRegister($4);
                         args.push_back(Argument{ArgumentType::Register, AddressingType::RegisterDirect, registerNum2, ""});

                         args.push_back(*$6);
                         Assembler::getCurrentSection()->translateInstruction(instr, args);
                         
                         free($1);
                         free($2);
                         free($4);
                         delete $6;
                         args.clear();
                 }
                 ;

%%
int yyerror(const char* s)
{
    throw std::runtime_error(s);
    return 1;
}
