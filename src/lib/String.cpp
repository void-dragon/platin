/**
 * String.cpp
 *
 * @since 10.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "String.hpp"
#include <lib/std.hpp>

namespace lib {

  void String::grow( uint32 n ) {
    if ( n > _size ) {

      if ( _data ) {

        while ( n > _size )
          _size = ( _size * 1.5 ) + 1;

        _data = ( char* ) lib::realloc( _data, _size );
      }
      else {
        _data = ( char* ) lib::alloc( n );
        _size = n;
      }

    }
  }

  String::String()
      : _deleteOnDestroy( true ), _length( 0 ), _size( 0 ), _data( 0 ) {

  }

  String::String( const char* Data )
      : _deleteOnDestroy( false ), _length( 0 ), _size( 0 ), _data( ( char* ) Data ) {
    _size = _length = lib::strlen( Data ) - 1;
    //_data = ( char* ) lib::alloc( _size );
    //memcpy( _data, Data, _size );
  }

  String::String( char* Data )
      : _deleteOnDestroy( false ), _length( 0 ), _size( 0 ), _data( Data ) {
    _size = _length = lib::strlen( Data );
    //_data = ( char* ) lib::alloc( _size );
    //memcpy( _data, Data, _size );
  }

  String::String( char* Data, uint32 Size )
      : _deleteOnDestroy( true ), _length( Size ), _size( Size ), _data( 0 ) {
    _data = ( char* ) lib::alloc( _size );
    memcpy( _data, Data, _size );
  }

  void String::operator+=( const char* str ) {
    uint32 len = strlen( str );

    grow( _length + len );

    memcpy( _data + _length, ( void* ) str, len );

    _length += len;
  }

  void String::operator+=( char v ) {
    grow( _length + 1 );

    _data[ _length ] = v;

    _length++;
  }

  void String::operator+=( String* a ) {
    grow( _length + a->_length );

    memcpy( _data + _length, a->_data, a->_length );

    _length += a->_length;
  }

  bool String::operator==( String* a ) {
    return compare( a ) == 0;
  }

  bool String::operator==( const char* str ) {
    return compare( ( char* ) str, lib::strlen( str ) ) == 0;
  }

  hash::Tiger String::hash() {
    hash::Tiger res;

    lib::hash::tiger( ( uint64* ) _data, _length, res.data );

    return res;
  }

  void String::add( char v ) {
    grow( _length + 1 );

    _data[ _length ] = v;

    _length++;
  }

  void String::add( char* a, uint32 len ) {
    grow( _length + len );

    memcpy( _data + _length, a, len );

    _length += len;
  }

  /**
   * Adds an array of new elements.
   *
   * @param a Array of elements which will be added.
   */
  void String::add( String* a ) {
    grow( _length + a->_length );

    memcpy( _data + _length, a->_data, a->_length );

    _length += a->_length;
  }

  int32 String::compare( char* data, uint32 len ) {
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

  int32 String::compare( String* A ) {
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

  void String::set( char* Data, uint32 Len ) {
    if ( _data )
      lib::free( _data );

    _size = _length = Len;

    _data = ( char* ) lib::alloc( _size );

    lib::memcpy( _data, Data, _size );
  }

  String::~String() {
    if ( _data and _deleteOnDestroy )
      lib::free( _data );
  }

}
