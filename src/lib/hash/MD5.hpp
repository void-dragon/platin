/**
 * MD5.hpp
 *
 * @since 14.04.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef MD5_HPP_
#define MD5_HPP_

#include <cpp.hpp>

namespace lib {

  namespace hash {

    /**
     * @since 14.04.2010
     * @author Arne Simon => email::[arne_simon@gmx.de]
     */
    class MD5 {
      protected:
        static const uint32 r[ 64 ];
        static const uint32 k[ 64 ];

        static uint32 leftrotate( uint32 var, uint32 n );

        uint8 digest[ 16 ];

      public:

        /**
         *
         * @param Data
         * @param Size
         * @param hash An array of 16 bytes which represent the md5 hash.
         */
        static void hash( uint8 hash[ 16 ], uint8* Data, uint32 Size );

        MD5( uint32 x = 0 );

        MD5( const MD5& md5 );

        MD5( uint8* Data, uint32 len );

        uint8 operator[]( uint32 i );

        bool operator<( const MD5 m ) const;

        bool operator>( const MD5 m ) const;

        bool operator==( const MD5 m ) const;
    };

  } /* namespace hash */

}

#endif /* MD5_HPP_ */
