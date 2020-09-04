/**
 * Heap.hpp
 *
 * @since 23.06.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_COLLECTION_HEAP_HPP_
#define LIB_COLLECTION_HEAP_HPP_

#include <cpp.hpp>
#include <lib/std.hpp>
#include <lib/Exception.hpp>

namespace lib {

  namespace collection {

    template< class T > bool HeapMinCompare( T& a, T& b ) {
      return a <= b;
    }

    template< class T > bool HeapMaxCompare( T& a, T& b ) {
      return a >= b;
    }

    /**
     * A heap.
     *
     * @note By default the heap is a max-heap.
     */
    template< class T, bool(*Compare)( T& a, T& b ) = &HeapMaxCompare > class Heap {
      protected:

        T* heap;///< The actual data.
        uint32 heap_length;

        uint32 next;///< The index of the cell into which the next element will go.

        /**
         * Get the index of the parent for the element at the given index.
         */
        uint32 parent( uint32 index ) {
          return ( index - 1 ) / 2;
        }

        /**
         * Having just inserted, restore the heap invariant (that a node's value is greater than its children)
         */
        void fixup( uint32 index ) {
          if ( index == 0 )
            return;

          uint32 par = parent( index );
          if ( !Compare( heap[ par ], heap[ index ] ) ) {
            swap( par, index );
            fixup( par );
          }
        }

        /**
         * Having just removed and replaced the top of the heap with the last inserted element,
         * restore the heap invariant.
         */
        void fixdown( uint32 index ) {
          uint32 left = 2 * index + 1;
          uint32 down;

          if ( left >= next ) {
            return;
          }

          if ( left == next - 1 ) {
            down = left;
          }
          else if ( Compare( heap[ left ], heap[ left + 1 ] ) ) {
            down = left;
          }
          else {
            down = left + 1;
          }

          if ( !Compare( heap[ index ], heap[ down ] ) ) {
            swap( index, down );
            fixdown( down );
          }
        }

        /**
         * Swap two elements in the array.
         */
        void swap( uint32 a, uint32 b ) {
          T t1 = heap[ a ];
          T t2 = heap[ b ];
          heap[ a ] = t2;
          heap[ b ] = t1;
        }

      public:

        Heap( uint32 Reserve = 5 ) :
          heap_length( Reserve ), next( 0 ) {
          heap = ( T* ) lib::alloc( sizeof(T) * heap_length );
        }

        /**
         * Inserts the given element into the heap.
         */
        void push( T t ) {

          if ( next >= heap_length ) {
            Exception::throwing( "Heap : is full!" );
          }

          uint32 index = next++;

          while ( heap_length <= index ) {
            heap_length = 2 * heap_length + 32;
            heap = ( T* ) lib::realloc( heap, sizeof(T) * heap_length );
          }

          heap[ index ] = t;
          fixup( index );
        }

        /**
         * Removes the top of this heap and returns it.
         */
        T pop() {
          return removeAt( 0 );
        }

        /**
         * Remove the element at the given index from the heap.
         *
         * @attention The index is according to the heap's internal layout;
         * you are responsible for making sure the index is correct.
         *
         * The heap invariant is maintained.
         */
        T removeAt( uint32 index ) {
          if ( next <= index ) {
            Exception::throwing( "Heap : remove index is greater than heap size!" );
          }
          next--;
          T t = heap[ index ];
          // if next == index, then we have nothing valid on the heap
          // so popping does nothing but change the length
          // the other calls are irrelevant, but we surely don't want to
          // call Move with invalid data
          if ( next > index ) {
            heap[ index ] = heap[ next ];
            fixdown( index );

            if ( heap[ index ] == heap[ next ] )
              fixup( index );
          }

          return t;
        }

        /**
         * Gets the value at the top of the heap without removing it.
         */
        T peek() {
          if ( next == 0 ) {
            Exception::throwing( "Heap : peek on empty heap!" );
          }

          return heap[ 0 ];
        }

        /**
         *  Returns the number of elements in this heap.
         */
        uint32 size() {
          return next;
        }

        /**
         * Get the reserved capacity of this heap.
         */
        uint32 capacity() {
          return heap_length;
        }

        /**
         * Reset this heap.
         */
        void clear() {
          next = 0;
        }

    };

  }

}

#endif /* LIB_COLLECTION_HEAP_HPP_ */
