/**
 * VirtualMemory.hpp
 *
 * @since 26.03.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef KERNEL_VIRTUALMEMORY_HPP_
#define KERNEL_VIRTUALMEMORY_HPP_

#include <kernel/PhysicalMemory.hpp>
#include <kernel/AbstractMemory.hpp>

namespace kernel {
  class Process;

  /**
   * Handles the virtual memory for a Process.
   *
   * This class handles two main tasks:
   * <ol>
   *  <li>Allocating/Freeing virtual memory</li>
   *  <li>Mapping of virtual and real memory areas</li>
   * </ol>
   *
   * @code
   * +------------------+
   * |  Page Directory  |
   * +------------------+
   * | 4Kb aligned Addr |-------
   * | of a Page Table  |      |
   * +------------------+      |  +-------------------+
   * |      ....        |      ---|     Page Table    |
   * +------------------+         +-------------------+
   *                              | 4Kb aligned Addr. |         +----------------+
   *                              | of 4Kb physical   |---------|  Memory Block  |
   *                              | Memory Block      |         +----------------+
   *                              +-------------------+
   *                              |      ....         |
   *                              +-------------------+
   *
   * Virtual Address:
   * 32                    22                 12                  0
   * +----------------------+------------------+------------------+
   * | Page Directory Index | Page Table Index | Offset From Page |
   * +----------------------+------------------+------------------+
   * @endcode
   *
   * @note http://www.viralpatel.net/taj/tutorial/paging.php
   * @note http://wiki.osdev.org/Paging
   * @note http://en.wikipedia.org/wiki/Binomial_tree
   *
   * @since 26.03.2011
   * @date 22.05.2011
   * @author Arne Simon => email::[arne_simon@gmx.de]
   */
  class VirtualMemory : public AbstractMemory {
    private:
      friend class System;
      friend class Process;

    public:

      /**
       * The physical address of the page directory.
       * @attention The address has to be 4Kb aligned!
       */
      uint32* page_directoies;

      /**
       * Expands the virtual memory size.
       *
       * @param Size
       */
      void expandForSize( uint32 Size );

      /**
       * Maps a physical 4096 byte block to a virtual 4096 byte block of memory.
       *
       * @note Addresses have to be 4096 byte block aligned!
       *
       * @param Physical
       * @param Virtual
       */
      void map( uint32 Physical, uint32 Virtual );

      /**
       * Returns the physical address for a virtual address.
       *
       * @param Virtual The virtual address.
       * @return The physical address.
       */
      uint32 getPhysicalAddress( uint32 Virtual );

      /**
       * Allocates memory of a given size.
       *
       * @note The allocated memory is set to zero!
       *
       * @param size The size of the memory to be allocated.
       */
      void* alloc( uint32 size );

      /**
       * Reallocates a memory area.
       *
       * @param v
       * @param size
       */
      void* realloc( void* v, uint32 size );

      /**
       * Deallocates/frees the memory associated with a given address.
       *
       * @param v The address of the memory which will be deallocated.
       */
      void free( void* v );

      /**
       * Frees all obtained physical memory.
       */
      virtual ~VirtualMemory();
  };

}

#endif /* KERNEL_VIRTUALMEMORY_HPP_ */
