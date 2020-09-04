/**
 * Ring.hpp
 *
 * @since 05.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef RING_HPP_
#define RING_HPP_

#include "Iterator.hpp"
#include <lib/Exception.hpp>

namespace lib {

  namespace collection {

    /**
     * A ring of elements.
     */
    template< class T > class Ring {
      protected:
        struct Node {
            T data;
            Node* next;
        };

        Node* _start;
        uint32 _size;

      public:
        class Iterator: public collection::Iterator< T > {
            friend class Ring;
          protected:
            Node* start;
            Node* n;

            Iterator( Node* N ) :
              start( N ), n( N ) {

            }
          public:
            bool isStart() {
              return n == start;
            }

            T& operator*() {
              return n->data;
            }

            void operator++() {
              if ( n )
                n = n->next;
            }

            operator bool() {
              return true;
            }
        };

        Ring() :
          _start( 0 ), _size( 0 ) {

        }

        Iterator* iterator() {
          return new Iterator( _start );
        }

        /**
         * @note O( idx )
         * @param idx
         * @return
         */
        T& get( uint32 idx ) {
          if ( idx > _size )
            Exception::throwing( "Ring: out of bound!" );

          Node* n = _start;

          for ( uint32 i = 0; n && i < idx; ++i ) {
            n = n->next;
          }

          return n->data;
        }

        /**
         *
         * @param t
         */
        void push( T t ) {
          if ( _start ) {
            Node *n = new Node();
            n->next = _start->next;
            n->data = t;
            _start->next = n;
          }
          else {
            _start = new Node();
            _start->next = _start;
            _start->data = t;
          }

          _size++;
        }

        /**
         *
         * @param t
         */
        void remove( T t ) {
          if ( _start->data == t ) {
            _size--;
            Node* n = _start;
            _start = n->next;
            delete n;
          }
          else {
            Node* pre = _start;
            for ( Node* i = _start->next; i != _start; i = i->next ) {
              if ( i->data == t ) {
                _size--;
                pre->next = i->next;
                delete i;
              }
            }
          }
        }

        uint32 size() const {
          return _size;
        }

    };

  }

}

#endif /* RING_HPP_ */
