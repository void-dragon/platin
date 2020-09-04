/**
 * List.hpp
 *
 * @since 23.04.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_COLLECTION_LIST_HPP_
#define LIB_COLLECTION_LIST_HPP_

#include "Iterator.hpp"
#include "Collection.hpp"
#include <lib/Exception.hpp>

namespace lib {

  namespace collection {

    /**
     * A double linked List.
     *
     * @since 23.04.2010
     * @date 30.06.2010
     * @author Arne Simon => email::[arne_simon@gmx.de]
     */
    template< class T > class List: public Collection< T > {
      protected:
        class Node {
            friend class List;
            Node* next;
            Node* prev;
            T data;

            Node() :
              next( 0 ), prev( 0 ) {

            }
        };

        Node* head;
        Node* tail;
        uint32 count;
      public:
        class Iterator: public collection::Iterator< T > {
            friend class List;
          private:
            Node* n;

            Iterator( Node* N ) :
              n( N ) {

            }
          public:
            bool hasNext() {
              if ( n )
                return n->next != 0;

              return false;
            }

            bool hasPrev() {
              if ( n )
                return n->prev != 0;

              return false;
            }

            T& operator*() {
              return n->data;
            }

            void operator++() {
              if ( n )
                n = n->next;
            }

            void operator--() {
              if ( n )
                n = n->prev;
            }

            operator bool() {
              return n ? true : false;
            }

            ~Iterator() {

            }
        };

        List() :
          head( 0 ), tail( 0 ), count( 0 ) {

        }

        ~List() {
          for ( Node* i = head; i; i = i->next ) {
            delete i;
          }
        }

        /**
         * The number of items in the list.
         * @return The number of items.
         */
        uint32 size() {
          return count;
        }

        T& first() {
          if ( count <= 0 )
            Exception::throwing( "list is empty!" );

          return head->data;
        }

        T& last() {
          if ( count <= 0 )
            Exception::throwing( "list is empty!" );

          return tail->data;
        }

        void pushBack( T t ) {
          if ( tail ) {
            tail->next = new Node();
            tail->next->prev = tail;
            tail = tail->next;
            tail->data = t;
          }
          else {
            tail = head = new Node();
            tail->data = t;
          }
          count++;
        }

        void pushFront( T t ) {
          if ( head ) {
            Node* n = new Node();
            n->next = head;
            head->prev = n;
            head = n;
            head->data = t;
          }
          else {
            head = tail = new Node();
            head->data = t;
          }
          count++;
        }

        void popBack() {
          if ( tail ) {
            Node* n = tail->prev;

            if ( head == tail ) {
              head = n;
            }

            delete tail;
            tail = n;
            count--;
          }
        }

        void popFront() {
          if ( head ) {
            Node* n = head->next;

            if ( head == tail ) {
              tail = n;
            }

            delete head;
            head = n;
            count--;
          }
        }

        void remove( T t ) {
          for ( Node* i = head; i; i = i->next ) {
            if ( i->data == t ) {

              if ( i->prev )
                i->prev->next = i->next;

              if ( i->next )
                i->next->prev = i->prev;

              delete i;
              count--;
              break;
            }
          }
        }

        Iterator* iterator() {
          return new Iterator( head );
        }
    };

  }

}

#endif /* LIB_COLLECTION_LIST_HPP_ */
