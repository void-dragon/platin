/**
 * Array.hpp
 *
 * @since 24.06.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_COLLECTION_ARRAY_HPP_
#define LIB_COLLECTION_ARRAY_HPP_

#include <cpp.hpp>
#include <lib/std.hpp>
#include "Iterator.hpp"
#include "Collection.hpp"

namespace lib {

   namespace collection {

      /**
       * A wrapper around an array.
       *
       * @since 24.06.2010
       * @author Arne Simon => email::[arne_simon@gmx.de]
       */
      template< class V > class Array: public Collection< V > {
         protected:
            uint32 _length;
            uint32 _size;
            V* _data;

            virtual void grow( uint32 n ) {
               if ( n > _size ) {

                  if ( _data ) {

                     while ( n > _size )
                        _size = ( _size * 1.5 ) + 1;

                     _data = ( V* ) lib::realloc( _data, _size * sizeof(V) );
                  }
                  else {
                     _data = ( V* ) lib::alloc( n * sizeof(V) );
                     _size = n;
                  }

               }
            }

         public:

            class Iterator: public collection::Iterator< V > {
                  friend class Array;
               protected:
                  Array* a;
                  uint32 i;

                  Iterator( Array* A ) :
                     a( A ), i( 0 ) {

                  }

               public:

                  V& operator*() {
                     return a->_data[ i ];
                  }

                  void operator++() {
                     i++;
                  }

                  operator bool() {
                     return i < a->_length ? true : false;
                  }

            };

            /**
             * Constructs a new array.
             *
             * @param Reserve The initial capacity of this array.
             */
            Array( uint32 Reserve ) :
               _length( 0 ), _size( Reserve ) {
               _data = ( V* ) lib::alloc( _size * sizeof(V) );
            }

            Array() :
               _length( 0 ), _size( 0 ), _data( 0 ) {

            }

            /**
             * Wrappes an existing array.
             *
             * @param Data The array it self.
             * @param Size The number of elements.
             */
            Array( V* Data, uint32 Size ) :
               _length( Size ), _size( Size ), _data( Data ) {
               _data = ( V* ) lib::alloc( _size * sizeof(V) );
               memcpy( _data, Data, _size * sizeof(V) );
            }

            Iterator* iterator() {
               return new Iterator( this );
            }

            V& operator[]( uint32 idx ) {
               return _data[ idx ];
            }

            V& get( uint32 idx ) {
               return _data[ idx ];
            }

            /**
             * Adds a new element.
             *
             * @param v The new element.
             */
            void operator+=( V v ) {
               grow( _length + 1 );

               _data[ _length ] = v;

               _length++;
            }

            /**
             * Adds an array of new elements.
             *
             * @param a Array of elements which will be added.
             */
            void operator+=( Array< V >* a ) {
               grow( _length + a->_length );

               memcpy( _data + _length, a->_data, a->_length * sizeof(V) );

               _length += a->_length;
            }

            /**
             * Adds a new element.
             *
             * @param v The new element.
             */
            void add( V v ) {
               grow( _length + 1 );

               _data[ _length ] = v;

               _length++;
            }

            void add( V* a, uint32 len ) {
               grow( _length + len );

               memcpy( _data + _length, a, len * sizeof(V) );

               _length += len;
            }

            /**
             * Adds an array of new elements.
             *
             * @param a Array of elements which will be added.
             */
            void add( Array< V >* a ) {
               grow( _length + a->_length );

               memcpy( _data + _length, a->_data, a->_length * sizeof(V) );

               _length += a->_length;
            }

            int32 compare( V* data, uint32 len ) {
               if ( _length > len ) {
                  return 1;
               }
               else if ( _length < len ) {
                  return -1;
               }

               for ( uint32 i = 0; i < _length; ++i ) {
                  if ( _data[ i ] > data[ i ] ) {
                     return 1;
                  }
                  else if ( _data[ i ] < data[ i ] ) {
                     return -1;
                  }
               }

               return 0;
            }

            int32 compare( Array< V >* A ) {
               if ( A == this ) {
                  return 0;
               }

               if ( _length > A->_length ) {
                  return 1;
               }
               else if ( _length < A->_length ) {
                  return -1;
               }

               for ( uint32 i = 0; i < _length; ++i ) {
                  if ( _data[ i ] > A->_data[ i ] ) {
                     return 1;
                  }
                  else if ( _data[ i ] < A->_data[ i ] ) {
                     return -1;
                  }
               }

               return 0;
            }

            /**
             * The number of fields the array has allocated in the system.
             */
            uint32 capacity() const {
               return _size;
            }

            /**
             * The length of the actual data.
             */
            uint32 size() {
               return _length;
            }

            /**
             * Sets a new size for this array.
             *
             * @param Len
             */
            void resize( uint32 Len ) {
               if ( _size < Len ) {
                  grow( Len );
               }
               else {
                  for ( uint32 i = Len; i < _length; ++i ) {
                     _data[ i ] = 0;
                  }

                  _length = Len;
               }
            }

            bool hasElement( V &el ) {
               for ( uint32 i = 0; i < _length; ++i ) {
                  if ( _data[ i ] == el ) {
                     return true;
                  }
               }

               return false;
            }

            uint32 chunkSize() {
               return sizeof(V);
            }

            /**
             * Returns a new array which references to a part of the original array.
             *
             * @param start The index of the first element of the new array.
             * @param end The index of the last element of the new array.
             * @return A new reference to the array data.
             */
            Array< V >* sub( uint32 start, uint32 end ) {
               return new Array< V > ( _data + start, end );
            }

            /**
             * Returns a new array.
             *
             * @param start The index of the first element of the new array.
             * @param end The index of the last element of the new array.
             * @return
             */
            Array< V >* subcopy( uint32 start, uint32 end ) {
               Array< V >* a = new Array< V > ( end - start );

               memcpy( a->_data, _data + start, ( end - start ) * sizeof(V) );

               return a;
            }

            V first() {
               return _data[ 0 ];
            }

            V last() {
               return _data[ _length - 1 ];
            }

            V* ptr() const {
               return _data;
            }

            virtual ~Array() {
               lib::free( _data );
            }
      };

   }

}

#endif /* LIB_COLLECTION_ARRAY_HPP_ */
