/**
 * Stack.hpp
 *
 * @since 26.09.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_COLLECTION_STACK_HPP_
#define LIB_COLLECTION_STACK_HPP_

#include <Cpp.hpp>
#include <lib/std.hpp>
#include <lib/Exception.hpp>

namespace lib {

   namespace collection {

      template< class T > class Stack {
         protected:
            T* _data;
            uint32 _pointer;
            uint32 _size;

            void grow() {
               _size <<= 1;
               _data = ( T* ) lib::realloc( _data, _size );
            }

         public:
            Stack( uint32 Reserve = 5 )
                  : _pointer( 0 ), _size( Reserve ) {

               _data = ( T* ) lib::alloc( sizeof(T) * _size );
            }

            void push( const T& t ) {
               if ( _pointer > _size ) {
                  grow();
               }

               _data[ _pointer++ ] = t;
            }

            void pop() {
               if ( _pointer == 0 )
                  lib::Exception::throwing( "pop on empyt stack?" );

               _pointer--;
            }

            T& get( uint32 i ) {
               if ( i > _pointer )
                  lib::Exception::throwing( "stack: index out of bound" );

               return _data[ _pointer - i - 1 ];
            }

            T& top() {
               if ( _pointer == 0 )
                  lib::Exception::throwing( "top on empty stack?" );

               return _data[ _pointer - 1 ];
            }

            uint32 size() const {
               return _pointer;
            }

            ~Stack() {
               lib::free( _data );
            }
      };

   }

}

#endif /* LIB_COLLECTION_STACK_HPP_ */
