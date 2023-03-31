#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/reg.h"
#include "memory/memory.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
   }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(){
	if(head==NULL)	//若head为空
	{
		head=free_;
		free_=free_->next;
		head->next=NULL;
		return head;
	}
	else if(head!=NULL&&free_!=NULL)	//若head和free_都不为空
	{
		WP *p=head;
		while(p->next!=NULL)	//找到head的尾节点
			p=p->next;
		p->next=free_;
		free_=free_->next;
		p->next->next=NULL;
		return p->next;
	}
	else	//free_为空则报错
		assert(0);	
}

void free_wp(WP *wp){
	if(head==NULL){
		assert(0);
	}
	else if(head==wp)
	{
		head=wp->next;
		wp->next=free_;
		free_=wp;
	}
	else
	{
		WP *p=head,*q=NULL;
		while(p->next!=NULL&&p->next!=wp)
			p=p->next;
		q=p->next;
		if(q==NULL)
			assert(0);
		p->next=q->next;
		q->next=head;
		head=q;
	}
}


int set_watchpoint(char *e,int type){
	WP *p=new_wp();
     bool flag=1;
     bool *success=&flag;
     uint32_t ret=expr(e,success);
     assert(flag);
     p->old_val=ret;
     strcpy(p->expr,e);
	 p->type=type;
     printf("Set watchpoint #%d\n",p->NO);
     printf("expr\t= %s\n",e);
     printf("old value = 0x%08x\n",ret);
	 if(type==2)
	 {
		p->address=ret;
		p->replaced=vaddr_read(ret,1);
		uint32_t code;
		sscanf("0xcc","%x",&code);
		vaddr_write(ret,1,code);
	 }
     return p->NO;
}

void list_watchpoint(int NO){
	WP *p=head;
    if(p==NULL)
         printf("No used watchpoint!\n");
    else
    {
       printf("NO\texpr\t\t\tOld Value\n");
       while(p!=NULL)
       {
	        printf("%d\t%s\t\t\t0x%08x\n",p->NO,p->expr,p->old_val);
            p=p->next;
	   }
    }
}

bool delete_watchpoint(int NO){
	if(NO<0||NO>31)
		return false;
	else{
		WP* p=&wp_pool[NO];
		free_wp(p);
		return true;
	}	
}

WP* scan_watchpoint(){
	WP *p=head,*q=NULL;
	bool pause=false;
	while(p!=NULL)
	{
		bool flag=1;
		bool *success=&flag;
		p->new_val=expr(p->expr,success);
		if(p->new_val!=p->old_val)
		{
			if(!pause)
				q=p;
			pause=true;
			printf("Hit watchpoint %d at address 0x%08x\n",p->NO,cpu.eip);
			printf("expr\t= %s\n",p->expr);
			printf("old value = %08x\n",p->old_val);
			printf("new value = %08x\n",p->new_val);
			printf("program paused\n");
			p->old_val=p->new_val;
 		}
		p=p->next;
 	}
	return q;
}

WP* get_head(){
	return head;
}
