/**
 * File.cpp
 *
 * @since 11.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */
#include "File.hpp"
#include <lib/std.hpp>

namespace lib {

  uint64 File::size() {
    return 0;
  }

  void* File::range( uint32 start, uint32 size ) {
    return ( void* ) 0;
  }

  void File::range( uint32 start, uint32 size, void* data ) {
  }

  bool File::hasFiles() {
    return false;
  }

  File::Files* File::files() {
    return 0;
  }

  File* File::get( const char* name ) {
    return 0;
  }

  uint32 File::write( void* data, uint32 length ) {
    return 0;
  }

  File& File::operator<<( const char* cp ) {
    write( ( void* ) cp, strlen( cp ) );
    return *this;
  }

  File& File::operator<<( uchar* cp ) {
    write( ( void* ) cp, strlen( ( const char* ) cp ) );
    return *this;
  }

  File& File::operator<<( uint32 i ) {
    write( &i, sizeof(uint32) );
    return *this;
  }

  File& File::operator<<( int32 i ) {
    write( &i, sizeof(int32) );
    return *this;
  }

  File& File::operator<<( double i ) {
    write( &i, sizeof(double) );
    return *this;
  }

  void File::read( void** data, uint32* length ) {
  }

  uint32 File::read( void* data, uint32 length ) {
    return 0;
  }

  File& File::operator>>( const char* &cp ) {
    read( ( void* ) cp, size() );
    return *this;
  }

  File& File::operator>>( uchar* &cp ) {
    read( ( void* ) cp, size() );
    return *this;
  }

  File& File::operator>>( uint32 &i ) {
    read( &i, sizeof(uint32) );
    return *this;
  }

  File& File::operator>>( int32 &i ) {
    read( &i, sizeof(int32) );
    return *this;
  }

  File& File::operator>>( double &i ) {
    read( &i, sizeof(double) );
    return *this;
  }

  File::~File() {

  }

}
