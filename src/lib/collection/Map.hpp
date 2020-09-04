/**
 * Map.hpp
 *
 * @since 28.06.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_COLLECTION_MAP_HPP_
#define LIB_COLLECTION_MAP_HPP_

#include <cpp.hpp>
#include "Iterator.hpp"

namespace lib {

   namespace collection {

      template< class T > int32 MapDefaultCompare( T& a, T& b ) {
         return ( int32 ) a - ( int32 ) b;
      }

      /**
       * An implementation of an AA-Tree.
       *
       * @note for visualization: http://people.ksp.sk/~kuko/bak/index.html
       *
       * @since 28.06.2010
       * @author Arne Simon => email::[arne_simon@gmx.de]
       */
      template< class K, class V, int32(*Compare)( K&, K& ) = &MapDefaultCompare > class Map {
         public:
            class Node {
               public:
                  Node* prev; ///< Previous element for iterator chain.
                  Node* next; ///< Next element for iterator chain.
                  Node* left;
                  Node* right;
                  K key;
                  V value;
                  int32 level;

                  Node()
                        : prev( 0 ), next( 0 ), left( 0 ), right( 0 ), level( 1 ) {

                  }
            };

         protected:

            Node* root;
            Node* head;
            Node* tail;
            K tmpk;
            V tmpv;
            uint32 count;

            Node* skew( Node* n ) {
               if ( n && n->left && n->left->level == n->level ) {
                  Node* l = n->left;
                  n->left = l->right;
                  l->right = n;
                  return l;
               }

               return n;
            }

            Node* split( Node* n ) {
               if ( n && n->right && n->right->right && n->level == n->right->right->level ) {
                  Node* r = n->right;
                  n->right = r->left;
                  r->left = n;
                  r->level++;
                  return r;
               }

               return n;
            }

            Node* insert( Node* r ) {
               Node *tmp = 0;

               if ( r ) {
                  int32 diff = Compare( r->key, tmpk );

                  if ( diff > 0 ) {
                     r->left = insert( r->left );
                  }
                  else if ( diff < 0 ) {
                     r->right = insert( r->right );
                  }
                  else {
                     r->value = tmpv;
                     return r;
                  }

                  r = skew( r );
                  r = split( r );

                  return r;
               }

               tmp = new Node();

               tmp->key = tmpk;
               tmp->value = tmpv;

               if ( tail )
                  tail->next = tmp;

               tmp->prev = tail;
               tail = tmp;

               if ( count == 0 ) {
                  head = tail;
               }

               count++;

               return tmp;
            }

            Node* successor( Node* n ) {
               n = n->right;
               while ( n->left )
                  n = n->left;
               return n;
            }

            Node* predecessor( Node* n ) {
               n = n->left;
               while ( n->right )
                  n = n->right;
               return n;
            }

            static inline int min( int a, int b ) {
               return a < b ? a : b;
            }

            Node* decrease_level( Node* n ) {
               int shouldbe = 1;

               if ( n->left && n->right ) {
                  shouldbe += min( n->left->level, n->right->level );
               }
               else if ( n->left ) {
                  shouldbe += n->left->level;
               }
               else if ( n->right ) {
                  shouldbe += n->right->level;
               }

               if ( shouldbe < n->level ) {
                  n->level = shouldbe;
                  if ( shouldbe < n->right->level ) {
                     n->right->level = shouldbe;
                  }
               }

               return n;
            }

            Node* remove( Node* r ) {
               if ( r ) {

                  int32 diff = Compare( r->key, tmpk );

                  if ( diff > 0 ) {
                     r->left = remove( r->left );
                  }
                  else if ( diff < 0 ) {
                     r->right = remove( r->right );
                  }
                  else {
                     if ( !( r->left || r->right ) ) {
                        if ( count == 1 ) {
                           head = tail = 0;
                        }
                        else {
                           if ( r->next )
                              r->next->prev = r->prev;
                           if ( r->prev )
                              r->prev->next = r->next;
                        }

                        delete r;
                        count--;
                        return 0;
                     }
                     else if ( r->right ) {
                        Node* L = successor( r );
                        tmpk = L->key;
                        r->right = remove( r->right );
                        r->key = L->key;
                        r->value = L->value;
                     }
                     else {
                        Node* L = predecessor( r );
                        tmpk = L->key;
                        r->left = remove( r->left );
                        r->key = L->key;
                        r->value = L->value;
                     }
                  }

                  r = decrease_level( r );

                  r = skew( r );
                  r->right = skew( r->right );

                  r = split( r );
                  r->right = split( r->right );

               }

               return r;
            }

         public:

            class KeyIterator: public lib::collection::Iterator< K > {
                  friend class Map;
               protected:
                  Node* _n;

                  KeyIterator( Node* N )
                        : _n( N ) {

                  }
               public:

                  void operator++() {
                     if ( _n )
                        _n = _n->next;
                  }

                  K& operator*() {
                     return _n->key;
                  }

                  V& value() {
                     return _n->value;
                  }

                  operator bool() {
                     return _n ? true : false;
                  }

            };

            class ValueIterator: public lib::collection::Iterator< V > {
                  friend class Map;
               protected:
                  Node* _n;

                  ValueIterator( Node* N )
                        : _n( N ) {

                  }
               public:

                  void operator++() {
                     if ( _n )
                        _n = _n->next;
                  }

                  V& operator*() {
                     return _n->value;
                  }

                  K& key() {
                     return _n->key;
                  }

                  operator bool() {
                     return _n ? true : false;
                  }

            };

            Map()
                  : root( 0 ), head( 0 ), tail( 0 ), count( 0 ) {

            }

            virtual KeyIterator* keyIterator() {
               return new KeyIterator( head );
            }

            virtual ValueIterator* valueIterator() {
               return new ValueIterator( head );
            }

            uint32 size() const {
               return count;
            }

            void clear() {
               delete root;

               root = 0;
               head = 0;
               tail = 0;
               count = 0;
            }

            virtual void put( const K& k, const V& v ) {
               tmpk = k;
               tmpv = v;

               root = insert( root );
            }

            Node* find( K k ) {
               if ( count == 0 ) {
                  return 0;
               }

               Node *n = root;

               while ( n ) {
                  int32 diff = Compare( n->key, k );

                  if ( diff > 0 ) {
                     n = n->left;
                  }
                  else if ( diff < 0 ) {
                     n = n->right;
                  }
                  else {
                     return n;
                  }
               }

               return 0;
            }

            virtual bool has( const K& k ) {
               Node *n = root;

               while ( n ) {
                  int32 diff = Compare( n->key, tmpk );

                  if ( diff > 0 ) {
                     n = n->left;
                  }
                  else if ( diff < 0 ) {
                     n = n->right;
                  }
                  else {
                     return true;
                  }
               }

               return false;
            }

            virtual void remove( const K& k ) {
               tmpk = k;
               root = remove( root );
            }

            virtual ~Map() {
               for ( Node* n = head; n; ) {
                  Node* x = n->next;
                  delete n;
                  n = x;
               }
            }
      };

   }

}

#endif /* LIB_COLLECTION_MAP_HPP_ */
