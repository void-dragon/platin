/**
 * VirtualMemory.cpp
 *
 * @since 26.03.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "VirtualMemory.hpp"
#include <lib/Exception.hpp>
#include <lib/std.hpp>
#include <kernel/System.hpp>

namespace kernel {

  void VirtualMemory::expandForSize( uint32 Size ) {
    uint32 blocks = ( Size ) / PhysicalMemory::PAGE_SIZE;

    if ( Size % PhysicalMemory::PAGE_SIZE ) {
      blocks++;
    }

    Area* a = node_get();

    init( a );

    uint32 page = System::physical_memory.alloc( blocks );

    if ( page_directoies ) {
      a->val = memsize; // start of the memory area

      for ( uint32 p = 0; p < blocks; ++p ) {

        map( page + p * PhysicalMemory::PAGE_SIZE, memsize );
        memsize += PhysicalMemory::PAGE_SIZE;
      }
    }
    else {
      a->val = page;
    }

    a->key = blocks * PhysicalMemory::PAGE_SIZE; // size of the memory area

    sizes.put( a );
  }

  void VirtualMemory::map( uint32 Physical, uint32 Virtual ) {
    if ( page_directoies ) {
      uint32 pd_index = Virtual >> 22;
      uint32 pt_index = ( Virtual >> 12 ) & 0x03FF;

      if ( ( page_directoies[ pd_index ] & 0xFFFFF000 ) == 0 ) {

        uint32 addr = System::physical_memory.alloc();

        page_directoies[ pd_index ] = addr | 0x02 | 0x01;
      }

      uint32* pt = ( uint32* ) ( page_directoies[ pd_index ] & 0xFFFFF000 );

      pt[ pt_index ] = ( Physical & 0xFFFFF000 ) | 0x02 | 0x01;
    }
  }

  uint32 VirtualMemory::getPhysicalAddress( uint32 Virtual ) {
    if ( page_directoies ) {
      uint32 pd_index = Virtual >> 22;
      uint32 pt_index = ( Virtual >> 12 ) & 0x03FF;

      uint32* pt = ( uint32* ) ( page_directoies[ pd_index ] & 0xFFFFF000 );

      return ( pt[ pt_index ] & 0xFFFFF000 ) | ( Virtual & 0xFFF );
    }
    else {
      return Virtual;
    }
  }

  void* VirtualMemory::alloc( uint32 size ) {
    uint32 ptr = 0;

    if ( size == 0 ) {
      lib::Exception::throwing( "VirtualMemory - allocating zero space ?!?" );
    }

    mutex.enter();

    Area* node = sizes.find( size );

    if ( node == 0 ) {
      node = sizes.nearest( size );
    }

    if ( node == 0 ) {
      if ( System::physical_memory.memused < System::physical_memory.memsize ) {
        expandForSize( size );

        node = sizes.nearest( size );
      }
      else {
        lib::Exception::throwing( "VirtualMemory - no memory block of appropriate size!" );
      }
    }

    if ( node->key < size ) {
      lib::Exception::throwing( "VirtualMemory - picked too small memory area!" );
    }

    sizes.del( node );

    ptr = node->val; // the usable memory address!!

    // we have more memory than we need and it is wealthy to manage the memory
    if ( ( node->key - size ) > ( sizeof(Area) * 2 ) ) {
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

    lib::memset( ( void* ) getPhysicalAddress( ptr ), 0, tmp );

    return ( void* ) ptr;
  }

  void* VirtualMemory::realloc( void* v, uint32 size ) {

    if ( v == 0 ) {
      lib::Exception::throwing( "VirtualMemory - can not reallocate zero pointer!" );
    }

    if ( size == 0 ) {
      lib::Exception::throwing( "VirtualMemory - can not reallocate zero space!" );
    }

    Area* x = addresses.find( ( uint32 ) v );

    if ( ( uint32 ) x->val >= size ) { // if size of allocated memory area is greater or equal to the new size,
      return v; // we do not need to re-allocate.
    }

    void* _new = alloc( size ); // alloc new memory of new size

    lib::memcpy( _new, v, lib::min( ( uint32 ) x->val, size ) ); // copy from old to new

    free( v ); // free old area

    return _new; // return new memory address
  }

  void VirtualMemory::free( void* v ) {

    if ( v == 0 ) {
      lib::Exception::throwing( "VirtualMemory - can not free null!" );
    }

    Area* node = addresses.find( ( uint32 ) v );

    if ( node == 0 ) {
      lib::Exception::throwing( "VirtualMemory - freeing unknown memory area!" );
    }

    addresses.del( node );

    uint32 tmp = node->key;
    node->key = node->val;
    node->val = tmp;

    memused -= node->key;

    clear( node );

    sizes.put( node );

  }

  VirtualMemory::~VirtualMemory() {
//    uint32* pd = page_directoies;
//
//    lib::cli();
//    System::disablePaging();
//
//    for ( uint32 pd_index = 0; pd_index < 1024; ++pd_index ) {
//
//      if ( pd[ pd_index ] & 0xFFFFF000 ) {
//        uint32* pt = ( uint32* ) ( pd[ pd_index ] & 0xFFFFF000 );
//
//        for ( uint32 pt_index = 0; pt_index < 1024; ++pt_index ) {
//          if ( pt[ pt_index ] & 0xFFFFF000 ) {
//            System::physical_memory.free( ( void* ) ( pt[ pt_index ] & 0xFFFFF000 ) );
//          }
//        }
//
//        System::physical_memory.free( pt );
//      }
//    }
//
//    System::enablePaging();
//    lib::sti();
  }

}
