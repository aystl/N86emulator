#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_DEC,TK_HEX,TK_REG,DEREF,MINUS,TK_UEQ,TK_LN,TK_LO,TK_LA

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"!=",TK_UEQ},		// unequal sign
  {"!",TK_LN},			// logical non
  {"\\|\\|",TK_LO},		// logical or
  {"\\&\\&",TK_LA},		// logical and
  {"\\-", '-'},			// minus
  {"\\*", '*'},			// multiply
  {"\\/", '/'},			// divide
  {"\\(", '('},			// left bracket
  {"\\)", ')'},			// right bracket
  {"0x[0-9a-f]+", TK_HEX}, // hexadecimal number
  {"[0-9]+", TK_DEC},	// decimal number
  {"\\$eax|\\$edx|\\$ecx|\\$ebx|\\$ebp|\\$esi|\\$edi|\\$esp|\\$eip", TK_REG} // register
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
		
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
			case '+':
				{
					tokens[nr_token].type='+';
					nr_token++;
					break;
				}
			case TK_DEC:
				{ 
					tokens[nr_token].type=TK_DEC;
					strncpy(tokens[nr_token].str,substr_start,substr_len);
					tokens[nr_token].str[substr_len]='\0';
					nr_token++;
					break;
				}
			case '-':
                 {
                     tokens[nr_token].type='-';
                     nr_token++;
                     break;
                }
			case '*':
                 {
                     tokens[nr_token].type='*';
                     nr_token++;
                     break;
                }
			case '/':
                 {
                     tokens[nr_token].type='/';
                     nr_token++;
                     break;
                }
			case TK_HEX:
                 { 
                     tokens[nr_token].type=TK_HEX;
                     strncpy(tokens[nr_token].str,substr_start,substr_len);
                     tokens[nr_token].str[substr_len]='\0';
				   	 nr_token++;
                     break;
                 }
			case '(':
                 {
                     tokens[nr_token].type='(';
                     nr_token++;
                     break;
                 }
			case ')':
                 {
                     tokens[nr_token].type=')';
                     nr_token++;
                     break;
                 }
			case TK_REG:
				 {
					 tokens[nr_token].type=TK_REG;
					 strncpy(tokens[nr_token].str,substr_start,substr_len);
					 tokens[nr_token].str[substr_len]='\0';
					 nr_token++;
				 }
			case TK_NOTYPE:
				 break;
			case TK_UEQ:
                  {
                      tokens[nr_token].type=TK_UEQ;
                      nr_token++;
                      break;
                  }
			case TK_LA:
                  {
                      tokens[nr_token].type=TK_LA;
                      nr_token++;
                      break;
                  }
			case TK_LN:
                  {
                      tokens[nr_token].type=TK_LN;
                      nr_token++;
                      break;
                  }
			case TK_LO:
                  {
                      tokens[nr_token].type=TK_LO;
                      nr_token++;
                      break;
                  }
			case TK_EQ:
				  {
					  tokens[nr_token].type=TK_EQ;
					  nr_token++;
					  break;
				  }
          default: TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t eval(int p,int q);
bool check_parentheses(int p,int q);
int find_dominated_op(int p,int q);

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i==0||(tokens[i - 1].type!=TK_DEC&&tokens[i-1].type!=TK_HEX&&tokens[i-1].type!=')'&&tokens[i-1].type!=TK_REG)) ) {
        tokens[i].type = DEREF;
    }
	if (tokens[i].type == '-' && (i==0||(tokens[i - 1].type!=TK_DEC&&tokens[i-1].type!=TK_HEX&&tokens[i-1].type!=')'&&tokens[i-1].type!=TK_REG)) ) {
         tokens[i].type = MINUS;
     }
  }
  int ans=eval(0,nr_token-1);

  return ans;
}

uint32_t eval(int p, int q) {		//表达式求值函数
    if (p > q) {
        panic("bad expression1");
    }
    else if (p == q) {
        /* Single token.
        * For now this token should be a number.
        * Return the value of the number.
        */
		if(tokens[p].type==TK_DEC)
		{
			uint32_t ret;
			sscanf(tokens[p].str,"%d",&ret);
			return ret;
		}
		else if(tokens[p].type==TK_HEX)
		{
			uint32_t ret;
			sscanf(tokens[p].str,"%x",&ret);
			return ret;
		}
		else if(tokens[p].type==TK_REG)
		{
			if(strcmp(tokens[p].str,"$eax")==0)
		     	 return cpu.eax;
			else if(strcmp(tokens[p].str,"$ebx")==0)
                 return cpu.ebx;
			else if(strcmp(tokens[p].str,"$ecx")==0)
                 return cpu.ecx;
			else if(strcmp(tokens[p].str,"$edx")==0)
                 return cpu.edx;
			else if(strcmp(tokens[p].str,"$esp")==0)
                 return cpu.esp;
			else if(strcmp(tokens[p].str,"$ebp")==0)
                 return cpu.ebp;
			else if(strcmp(tokens[p].str,"$esi")==0)
                 return cpu.esi;
			else if(strcmp(tokens[p].str,"$edi")==0)
                 return cpu.edi;
			else if(strcmp(tokens[p].str,"$eip")==0)
                 return cpu.eip;
			else 
				 panic("can not return a value of register!");
		}
		else
		{
			panic("can not return a value");
		}
    }
    else if (check_parentheses(p, q) == true) {
        /* The expression is surrounded by a matched pair of parentheses.
        * If that is the case, just throw away the parentheses.
        */
        return eval(p + 1, q - 1);
    }
    else {
        /* We should do more things here. */
		int op=find_dominated_op(p,q);
		switch(tokens[op].type)
		{
			case '+':
				return eval(p, op - 1) + eval(op + 1,q);
				break;
			case '-':
                return eval(p, op - 1) - eval(op + 1,q);
                break;
			case '*':
                return eval(p, op - 1) * eval(op + 1,q);
                break;
			case '/':
                return eval(p, op - 1) / eval(op + 1,q);
                break;
			case DEREF:		//解引用
				return vaddr_read(eval(op+1,q),4);
				break;
			case MINUS:		//负号
				return (-1)*eval(op+1,q);
				break;
			case TK_EQ:		//等号
				{
					if(eval(p,op-1)==eval(op+1,q))
						return 1;
					else return 0;
					break;
				}
			case TK_UEQ:	//不等号
                 {
                     if(eval(p,op-1)==eval(op+1,q))
                         return 0;
                     else return 1;
                     break;
                 }
			case TK_LN:		//逻辑非
				 {
					 if(eval(op+1,q)==0)
						 return 1;
					 else return 0;
					 break;
				 }
			case TK_LA:		//逻辑与
				 { 
					 return(eval(p,op-1)&&eval(op+1,q));
					 break;
				 }
			case TK_LO:		//逻辑或
				 {
					 return(eval(p,op-1)||eval(op+1,q));
					 break;
				 }
			default:
				panic("error!");
		}
    }
}

bool check_parentheses(int p,int q){
	if(tokens[p].type!='(' || tokens[q].type!=')')
		return false;
	else
	{
		bool ans=true;
		int count=0;
		for(int i=p;i<=q;i++)
		{
			if(tokens[i].type=='(')
				count++;
			if(tokens[i].type==')')
				count--;
			if(count<1&&i!=q)
				ans=false;
		}
		if(count!=0) //如果左右括号数不匹配
			panic("bad expression");
		else
			return ans;
	}
}

int find_dominated_op(int p,int q){
	 int main_op=p;
	 int priority;
     int min=100;
	 int count=0;
     for(int j=p;j<=q;j++)
     {
         priority=100;
         switch(tokens[j].type)		//赋予不同符号不同优先级，计算括号数量，在括号内的非主运算符
         {
			  case TK_LO:
				  priority=(-2);
				  break;
			  case TK_LA:
				  priority=(-1);
				  break;
			  case TK_EQ:
			  case TK_UEQ:
				  priority=0;
				  break;
              case '+':
              case '-':
                  priority=1;
                  break;
              case '*':
              case '/':
                  priority=2;
                  break;
			  case MINUS:
			  case DEREF:
			  case TK_LN:
				  priority=3;
				  break;
			  case '(':
				  count++;
				  break;
			  case ')':
				  count--;
				  break;
          }
          if(count==0&&priority<min)
          {
              min=priority;
              main_op=j;
          }
		  else if(count==0&&priority==min&&priority<3)	//考虑到解引用和负号为右结合，优先级相等的时候应分情况讨论
		  {
			  min=priority;
			  main_op=j;
		  }
     }
	 return main_op;
}
