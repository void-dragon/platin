#include "cpp.hpp"
#include <kernel/System.hpp>
#include <kernel/Thread.hpp>
#include <lib/Exception.hpp>
#include <lib/std.hpp>

extern "C" void __cxa_pure_virtual() {
  lib::Exception::throwing( "panic calling pure virtual function" );
}

namespace __gnu_cxx {

  void __verbose_terminate_handler() {
    //kernel::Thread::current()->kill();
  }

}

//extern "C" uint32 __umoddi3( uint64 xp, uint32 y ) {
//   uint32 rem;
//   uint64 q = __xdiv64_32( xp, y );
//
//   rem = xp - q * y;
//
//   return rem;
//}

extern "C" uint64 __udivdi3( uint64 dividend, uint64 divisor ) {
  uint32 q1 = dividend >> 32;
  uint32 q2 = ( uint32 ) dividend / ( uint32 ) divisor;
  uint32 mod = 0;

  if ( q1 )
    asm volatile(
        "divl %0"
        : "=d"(q1), "=a"(mod) : "m"(divisor), "d"(q1) );

  uint64 result = ( ( uint64 ) q1 << 32 ) + mod + q2;

  return ( result );

}

extern "C" void gcc_assert( bool b ) {
  if ( not b ) {
    asm volatile("hlt;");
  }
}

extern "C" void gcc_unreachable() {
  asm volatile("hlt;");
}

extern "C" int __builtin_strcmp( const char* N, const char* N2 ) {
  uint32 len1 = lib::strlen( N );
  uint32 len2 = lib::strlen( N2 );

  if ( len1 == len2 ) {
    for ( uint32 i = 0; i < len1; ++i ) {
      uint32 c = N[ i ] - N2[ i ];

      if ( c != 0 )
        return c;
    }
  }
  else {
    return len1 - len2;
  }

  return 0;
}

void staticConstructors() {
  for ( constructor* i = &start_ctors; i != &end_ctors; ++i )
    ( *i )();
}

void staticDestructors() {
  for ( constructor* i = &start_ctors; i != &end_ctors; ++i )
    ( *i )();
}

void *operator new( uint32 size ) {
  return system->virtual_memory.alloc( size );
}

void *operator new[]( uint32 size ) {
  return system->virtual_memory.alloc( size );
}

void operator delete( void *obj ) {
  system->virtual_memory.free( obj );

}

void operator delete[]( void *obj ) {
  system->virtual_memory.free( obj );
}

