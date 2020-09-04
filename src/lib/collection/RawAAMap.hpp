/**
 * RawAAMap.hpp
 *
 * @since 25.03.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef RAWAAMAP_HPP_
#define RAWAAMAP_HPP_

#include <cpp.hpp>

namespace lib {

  namespace collection {

    struct RawAAMap {

        struct Node {
            uint32 key;
            uint32 val;
            int level;
            Node* left;
            Node* right;
        };

        Node* root;
        Node* tmp;
        uint32 count;

        RawAAMap();

        void put( Node* n );

        void del( Node* n );

        Node* skew( Node* n );

        Node* split( Node* n );

        Node* nearest( uint32 k );

        Node* near( uint32 k, Node* n );

        Node* find( uint32 addr );

        Node* insert( Node* r );

        Node* successor( Node* n );

        Node* predecessor( Node* n );

        Node* decrease_level( Node* n );

        Node* remove( Node* r );
    };

  }

}

#endif /* RAWAAMAP_HPP_ */
