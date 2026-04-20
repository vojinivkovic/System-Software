%{
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>
void yyerror(char* s);
int yylex();  
%}
%%

".global" {
          yylval.field = strdup(yytext + 1); 
          return DIRECTIVE;
          }

".extern" {
          yylval.field = strdup(yytext + 1); 
          return DIRECTIVE;
          }
          
".section" {
          yylval.field = strdup(yytext + 1); 
          return DIRECTIVE;
          }

".word" {
          yylval.field = strdup(yytext + 1); 
          return DIRECTIVE;
          }

".skip" {
          yylval.field = strdup(yytext + 1); 
          return DIRECTIVE;
          }

".ascii" {
          yylval.field = strdup(yytext + 1); 
          return DIRECTIVE;
          }

".equ"  {
          yylval.field = strdup(yytext + 1); 
          return EQU;
        }

".end" {
          yylval.field = strdup(yytext + 1); 
          return END;
        }


"halt" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"int" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	
"iret" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"call" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	
"ret" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"jmp" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	
      
"beq" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"bne" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"bgt" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"push" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"pop" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"xchg" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	
      
"add" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"sub" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"mul" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"div" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"not" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"and" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"or" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"xor" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"shl" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"shr" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"ld" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"st" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"csrrd" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	

"csrwr" {
        yylval.field = strdup(yytext); 
        return COMMAND;
      }	


"%r"(1[0-5]|[0-9])\b {
          yylval.field = strdup(yytext + 1); 
          return GP_REGISTER;
          }
"%sp" {
          yylval.field = strdup(yytext + 1); 
          return GP_REGISTER;
          }
"%pc" {
          yylval.field = strdup(yytext + 1); 
          return GP_REGISTER;
          }

"%status" {
  yylval.field = strdup(yytext + 1); 
  return CS_REGISTER;
  }

"%handler" {
  yylval.field = strdup(yytext + 1); 
  return CS_REGISTER;
  }

"%cause" {
  yylval.field = strdup(yytext + 1); 
  return CS_REGISTER;
  }


[._a-zA-Z][._a-zA-Z0-9]*: { 
  yytext[yyleng - 1] = '\0';
  yylval.field = strdup(yytext); 
  return LABEL;
}

[._a-zA-Z][._a-zA-Z0-9]* {
  yylval.field = strdup(yytext); 
  return SYMBOL;
}

[+-]?(0[xX][0-9a-fA-F]+|0[bB][01]+|[0-9]+) {
    yylval.field = strdup(yytext);
    return LITERAL;
}


"," {return ',' ;}
"-" {return '-' ;}
"+" {return '+' ;}


\"([^\"\\]|\\.)*\" {
    yylval.field = strdup(yytext);
    return STRING;
}

"$" {
  yylval.field = strdup(yytext);
  return '$';
}

"\[" {
  yylval.field = strdup(yytext);
  return '[';
} 

"\]" {
  yylval.field = strdup(yytext);
  return ']';
}

"#".* ;

"\n" { return '\n'; }

%%

int yywrap()
{
  return 1;
}

void yyerror(char* s)
{

}