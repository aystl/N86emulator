#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) { 
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_info(char *args){
    if (strcmp("r",args)==0)
    {
		 for (int i = R_EAX; i <= R_EDI; i ++)
		 { 
			 printf("%s:\t0x%08x\t%d\n", regsl[i], cpu.gpr[i]._32,cpu.gpr[i]._32);
		 }
	printf("eip:\t0x%08x\t%d\n", cpu.eip,cpu.eip);	 
    } 
    else if (strcmp("w",args)==0)
    {  
        list_watchpoint();
    }
    return 0;
}

static int cmd_si(char *args){
	int ret=0;
	if(args==NULL)
		ret=1;
	else
		sscanf(args,"%d",&ret);
    cpu_exec(ret);
    return 0;
}

static int cmd_x(char *args){
    char *str = strtok(args," ");
	int count=0;
	uint32_t hex=0;
	sscanf(str,"%d",&count);
	str = strtok(NULL," ");
	//sscanf(str,"%x",&hex);
	bool flag=1;
	bool *success=&flag;
	hex=expr(str,success);
	assert(flag);	//检查make_token是否成功
    for(int j=0;j<count;++j,hex+=4){
       uint32_t data =  vaddr_read(hex,4);
	   unsigned int byte[4];
	   for(int k=0;k<4;++k)
	   {
			byte[k]=255 & (data >> (8*k));
	   }
    //每次循环将读取到的数据用 printf 打印出来
        printf("0x%08x\t0x%08x...",hex,data);
	    printf("%02x %02x %02x %02x\n",byte[0],byte[1],byte[2],byte[3]);	
    }
	return 0;
}

static int cmd_p(char *args){
	bool flag = 1;
	bool *success = &flag;
	uint32_t ret = expr(args,success);
	assert(flag);   //检查make_token是否成功
	printf("%d\n",ret);
	return 0;
}

static int cmd_w(char *args){
	set_watchpoint(args,1);
	return 0;
}

static int cmd_b(char *args){
	set_watchpoint(args,2);
	return 0;
}

static int cmd_d(char *args){
	int NO;
	sscanf(args,"%d",&NO);
	delete_watchpoint(NO);
	return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si", "Single step execution", cmd_si},
  {"info","Print program status",cmd_info},
  {"x","Scan memory",cmd_x},
  {"p","Expression evaluation",cmd_p},
  {"w","Set watchpoint",cmd_w},
  {"d","Delete wantchpoint",cmd_d},
  {"b","Set software breakpoints",cmd_b},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
