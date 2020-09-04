/**
 * In.hpp
 *
 * @since 24.06.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_STREAM_IN_HPP_
#define LIB_STREAM_IN_HPP_

#include <cpp.hpp>

namespace lib {

  namespace stream {

    /**
     * An abstract input stream.
     *
     * @since 24.06.2010
     * @author Arne Simon => email::[arne_simon@gmx.de]
     */
    class In {
      public:
        /**
         * Reads all data currently aviable in the stream.
         *
         * @note This function allocates a new byte array and returns the size.
         *
         * @param data The pointer to the new data array.
         * @param length The length of the array.
         */
        virtual void read( void** data, uint32* length ) = 0;

        /**
         * Reads length bytes into data.
         *
         * @param data A data array.
         * @param length The length of the data array, and the number of bytes to read.
         * @return The number of bytes actually read.
         */
        virtual uint32 read( void* data, uint32 length ) = 0;

        virtual In& operator>>( const char* &cp ) = 0;

        virtual In& operator>>( uchar* &cp ) = 0;

        virtual In& operator>>( uint32 &i ) = 0;

        virtual In& operator>>( int32 &i ) = 0;

        virtual In& operator>>( double &i ) = 0;

        virtual ~In() {
        }
    };

  }

}

#endif /* LIB_STREAM_IN_HPP_ */
