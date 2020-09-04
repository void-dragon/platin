/**
 * Thread.cpp
 *
 * @since 26.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Thread.hpp"
#include <lib/std.hpp>
#include <kernel/System.hpp>

namespace kernel {

  void Thread::init() {

    mode = START;

    behavior.priority = 0;
    behavior.duration = 1;
    behavior.step = 0;

    _id = system->ThreadIDPool++;

    if ( stack_size ) {

      uint8* virtualstack = ( uint8* ) _process->virtual_memory.alloc( stack_size );

      if ( _process->virtual_memory.page_directoies ) {
        stack = ( uint8* ) _process->virtual_memory.getPhysicalAddress( ( uint32 ) virtualstack );

        // blend the thread stack into the virtual memory of the process.
        _process->virtual_memory.map( ( uint32 ) stack, ( uint32 ) virtualstack );

        // set the task state segment at the beginning of the stack.
        state = ( State* ) ( stack + stack_size - sizeof(State) );
      }
      else
      {
        state = ( State* ) ( virtualstack + stack_size - sizeof(State) );
      }

      state->EAX = 0;
      state->EBX = 0;
      state->ECX = 0;
      state->EDX = 0;
      state->ESI = 0;
      state->EDI = 0;
      state->EBP = ( uint32 ) virtualstack;
      state->ESP = ( uint32 ) ( virtualstack + stack_size );
      state->EIP = func;

      // set our code and data segment
      state->CS = 0x08;
      state->SS = 0x10;
      state->DS = 0x10;
      state->ES = 0x10;
      state->FS = 0x10;
      state->GS = 0x10;

      asm volatile("movl %%cr0, %0": "=b"(state->cr0));
      if ( _process->virtual_memory.page_directoies )
        state->cr0 |= 0x80000000;
      state->cr3 = ( uint32 ) _process->virtual_memory.page_directoies;

      // enable irq's
      state->EFLAGS = 1 << 9;

      state->para = ( uint32 ) this; // take the pointer of this Thread as parameter.

      // set stack to the virtual stack address
      state = ( State* ) ( virtualstack + stack_size - sizeof(State) );
    }

    _process->threads.pushBack( this );
    system->plan->push( this );
  }

  Thread::Thread( Process* P, uint32 Entry, uint32 StackSize )
      : _process( P ), _id( 0 ), func( Entry ), stack_size( StackSize ), state( 0 ) {
    init();
  }

  Thread::Thread( Process* P, uint32 StackSize )
      : _process( P ), _id( 0 ), func( 0 ), stack_size( StackSize ), state( 0 ) {
    init();
  }

  uint32 Thread::id() const {
    return _id;
  }

  Process* Thread::process() const {
    return _process;
  }

  void Thread::join() {
    while ( mode != DEAD ) {
      yield();
    }
  }

  void Thread::yield() {
    asm volatile( "int $0x20;" );
    // interrupt 32(0x20) is the timer interrupt which invokes the dispatcher
  }

  void Thread::sleep( uint32 mircosec ) {
    //todo sleep not implemented
  }

  void Thread::kill() {
    mode = DEAD;
  }

  Thread::~Thread() {
    if ( stack_size )
      delete stack;

    _process->threads.remove( this );
    system->plan->remove( this );
  }

}
