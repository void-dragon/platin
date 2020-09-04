/**
 * Process.cpp
 *
 * @since 08.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Process.hpp"
#include <kernel/System.hpp>

namespace kernel {

  Process::Process() {
    // our system is a process, too.
    // but does not use standard process setup :)
    if ( system ) {
      user = 0;

      _id = system->ProcessIDPool++;
      state = Process::Active;

      virtual_memory.page_directoies = new uint32[ 1024 ];
      virtual_memory.memsize = PhysicalMemory::PAGE_SIZE;

      virtual_memory.node_addr_pointer = 0;
      virtual_memory.node_addr_stack = new VirtualMemory::NodeAddrBlock;
      virtual_memory.node_last_block = new VirtualMemory::NodeBlock;

      // pushing all free tree nodes onto the stack
      for ( uint32 i = 16; i < 4096; i += sizeof(VirtualMemory::Area) ) {
        virtual_memory.node_put( ( VirtualMemory::Area* ) ( virtual_memory.node_last_block + i ) );
      }

      for ( uint32 i = PhysicalMemory::Kernel_Start; i < System::physical_memory.usable_mem_start; i +=
          PhysicalMemory::PAGE_SIZE ) {
        virtual_memory.map( i, i );
      }
      virtual_memory.map( ( uint32 ) system, ( uint32 ) system );

      system->processes.pushBack( this );
    }
  }

  Process::~Process() {
    Threads::Iterator* i = threads.iterator();

    while ( *i ) {
      ( **i )->kill();
    }
    delete i;

    system->processes.remove( this );
  }

}
