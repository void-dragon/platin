/**
 * Array.hpp
 *
 * @since 12.05.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_COLLECTION_HPP_
#define LIB_COLLECTION_HPP_

#include "Iterator.hpp"

namespace lib {

  namespace collection {

    template< class V > class Collection {
      public:
        virtual Iterator< V >* iterator() = 0;

        virtual uint32 size() = 0;
    };

  }

}

#endif /* LIB_COLLECTION_HPP_ */
