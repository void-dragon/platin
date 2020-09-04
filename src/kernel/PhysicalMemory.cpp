/**
 * PhysicalMemory.cpp
 *
 * @since 25.03.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "PhysicalMemory.hpp"
#include <kernel/System.hpp>
#include <lib/std.hpp>
#include <lib/Exception.hpp>

extern uint32 kernel_size;

namespace kernel {

  void PhysicalMemory::analyse( MultiBoot* M ) {
    MmapAddr* memorymap = M->mmap_addr;
    uint32 Size = M->mmap_length / 20 - 1;

    // quick and dirty
    // we look for the address 0x0100000, which on x86 system is
    // mostly the start point of the non IO-mapped memory area which
    // has usually the size of the whole usable RAM.
    for ( uint32 i = 0; i < Size; i++ ) {
      if ( memorymap[ i ].BaseAddr == Kernel_Start ) {
        memsize = memorymap[ i ].Length;
        break;
      }
    }

    memused = 0;
    node_addr_pointer = 0;

    // set buddy tree behind the kernel
    node_addr_stack = ( uint32* ) ( Kernel_Start + ( uint32 ) &kernel_size );

    // page align buddy tree address
    if ( ( uint32 ) node_addr_stack % PAGE_SIZE ) {
      node_addr_stack = ( uint32* ) ( ( uint32 ) node_addr_stack + PAGE_SIZE - ( uint32 ) node_addr_stack % PAGE_SIZE );
    }

    node_last_block = ( uint8* ) ( ( uint32 ) node_addr_stack + PAGE_SIZE );

    memsize -= ( uint32 ) node_last_block + PAGE_SIZE - Kernel_Start;

    usable_mem_start = ( uint32 ) node_last_block + PAGE_SIZE;

    // pushing all free tree nodes onto the stack
    for ( uint32 i = 16; i < 4096; i += sizeof(Area) ) {
      node_put( ( Area* ) ( node_last_block + i ) );
    }

    Area* a = node_get();
    init( a );

    a->key = memsize;
    a->val = usable_mem_start;

    sizes.put( a );

    if ( usable_mem_start % PAGE_SIZE ) {
      usable_mem_start += PAGE_SIZE - ( usable_mem_start % PAGE_SIZE );
    }

  }

  uint32 PhysicalMemory::alloc( uint32 blks ) {
    if ( memused >= memsize ) {
      lib::Exception::throwing( "PhysicalMemory - no physical memory left!" );
    }

    uint32 size = blks * PAGE_SIZE;

    mutex.enter();

    Area* node = sizes.find( size );

    if ( node == 0 ) {
      node = sizes.nearest( size );
    }
    else {
      lib::Exception::throwing( "PhysicalMemory - no memory block of appropriate size!" );
    }

    if ( node->key < PAGE_SIZE ) {
      lib::Exception::throwing( "PhysicalMemory - picked too small memory area!" );
    }

    sizes.del( node );

    uint32 ptr = node->val; // the usable memory address!!

    // we have more memory than we need and it is wealthy to manage the memory
    if ( ( node->key - size ) >= PAGE_SIZE ) {
      Area* n = node_get();

      init( n );

      n->key = node->key - size;
      n->val = node->val + size;

      node->key = size;

      sizes.put( n );
    }

    uint32 tmp = node->key;
    node->key = node->val;
    node->val = tmp;

    clear( node );

    addresses.put( node );

    memused += tmp;

    mutex.leave();

    lib::memset( ( void* ) ptr, 0, tmp );

    return ptr;
  }

  void PhysicalMemory::free( void* v ) {
    if ( v == 0 ) {
      lib::Exception::throwing( "PhysicalMemory - can not free null!" );
    }

    Area* node = addresses.find( ( uint32 ) v );

    if ( node == 0 ) {
      lib::Exception::throwing( "PhysicalMemory - freeing unknown memory area!" );
    }

    addresses.del( node );

    uint32 tmp = node->key;
    node->key = node->val;
    node->val = tmp;

    memused -= node->key;

    clear( node );

    sizes.put( node );
  }

  PhysicalMemory::~PhysicalMemory() {
  }

}
