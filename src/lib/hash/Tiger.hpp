/**
 * Tiger.hpp
 *
 * @date 22.10.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_TIGER_HPP_
#define LIB_TIGER_HPP_

#include <cpp.hpp>

namespace lib {

  namespace hash {

    typedef uint64 TigerHashData[ 3 ];

    union Tiger {
        TigerHashData data;
        struct {
            uint64 first;
            uint64 second;
            uint64 third;
        };

        bool operator==(Tiger& other ){
          return first == other.first && second == other.second && third == other.third;
        }
    };

    /**
     * Creates a tiger hash from a string.
     *
     * @note http://www.cs.technion.ac.il/~biham/Reports/Tiger/
     *
     * @param str
     * @param length
     * @param res
     */
    void tiger( uint64 *str, uint64 length, TigerHashData res );

  } /* namespace hash */

} /* namespace lib */

#endif /* LIB_TIGER_HPP_ */
