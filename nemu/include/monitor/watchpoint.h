#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  
  uint32_t old_val;
  uint32_t new_val;
  char expr[64];

  int type;
  uint32_t address;
  uint8_t replaced;

} WP;

int set_watchpoint(char *e,int type);    //给予一个表达式e，构造以该表达式为监视目标的监视点，并返回编号
bool delete_watchpoint(int NO); //给予一个监视点编号，从已使用的监视点中归还该监视点到池中
void list_watchpoint();     //显示当前在使用状态中的监视点列表
WP* scan_watchpoint();      //扫描所有使用中的监视点，返回触发的监视点指针，若无触发返回NULL



void free_wp(WP* wp);

WP* get_head();

#endif
