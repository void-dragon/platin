/**
 * MD5.cpp
 *
 * @since 14.04.2010
 * @author Arne Simon => email::<arne_simon@gmx.de>
 */

#include "MD5.hpp"
#include <lib/std.hpp>

namespace lib {

  namespace hash {

    const uint32 MD5::r[ 64 ] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14, 20, 5, 9, 14,
        20, 5, 9, 14, 20, 5, 9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6,
        10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

    const uint32 MD5::k[ 64 ] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613,
        0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x2441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6,
        0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681,
        0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085,
        0x4881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82,
        0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

    void MD5::hash( uint8 hash[ 16 ], uint8* Data, uint32 Size ) {
      uint32 w[ 16 ];
      uint32 h[ 4 ];
      h[ 0 ] = 0x67452301;
      h[ 1 ] = 0xEFCDAB89;
      h[ 2 ] = 0x98BADCFE;
      h[ 3 ] = 0x10325476;

      uint8* buff = new uint8[ Size + 64 ];

      memcpy( buff, Data, Size );

      buff[ Size ] = 0x80;

      uint32 len = Size;

      do {
        ++len;
        buff[ len ] = 0;
      } while ( ( len << 3 ) % 448 != 0 );

      Size <<= 3;

      buff[ len++ ] = Size;
      buff[ len++ ] = Size >> 8;
      buff[ len++ ] = Size >> 16;
      buff[ len++ ] = Size >> 24;
      buff[ len++ ] = 0;
      buff[ len++ ] = 0;
      buff[ len++ ] = 0;
      buff[ len++ ] = 0;

      //std::cout << "size: " << size << " len: " << len << std::endl;

      for ( uint32 q = 0; q < len; q += 64 ) {

        for ( uint32 i = 0, j = 0; i < 16; i++, j += 4 ) {
          w[ i ] = ( ( uint32 ) buff[ q + j ] ) | ( ( ( uint32 ) buff[ q + j + 1 ] ) << 8 )
              | ( ( ( uint32 ) buff[ q + j + 2 ] ) << 16 ) | ( ( ( uint32 ) buff[ q + j + 3 ] ) << 24 );
        }

        uint32 a = h[ 0 ];
        uint32 b = h[ 1 ];
        uint32 c = h[ 2 ];
        uint32 d = h[ 3 ];
        uint32 f, g, temp;

        for ( uint32 i = 0; i < 64; i++ ) {
          if ( i <= 15 ) {
            f = d ^ ( b & ( c ^ d ) );
            g = i;
          }
          else if ( i <= 31 ) {
            f = c ^ ( d & ( b ^ c ) );
            g = ( 5 * i + 1 ) % 16;
          }
          else if ( i <= 47 ) {
            f = b ^ c ^ d;
            g = ( 3 * i + 5 ) % 16;
          }
          else if ( i <= 63 ) {
            f = c ^ ( b | ( ~d ) );
            g = ( 7 * i ) % 16;
          }

          temp = d;
          d = c;
          c = b;
          b = b + leftrotate( ( a + f + k[ i ] + w[ g ] ), r[ i ] );
          a = temp;
        }

        //Add this chunk's hash to result so far:
        h[ 0 ] += a;
        h[ 1 ] += b;
        h[ 2 ] += c;
        h[ 3 ] += d;
      }

      delete buff;

      for ( uint32 i = 0, j = 0; j < 16; i++, j += 4 ) {
        hash[ j ] = ( uint8 ) ( h[ i ] & 0xff );
        hash[ j + 1 ] = ( uint8 ) ( ( h[ i ] >> 8 ) & 0xff );
        hash[ j + 2 ] = ( uint8 ) ( ( h[ i ] >> 16 ) & 0xff );
        hash[ j + 3 ] = ( uint8 ) ( ( h[ i ] >> 24 ) & 0xff );
      }
    }

    MD5::MD5( const MD5 &md5 ) {
      for ( uint32 i = 0; i < 16; i++ ) {
        digest[ i ] = md5.digest[ i ];
      }
    }

    MD5::MD5( uint32 x ) {
      for ( uint32 i = 0; i < 16; i++ ) {
        digest[ i ] = 0;
      }
    }

    uint32 MD5::leftrotate( uint32 var, uint32 n ) {
      return ( var << n ) | ( var >> ( 32 - n ) );
    }

    MD5::MD5( uint8* Data, uint32 Size ) {
      hash( digest, Data, Size );
    }

    uint8 MD5::operator[]( uint32 i ) {
      return digest[ i ];
    }

    //    kernel::Video& operator<<( std::ostream &stream, peers::MD5 context ) {
    //      char s[ 33 ];
    //
    //      for ( int i = 0; i < 16; i++ )
    //        sprintf( s + i * 2, "%02x", context.digest[ i ] );
    //
    //      s[ 32 ] = '\0';
    //
    //      stream << s;
    //
    //      return stream;
    //    }

    bool MD5::operator<( const MD5 md5 ) const {
      for ( uint8 i = 0; i < 16; ++i ) {
        if ( digest[ i ] > md5.digest[ i ] ) {
          return false;
        }
        else if ( digest[ i ] < md5.digest[ i ] ) {
          return true;
        }
      }

      return false;
    }

    bool MD5::operator>( const MD5 md5 ) const {
      for ( uint8 i = 0; i < 16; ++i ) {
        if ( digest[ i ] > md5.digest[ i ] ) {
          return true;
        }
        else if ( digest[ i ] < md5.digest[ i ] ) {
          return false;
        }
      }

      return false;
    }

    bool MD5::operator==( const MD5 md5 ) const {
      for ( uint8 i = 0; i < 16; ++i ) {
        if ( digest[ i ] != md5.digest[ i ] ) {
          return false;
        }
      }

      return true;
    }

  } /* namespace hash */

}
