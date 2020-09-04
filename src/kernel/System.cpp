/**
 * System.cpp
 *
 * @date 05.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "System.hpp"
#include <lib/collection/Heap.hpp>

extern "C" void isr0();
extern "C" void isr1();

extern uint32 kernel_size;
extern uint32 kernel_stack;

kernel::System *system = 0;

void isr_print_cpu_error( kernel::Thread::State* state ) {
  system->video.color( kernel::Video::Red );
  uint32 cr2;
  switch ( state->irq ) {
    case 0:
      system->video << "divide error exception";
    break;
    case 1:
      system->video << "debug exception";
    break;
    case 3:
      system->video << "break point exception";
    break;
    case 4:
      system->video << "overflow exception";
    break;
    case 5:
      system->video << "bound exception";
    break;
    case 6:
      system->video << "invalid opcode";
    break;
    case 7:
      system->video << "device not aviable";
    break;
    case 8:
      system->video << "double fault exception";
    break;
    case 9:
      system->video << "coprocessor segment overrun";
    break;
    case 10:
      system->video << "invalid tss";
    break;
    case 11:
      system->video << "segment not present";
    break;
    case 12:
      system->video << "stack fault";
    break;
    case 13:
      system->video << "general protection exception";
    break;
    case 14:
      system->video << "page fault";
      asm volatile("mov %%cr2, %0": "=b"(cr2));
      system->video << " wanted addr " << cr2;
    break;
    case 16:
      system->video << "floating point exception";
    break;
    case 17:
      system->video << "align check exception";
    break;
    case 18:
      system->video << "machine check exception";
    break;
    case 19:
      system->video << "smid floating point exception";
    break;
    default:
      system->video << state->irq;
    break;
  }

  //system->video << " by thread " << ( **system->next )->id();
}

kernel::Thread::State* isrcallback( kernel::Thread::State* state ) {

  if ( state->irq < 0x20 ) {
    if ( state->irq == 14 ) { // page exception
      uint32 virtual_addr, page;

      asm volatile("mov %%cr2, %0": "=b"(virtual_addr));

      page = ( uint32 ) kernel::System::physical_memory.alloc();
      system->virtual_memory.map( page, virtual_addr );

    }
    else {
      isr_print_cpu_error( state );
      asm volatile("cli;hlt;");
    }
  }
  else if ( state->irq < 0x40 ) { // hardware and software interrupts
    //kernel::System::disablePaging();
    uint32 irq = state->irq;
    uint32 cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    cr0 &= ~0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));

    kernel::Thread* t = **system->next;

    switch ( irq ) {
      case 0x20: // timer interrupt for task switch
        t->state = state; // save the state

        t = system->dispatch();

        state = t->state;
      break;
      case 0x30: // exception
        t->mode = kernel::Thread::DEAD;

        lib::Exception* e = ( lib::Exception* ) state->ESP;

        system->video.color( kernel::Video::LightRed );
        system->video << "thread-" << t->id() << " throws: " << e->msg;
        system->video.color( kernel::Video::LightGrey );

        asm volatile("cli;hlt;");

        delete e; // delete exception
        delete t; // kill the thread which makes trouble.

        system->schedule();
        state = system->dispatch()->state; // execute another thread

        system->video << "new thread " << ( **system->next )->id() << "\n";
      break;
    }

    if ( t->_process->virtual_memory.page_directoies ) {
      kernel::System::switchToPageDirectory( t->_process->virtual_memory.page_directoies );
      kernel::System::enablePaging();
    }
  }
  else if ( state->irq <= 99 ) {
    //system->interrupts.push( ( uint8 ) state->irq ); // tell the core which interrupt was invoked.
  }

// acknowledge that the interrupt was handled,
// so we can receive another one!
  kernel::System::eoi( state->irq );

  return state; // return thread state
}

namespace kernel {

  System::Address System::gdt_address;
  System::Address System::idt_address;
  uint64 System::gdt_table[ EntryCount ];
  uint64 System::idt_table[ IRQCount ];
  TSS System::tss;

  void System::set_gdt( uint32 idx, uint32 base, uint32 limit, uint32 flags ) {
    gdt_table[ idx ] = limit & 0xffff;
    gdt_table[ idx ] |= ( base & 0xffffff ) << 16;
    gdt_table[ idx ] |= ( flags & ( uint64 ) 0xff ) << 40;
    gdt_table[ idx ] |= ( ( limit >> 16 ) & ( uint64 ) 0xf ) << 48;
    gdt_table[ idx ] |= ( ( flags >> 8 ) & ( uint64 ) 0xff ) << 52;
    gdt_table[ idx ] |= ( ( base >> 24 ) & ( uint64 ) 0xff ) << 56;
  }

  void System::setup_idt() {
    uint32 step = ( uint32 ) &isr1 - ( uint32 ) &isr0; // get size of asm isr wrappers
    uint32 addr = ( uint32 ) &isr0; // set address of first wrapper
    idt_address.size = IRQCount * 8 - 1;
    idt_address.offset = ( uint32 ) &idt_table;

    // tell where the idt is
    asm volatile ("lidt %0;" : : "m" (idt_address));

    // set all interrupt wrappers in the idt
    for ( uint32 i = 0; i < 256; i++ ) {
      set_isr( i, addr );
      addr += step;
    }
  }

  void System::set_isr( uint8 IRQ, uint32 F ) {
    idt_table[ IRQ ] = F & 0xffff;
    idt_table[ IRQ ] |= ( uint64 ) ( F >> 16 ) << 48;
    idt_table[ IRQ ] |= ( uint64 ) 0x08 << 16; // data segment of the GDT
    idt_table[ IRQ ] |= ( uint64 ) 0x8E << 40; // 32 bit interrupt gate
  }

  void System::setup_gdt() {
    gdt_address.offset = ( uint32 ) &gdt_table;
    gdt_address.size = EntryCount * 8 - 1;

    tss.IOPB = sizeof(TSS) << 16;
    //tss.IOPB = 0;

    // Selector 0x00 cannot be used
    set_gdt( 0, 0, 0, 0 );

    // Selector 0x08 will be our code.
    // We will use all memory as our code segment!
    set_gdt( 1, 0, 0xFFFFFFFF, GDT_CODESEG | GDT_GRAN_4K | GDT_PRESENT | GDT_SEGMENT | GDT_BIT32 );

    // Selector 0x10 will be our data.
    // We will use all memory as our data segment!
    set_gdt( 2, 0, 0xFFFFFFFF, GDT_DATASEG | GDT_GRAN_4K | GDT_PRESENT | GDT_SEGMENT | GDT_BIT32 );

    // Selector 0x18 will be our tss.
    // Set our kernel TSS.
    set_gdt( 3, ( uint32 ) &tss, sizeof(TSS), GDT_TSS | GDT_PRESENT | GDT_BIT32 );

    asm volatile(
        //       "cli;"
        "lgdt %0;"
        "mov $0x18, %%eax;"
        "ltr %%ax;"
        //       "sti;"
        : : "m" (gdt_address));

    asm volatile (
        "mov $0x10, %ax;"
        "mov   %ax, %ds;"
        "mov   %ax, %es;"
        "mov   %ax, %fs;"
        "mov   %ax, %gs;"
        "mov   %ax, %ss;"
        "ljmp $0x8, $.1;"
        ".1:;"
    );

  }

  void System::eoi( int IRQ ) {
    if ( IRQ > 7 )
      lib::outb( PIC_SlaveCmd, PIC_EOI ); // tell slave PIC that the interrupt is acknowledged.

    lib::outb( PIC_MasterCmd, PIC_EOI ); // tell master PIC that the interrupt is acknowledged.
  }

  void System::setup_pic() {
    lib::outb( PIC_MasterCmd, PIC_ICW1_INIT + PIC_ICW1_ICW4 ); // starts the initialization sequence
    lib::outb( PIC_SlaveCmd, PIC_ICW1_INIT + PIC_ICW1_ICW4 );
    lib::outb( PIC_MasterData, PIC_Master_Offset ); // define the PIC vectors
    lib::outb( PIC_SlaveData, PIC_Slave_Offset );
    lib::outb( PIC_MasterData, 4 ); // continue initialization sequence
    lib::outb( PIC_SlaveData, 2 );

    lib::outb( PIC_MasterData, PIC_ICW4_8086 );
    lib::outb( PIC_SlaveData, PIC_ICW4_8086 );

    // restore masks.
    lib::outb( PIC_MasterData, 0xfc );
    lib::outb( PIC_SlaveData, 0xff );
  }

  bool HeapThreadCompare( Thread*& a, Thread*& b ) {
    return a->behavior.priority <= b->behavior.priority;
  }

  PhysicalMemory System::physical_memory;

  void* System::operator new( uint32 size, MultiBoot* multiboot ) {
    setup_gdt();
    setup_idt();
    setup_pic();

    physical_memory.analyse( multiboot );

    System* s = ( System* ) ( 1 );

    return ( void* ) s;
  }

  void System::initVirtualMemory() {
    // manually setting up the virtual memory basics to solve a chicken'n'egg problem
    virtual_memory.page_directoies = ( uint32* ) 0;
    virtual_memory.memsize = 0;
    virtual_memory.memused = 0;
    virtual_memory.node_addr_pointer = 0;
    virtual_memory.node_addr_stack = ( uint32* ) physical_memory.alloc();
    virtual_memory.node_last_block = ( uint8* ) physical_memory.alloc();

    // pushing all free tree nodes onto the stack
    for ( uint32 i = 16; i < 4096; i += sizeof(VirtualMemory::Area) ) {
      virtual_memory.node_put( ( VirtualMemory::Area* ) ( virtual_memory.node_last_block + i ) );
    }

    // initial memory layout:
    //    system | io-map area | kernel

  }

  System::System()
      : timer( PIT::Channel0 ), ProcessIDPool( 0 ), ThreadIDPool( 0 ), next( 0 ) {
    system = this;

    initVirtualMemory();

    user = new User( "system" );

    // we deactivate the timer
    timer.init( PIT::Mod_TerminalCount );
    timer.load( 0xffff );

    _id = ProcessIDPool++;

    Thread* k = ( Thread* ) virtual_memory.alloc( sizeof(Thread) ); // this thread represents our kernel thread

    k->_id = ThreadIDPool++;

    k->stack = ( uint8* ) &kernel_stack;

    k->stack_size = ( uint32 ) &kernel_size - ( uint32 ) &kernel_stack;

    k->mode = Thread::RUNNING; // our kernel is already running :)

    k->_process = this;

    threads.pushBack( k );

    schedule();

    state = Process::Active;

    plan = new SchedulingPlan();

    plan->push( k );

    next = plan->iterator();

    video.clear();
  }

  void System::schedule() {

  }

  Thread* System::dispatch() {

    // do we need to reschedule
    if ( threads.size() != plan->size() ) {
      schedule();
    }

    Thread* c = **next;

    // if our current thread is not blocked,
    // set it ready!
    if ( c->mode != Thread::BLOCKED && c->mode != Thread::DEAD )
      c->mode = Thread::READY;

    // look for the next thread which is ready or started.
    do {
      if ( ( **next )->mode == Thread::DEAD ) // delete dead threads
        delete ( **next );

      ++( *next );
    } while ( ( **next )->mode != Thread::READY && ( **next )->mode != Thread::START );

    // set the current state to running
    ( **next )->mode = Thread::RUNNING;

    return ( **next );

  }

  void System::switchToPageDirectory( uint32* PD ) {
    asm volatile("mov %0, %%cr3":: "r"( PD ) );
  }

  bool System::isPaged() {
    unsigned int cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));

    if ( cr0 & 0x80000000 )
      return true;

    return false;
  }

  void System::enablePaging() {
    unsigned int cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));
  }

  void System::disablePaging() {
    unsigned int cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    cr0 &= ~0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));
  }

  void System::copy( Process* fromP, uint32 fromAddr, Process* toP, uint32 toAddr, uint32 size ) {
    uint32 phyFrom = fromP->virtual_memory.getPhysicalAddress( fromAddr );
    uint32 phyTo = toP->virtual_memory.getPhysicalAddress( toAddr );

    lib::memcpy( ( void* ) phyFrom, ( void* ) phyTo, size );
  }

  System::~System() {
    disablePaging();
  }

  uint64 System::size() {
    return ( uint32 ) &kernel_size;
  }

  void System::read( void** data, uint32* length ) {

  }

  uint32 System::read( void* data, uint32 length ) {
    return 0;
  }

  uint32 System::write( void* data, uint32 length ) {
    return video.write( data, length );
  }

  void* System::range( uint32 start, uint32 size ) {
    return 0;
  }

  void System::range( uint32 start, uint32 size, void* data ) {

  }

}
