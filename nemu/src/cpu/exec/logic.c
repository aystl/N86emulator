#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
 rtl_and(&id_dest->val,&id_dest->val,&id_src->val);
 rtl_li(&t0,0);
 rtl_set_CF(&t0);
 rtl_set_OF(&t0);
 rtl_update_ZFSF(&id_dest->val,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);
  rtl_li(&t0,0);
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  //printf("%x\t%x",id_src->val,id_src2->val);
  rtl_xor(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);

  rtl_li(&t0,0);
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);
  rtl_li(&t3,0);
  rtl_set_OF(&t3);
  rtl_set_CF(&t3);//根据i386手册将OF、CF置零，并根据结果更新SF、ZF
  rtl_update_ZFSF(&id_dest->val,id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
 
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  rtl_not(&id_dest->val);
  operand_write(id_dest,&id_dest->val);

  print_asm_template1(not);
}

make_EHelper(rol) {
  //TODO();
  unsigned i;
  for(i=0;i<id_src->val;i++)
  {
	rtl_msb(&t0,&id_dest->val,id_dest->width);
	rtl_shli(&t1,&id_dest->val,1);
	rtl_or(&id_dest->val,&t0,&t1);
  }
  operand_write(id_dest,&id_dest->val);
  rtl_set_CF(&t0);

  print_asm_template2(rol);
}
