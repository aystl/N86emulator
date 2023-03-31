#include "cpu/exec.h"
#include "monitor/watchpoint.h"
#include "monitor/monitor.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  //TODO();
  if(decoding.is_operand_size_16)
  {
	cpu.idtr.limit=vaddr_read(id_dest->addr,2);
	cpu.idtr.base=vaddr_read(id_dest->addr+2,3);
  }
  else
  {
	cpu.idtr.limit=vaddr_read(id_dest->addr,2);
	cpu.idtr.base=vaddr_read(id_dest->addr+2,4);
  }

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
  raise_intr(id_dest->val,decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&decoding.jmp_eip);
  decoding.is_jmp=true;
  uint32_t cs;
  rtl_pop(&cs);
  rtl_pop(&cpu.eflags.init);

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
  id_dest->val=pio_read(id_src->val,id_dest->width);
  operand_write(id_dest,&id_dest->val);


  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  //TODO();
  pio_write(id_dest->val,id_dest->width,id_src->val); 

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int3){
	bool paused=false;
	WP *p=get_head();
	while(p!=NULL)
	{
		if(p->type==2)
		{
			if(!paused)
			{
				printf("Hit software breakpoint at 0x%08x\n",*eip-1);
				printf("program paused");
				(*eip)--;
				paused=true;
			}
			vaddr_write(p->address,1,p->replaced);
		}
		p=p->next;
	}
	nemu_state=NEMU_STOP;
}
