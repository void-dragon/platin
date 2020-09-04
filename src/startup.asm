
# global entry point
.global loader
.global kernel_stack

.extern main                            # from main.cpp
.extern staticConstructors              # from cpp.cpp
.extern staticDestructors               # from cpp.cpp

.section multiboot

.set ALIGN,    1<<0                  # align loaded modules on page boundaries
.set MEMINFO,  1<<1                  # provide memory map
.set VBE,      1<<11                 # vbe information
.set FLAGS,    ALIGN | MEMINFO       # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002            # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)      # checksum required

.align 4
.int MAGIC
.int FLAGS
.int CHECKSUM

.section .text
# reserve initial kernel stack space
.set KERNEL_STACKSIZE, 0x200000              # that is, 2MB.
.comm kernel_stack, KERNEL_STACKSIZE, 32     # reserve 2MB stack on a quadword boundary
 
loader:
  cli
  movl $(kernel_stack+KERNEL_STACKSIZE), %esp  # set up the stack

  call staticConstructors                      # call constructors

  pushl %ebx                                   # pass multiboot struct as parameter
  call main                                    # call main

  call staticDestructors                       # call destructors
 
stop:
  hlt
  jmp stop                                     # Endlosschleife nach Beendigung unseres Kernels

