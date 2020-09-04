/**
 * PhysicalMemory.hpp
 *
 * @since 25.03.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef KERNEL_PHYSICALMEMORY_HPP_
#define KERNEL_PHYSICALMEMORY_HPP_

#include <cpp.hpp>
#include <MultiBoot.hpp>
#include <kernel/AbstractMemory.hpp>

namespace kernel {

  class Process;

  /**
   * Manages the physical memory in 4096 byte blocks.
   *
   * A tree structure is used for handling the blocks.
   *
   * @since 25.03.2011
   * @date 12.05.2012
   * @author Arne Simon => email::[arne_simon@gmx.de]
   */
  class PhysicalMemory: public AbstractMemory {

    public:
      static const uint32 Kernel_Start = 0x0100000;
      static const uint32 PAGE_SIZE = 4096;

      uint32 usable_mem_start;

      /**
       * Sets up the memory management and scans the memory map structure for
       * usable and unusable memory areas.
       *
       * @param M MultiBoot structure.
       */
      void analyse( MultiBoot* M );

      /**
       * Allocates one page.
       *
       * @return Returns the physical address of the allocated memory.
       */
      uint32 alloc( uint32 blks = 1 );

      /**
       * Deallocates/frees a 4Kb block of memory associated with a given address.
       *
       * @param v The physical address of the memory block which will be deallocated.
       */
      void free( void* v );

      virtual ~PhysicalMemory();
  };

}

#endif /* KERNEL_PHYSICALMEMORY_HPP_ */
