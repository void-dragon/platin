/**
 * AbstractMemory.hpp
 *
 * @date 12.05.2012
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef ABSTRACTMEMORY_HPP_
#define ABSTRACTMEMORY_HPP_

#include <lib/sync/Mutex.hpp>
#include <lib/collection/RawAAMap.hpp>

namespace kernel {

  class AbstractMemory {
    protected:
      /**
       * If pushed in the size map for free memory:
       *    - key = size of the area
       *    - value = address of the area
       *
       * If pushed in the address map for allocated memory:
       *    - key = the address of the area
       *    - value = the size of the area
       */
      typedef lib::collection::RawAAMap::Node Area;

      typedef uint8 NodeBlock[ 4096 ]; ///< Block with area nodes. 4096 / sizeof(Area) = 204
      typedef uint32 NodeAddrBlock[ 1024 ]; ///< Block with free node addresses.

      uint8* node_last_block; ///< 204 address blocks.

      // stack of free address nodes
      uint32* node_addr_stack;
      uint32 node_addr_pointer;

      void node_put( Area* node );

      Area* node_get();

      Area* get_node_for_size( uint32 size );

    public:
      uint32 memsize; ///< Size of the usable memory in byte.
      uint32 memused; ///< Size of the used memory in byte.
      lib::sync::Mutex mutex;
      lib::collection::RawAAMap addresses;
      lib::collection::RawAAMap sizes;

      static void init( Area* a );

      static void clear( Area* a );

      AbstractMemory();

      virtual ~AbstractMemory();
  };

} /* namespace minerva */
#endif /* ABSTRACTMEMORY_HPP_ */
