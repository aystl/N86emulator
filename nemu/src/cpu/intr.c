#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */ 

  //TODO();
  //1.依次将EFLAGS, CS(代码段寄存器), EIP寄存器的值压栈(ret_addr)
  rtl_push(&cpu.eflags.init);
  /*rtl_push(&cpu.cs);这里CS寄存器是uint16_t类型，不符合rtl_push的参数,不知道如何处理
   *讲义里提到PA不使用CS寄存器，于是我直接省略这一小步骤,但为了通过difftest，push一个数*/
  uint32_t cs=0x8;
  rtl_push(&cs);
  rtl_push(&ret_addr);

  //2.从IDTR中读出IDT的首地址。3.根据异常号在IDT中进行索引, 找到一个门描述符。
  //由于一个Gate Descriptor大小为八字节，所以进行如下操作
  uint32_t IDT_addr=cpu.idtr.base+NO*8;

  //4.将门描述符中的offset域组合成目标地址
  uint32_t h16_addr,l16_addr;//高、低16位地址
  //l16_addr=vaddr_read(IDT_addr,2);
  //不知道注释掉的写法对不对，写完我感觉访问一个不是4的倍数的内存很奇怪，
  //似乎不符合数据对齐的理论
  /*h16_addr=vaddr_read(IDT_addr+6,2);
  h16_addr=h16_addr<<16;*/

  l16_addr=vaddr_read(IDT_addr,4)&0x0000ffff;
  h16_addr=vaddr_read(IDT_addr+4,4)&0xffff0000;
  uint32_t real_addr=h16_addr|l16_addr;

  //5.跳转到目标地址
  decoding.jmp_eip=real_addr;
  decoding.is_jmp=true;
}

void dev_raise_intr() {
}
