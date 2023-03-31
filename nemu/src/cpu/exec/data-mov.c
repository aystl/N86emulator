#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_push(&id_dest->val);
  print_asm_template1(push);
  //Log("make over");
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&id_dest->val);
  operand_write(id_dest,&id_dest->val);
  
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  if( decoding.is_operand_size_16 ){
	t0 = cpu.esp;
	rtl_lr_w(&t1, R_AX);//AX
	rtl_push(&t1);
	rtl_lr_w(&t1, R_CX);//CX
	rtl_push(&t1);
	rtl_lr_w(&t1, R_DX);//DX
	rtl_push(&t1);
	rtl_lr_w(&t1, R_BX);//BX
	rtl_push(&t1);
	rtl_push(&t0); //Temp
	rtl_lr_w(&t1, R_BP);//BP
	rtl_push(&t1);
	rtl_lr_w(&t1, R_SI);//SI
	rtl_push(&t1);
	rtl_lr_w(&t1, R_DI);//DI
	rtl_push(&t1);
	}
	else{
	t0 = cpu.esp;
	rtl_lr_l(&t1, R_EAX);//EAX
	rtl_push(&t1);
	rtl_lr_l(&t1, R_ECX);//ECX
	rtl_push(&t1);
	rtl_lr_l(&t1, R_EDX);//EDX
	rtl_push(&t1);
	rtl_lr_l(&t1, R_EBX);//EBX
	rtl_push(&t1);
	rtl_push(&t0); //Temp
	rtl_lr_l(&t1, R_EBP);//EBP
	rtl_push(&t1);
	rtl_lr_l(&t1, R_ESI);//ESI
	rtl_push(&t1);
	rtl_lr_l(&t1, R_EDI);//EDI
	rtl_push(&t1);
	}
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
	if(((int16_t)cpu.eax)<0)
		cpu.edx=0xffff;
	else cpu.edx=0;
  }
  else {
    //TODO();
	if(((int32_t)cpu.eax)<0)
		cpu.edx=0xffffffff;
	else cpu.edx=0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
	rtl_lr_b(&t0,R_AL);
    rtl_sext(&t0,&t0,1);
    rtl_sr_w(R_AX,&t0);
  }
  else {
    //TODO();
	rtl_lr_w(&t0,R_AX);
    rtl_sext(&t0,&t0,2);
    rtl_sr_l(R_EAX,&t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
