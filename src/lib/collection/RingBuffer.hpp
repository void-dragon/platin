/**
 * RingBuffer.hpp
 *
 * @since 09.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef RINGBUFFER_HPP_
#define RINGBUFFER_HPP_

#include <cpp.hpp>
#include <lib/collection/Iterator.hpp>
#include <lib/Exception.hpp>

namespace lib {

  namespace collection {

    /**
     *
     */
    template< class T, uint32 SIZE > class RingBuffer {
      protected:
        T _buffer[ SIZE ];
        uint32 _put;
        uint32 _get;
        uint32 _size;

      public:

        class Iterator: public collection::Iterator< T > {
            friend class RingBuffer;
          private:
            RingBuffer* _ring;
            uint32 _pos;

            Iterator( RingBuffer* R ) :
              _ring( R ), _pos( 0 ) {

            }

          public:

            T& operator*() {
              return _ring->_buffer[ ( _pos + _ring->_get ) % SIZE ];
            }

            void operator++() {
              if ( _pos < _ring->_size )
                _pos++;
            }

            operator bool() {
              return _pos < _ring->_size;
            }
        };

        RingBuffer() :
          _put( 0 ), _get( 0 ) {
        }

        Iterator* iterator() {
          return new Iterator( this );
        }

        void push( const T& t ) {
          if ( _size < SIZE ) {
            _buffer[ _put ] = t;

            if ( ++_put >= SIZE ) {
              _put = 0;
            }

            _size++;
          }
          else {
            Exception::throwing( "RingBuffer: full!" );
          }
        }

        T peek() {
          return _buffer[ _get ];
        }

        T pop() {
          if ( _size ) {
            T& t = _buffer[ _get ];

            if ( ++_get >= SIZE ) {
              _get = 0;
            }

            _size--;

            return t;
          }
          else {
            Exception::throwing( "RingBuffer: popping empyt buffer!" );
          }

          return 0;
        }

        uint32 size() const {
          return _size;
        }

        uint32 capacity() const {
          return SIZE;
        }

    };

  }

}

#endif /* RINGBUFFER_HPP_ */
