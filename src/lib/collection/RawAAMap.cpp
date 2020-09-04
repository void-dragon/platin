/**
 * RawAAMap.cpp
 *
 * @since 25.03.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */
#include "RawAAMap.hpp"
#include <lib/std.hpp>
#include <lib/Exception.hpp>

namespace lib {

  namespace collection {

    RawAAMap::RawAAMap() :
      root( 0 ), tmp( 0 ), count( 0 ) {
    }

    void RawAAMap::put( Node* n ) {
      if ( n == 0 ) {
        lib::Exception::throwing( "can not handle NULL node" );
      }

      tmp = n;
      tmp->left = 0;
      tmp->right = 0;
      tmp->level = 1;
      root = insert( root );
      tmp = 0;
    }

    void RawAAMap::del( Node* n ) {
      uint32 c = count;
      if ( !n )
        lib::Exception::throwing( "can not delete null node!" );
      tmp = n;
      root = remove( root );
      tmp = 0;
      if ( c <= count )
        lib::Exception::throwing( "exotic error" );
    }

    RawAAMap::Node* RawAAMap::skew( Node* n ) {
      if ( n && n->left && n->left->level == n->level ) {
        Node* l = n->left;
        n->left = l->right;
        l->right = n;
        return l;
      }

      return n;
    }

    RawAAMap::Node* RawAAMap::split( Node* n ) {
      if ( n && n->right && n->right->right && n->level == n->right->right->level ) {
        Node* r = n->right;
        n->right = r->left;
        r->left = n;
        r->level++;
        return r;
      }

      return n;
    }

    RawAAMap::Node* RawAAMap::near( uint32 k, Node* n ) {
      if ( n ) {
        Node* x;

        if ( n->key > k ) {
          if ( ( x = near( k, n->left ) ) ) {
            n = x;
          }
          else if ( ( x = near( k, n->right ) ) ) {
            n = x;
          }
        }
        else if ( n->key < k ) {
          if ( ( x = near( k, n->right ) ) ) {
            n = x;
          }
          else {
            return 0;
          }
        }
      }

      return n;
    }

    RawAAMap::Node* RawAAMap::nearest( uint32 k ) {
      if ( count == 0 ) {
        return 0;
      }

      return near( k, root );
    }

    RawAAMap::Node* RawAAMap::find( uint32 addr ) {
      if ( count == 0 ) {
        return 0;
      }

      Node *n = root;

      while ( n ) {
        if ( n->key > addr ) {
          n = n->left;
        }
        else if ( n->key < addr ) {
          n = n->right;
        }
        else {
          return n;
        }
      }

      return 0;
    }

    RawAAMap::Node* RawAAMap::insert( Node* r ) {

      if ( r ) {
        if ( r == tmp ) {
          lib::Exception::throwing( "mystical error!" );
        }

        if ( r->key > tmp->key ) {
          r->left = insert( r->left );
        }
        else if ( r->key < tmp->key ) {
          r->right = insert( r->right );
        }
        else {
          count++;
        }

        r = skew( r );
        r = split( r );

        if ( r == r->left || r == r->right )
          lib::Exception::throwing( "tree loop!" );

        return r;
      }

      count++;
      return tmp;
    }

    RawAAMap::Node* RawAAMap::successor( Node* n ) {
      n = n->right;

      while ( n->left ) {
        n = n->left;
      }

      return n;
    }

    RawAAMap::Node* RawAAMap::predecessor( Node* n ) {
      n = n->left;

      while ( n->right ) {
        n = n->right;
      }

      return n;
    }

    RawAAMap::Node* RawAAMap::decrease_level( Node* n ) {
      int shouldbe = 1;

      if ( n->left && n->right ) {
        shouldbe += lib::min( n->left->level, n->right->level );
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

    RawAAMap::Node* RawAAMap::remove( Node* r ) {
      if ( r ) {

        if ( r->key > tmp->key ) {
          r->left = remove( r->left );
        }
        else if ( r->key < tmp->key ) {
          r->right = remove( r->right );
        }
        else {
          Node* L = 0;

         if ( !( r->left || r->right ) ) {
            //delete r;
            //areas--;
            count--;
            return 0;
          }
          else if ( r->right ) {
            L = successor( r );
            tmp = L;
            r->right = remove( r->right );
          }
          else {
            L = predecessor( r );
            tmp = L;
            r->left = remove( r->left );
          }

          if ( !L )
            lib::Exception::throwing( "removing went wrong!" );

          L->right = r->right;
          L->left = r->left;
          L->level = r->level;
          r = L;
        }

        r = decrease_level( r );

        r = skew( r );
        r->right = skew( r->right );

        r = split( r );
        r->right = split( r->right );

        if ( r == r->left || r == r->right )
          lib::Exception::throwing( "tree loop!" );
      }

      return r;
    }

  }

}
