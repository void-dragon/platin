/**
 * Out.hpp
 *
 * @since 24.06.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_STREAM_OUT_HPP_
#define LIB_STREAM_OUT_HPP_

#include <cpp.hpp>

namespace lib {

  namespace stream {

    /**
     * An interface for an output stream.
     *
     * @since 24.06.2010
     *
     * @author Arne Simon => email::[arne_simon@gmx.de]
     */
    class Out {
      public:

        /**
         * Writes data in the stream.
         *
         * @param data Data to be written.
         * @param length The length of the data.
         * @return Bytes written.
         */
        virtual uint32 write( void* data, uint32 length ) = 0;

        virtual Out& operator<<( const char* cp ) = 0;

        virtual Out& operator<<( uchar* cp ) = 0;

        virtual Out& operator<<( uint32 i ) = 0;

        virtual Out& operator<<( int32 i ) = 0;

        virtual Out& operator<<( double i ) = 0;

        virtual ~Out() {
        }
    };

  }

}

#endif /* LIB_STREAM_OUT_HPP_ */
