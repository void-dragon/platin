/**
 * std.hpp
 *
 * @since 23.06.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef STD_HPP_
#define STD_HPP_

#include <cpp.hpp>

namespace lib {

   /**
    * Returns the length of a C-style zero terminated string.
    *
    * @param str The string.
    * @return The length of the string.
    */
   uint32 strlen( const char* str );

   /**
    * Logarithm of base 2.
    * @param x The value.
    * @return The exponent.
    */
   double lg2( double x );

   int32 char2int( char c );

   /**
    *
    * @param
    * @return
    */
   char int2char( uint32 i );

   void int2str( char* Text, uint32 Length, int32 I, int radix = 10 );

   long str2long( char* Text, uint32 Length, int radix = 10 );

   double str2float( char* Text, uint32 Length, int radix = 10 );

   void memset( void* dst, char token, uint32 length );

   void memcpy( void* dst, void* src, uint32 length );

   void cpuid( int32 code, uint32 a[ 4 ] );

   void cpustring( uchar c[ 48 ] );

   inline void get_msr( uint32 msr, uint32 *lo, uint32 *hi ) {
      asm volatile("rdmsr":"=a"(*lo),"=d"(*hi):"c"(msr));
   }

   inline void set_msr( uint32 msr, uint32 lo, uint32 hi ) {
      asm volatile("wrmsr"::"a"(lo),"d"(hi),"c"(msr));
   }

   inline uint8 from_bcd( uint8 bcd ) {
      return ( bcd & 0x0f ) + ( bcd >> 4 ) * 10;
   }

   inline uint8 to_bcd( uint8 n ) {
      return ( ( n / 10 ) << 4 ) + n % 10;
   }

   void outb( uint16 port, uint8 data );

   void outw( uint16 port, uint16 data );

   void out( uint32 port, uint32 data );

   void outs( uint32 port, uint32 pointer, uint32 bytes );

   uint8 inb( uint16 port );

   uint16 inw( uint16 port );

   uint32 in( uint32 port );

   void ins( uint32 port, uint32 pointer, uint32 bytes );

   /**
    * Enable interrupts.
    */
   void sti();

   /**
    * Disable interrupts.
    * @return The previous interrupt state.
    */
   bool cli();

   template< class T > T min( T a, T b ) {
      return a < b ? a : b;
   }

   template< class T > T max( T a, T b ) {
      return a > b ? a : b;
   }

   void* alloc( uint32 size );

   void free( void* ptr );

   void* realloc( void* ptr, uint32 size );

}

#endif /* STD_HPP_ */
