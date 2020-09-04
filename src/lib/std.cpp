/**
 * std.cpp
 *
 * @since 23.06.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */
#include "std.hpp"
#include <kernel/System.hpp>

namespace lib {

  void cpuid( int32 code, uint32 a[ 4 ] ) {
    asm volatile("cpuid":"=a"(a[0]),"=b"(a[1]),"=c"(a[2]),"=d"(a[3]):"0"(code):);
  }

  void cpustring( uchar c[ 48 ] ) {
    uint32 d[ 12 ];

    asm volatile("cpuid":"=a"(d[0]),"=b"(d[1]),"=c"(d[2]),"=d"(d[3]):"0"(0x80000002):);
    asm volatile("cpuid":"=a"(d[4]),"=b"(d[5]),"=c"(d[6]),"=d"(d[7]):"0"(0x80000003):);
    asm volatile("cpuid":"=a"(d[8]),"=b"(d[9]),"=c"(d[10]),"=d"(d[11]):"0"(0x80000004):);

    for ( uint8 i = 0, j = 0; i < 12; i++ ) {
      c[ j++ ] = d[ i ];
      c[ j++ ] = d[ i ] >> 8;
      c[ j++ ] = d[ i ] >> 16;
      c[ j++ ] = d[ i ] >> 24;
    }
  }

  double lg2( double x ) {
    register double result;
    asm volatile("fld1; fxch; fyl2x" : "=t" (result) : "0" (x));
    return result;
  }

  uint32 strlen( const char* str ) {
    uint32 i = 0;

    while ( ( str[ i++ ] ) != '\0' )
      ;

    return i;
  }

  char int2char( uint32 i ) {
    if ( i < 10 )
      return i + '0';
    else
      return 'A' + i - 10;

  }

  int32 char2int( char c ) {
    if ( c >= 65 && c <= 70 ) {
      return c - 55;
    }
    else if ( c >= 97 && c <= 102 ) {
      return c - 87;
    }
    else
      return c - 48;

  }

  void int2str( char* Text, uint32 Length, int32 I, int radix ) {
    char buff[ 10 ];
    uint32 idx = 0, i = 0;

    do {
      buff[ idx++ ] = int2char( I % radix );
      I /= radix;
    } while ( I > 0 );

    do {
      Text[ i++ ] = buff[ --idx ];
    } while ( idx > 0 && i < Length );
  }

  long str2long( char* Text, uint32 Length, int radix ) {
    unsigned long buf;
    unsigned long power = 1;
    long lng = 0;

    if ( Text[ 0 ] == '-' ) {
      Length--;
      Text++;
      power = -1;
    }

    while ( Length ) {
      --Length;
      if ( Text[ Length ] != '_' ) {
        buf = char2int( Text[ Length ] ) * power;
        power *= radix;
        lng += buf;
      }
    }

    return lng;
  }

  double str2float( char* Text, uint32 Length, int radix ) {
    double buf;
    double power = 0.1;
    uint32 dot;
    double dbl = 0;

    for ( dot = 0; dot < Length && Text[ dot ] != '.'; dot++, power *= radix )
      ;

    for ( uint32 i = 0; i < Length; i++ ) {
      if ( Text[ i ] != '_' && Text[ i ] != '.' ) {
        buf = char2int( Text[ i ] ) * power;
        power /= radix;
        dbl += buf;
      }
    }

    return dbl;
  }

  void memset( void* dst, char token, uint32 length ) {
    char* d = ( char* ) dst;

    for ( uint32 i = 0; i < length; ++i ) {
      d[ i ] = token;
    }
  }

  void memcpy( void* dst, void* src, uint32 length ) {
    uint8 *cdst = ( uint8* ) dst, *csrc = ( uint8* ) src;

    for ( uint32 i = length; i; ) {
      --i;
      cdst[ i ] = csrc[ i ];
    }
  }

  void outb( uint16 port, uint8 data ) {
    asm volatile ("outb %b1, %w0" : : "d" (port), "a" (data));
  }

  void outw( uint16 port, uint16 data ) {
    asm volatile ("outw %w1, %w0" : : "d" (port), "a" (data));
  }

  void out( uint32 port, uint32 data ) {
    asm volatile ("outl %1, %w0" : : "d" (port), "a" (data));
  }

  void outs( uint32 port, uint32 pointer, uint32 bytes ) {
    asm volatile("rep outsw;": :"c"(bytes), "d"(port), "S"(pointer) );
  }

  uint8 inb( uint16 port ) {
    char data = 0;
    asm volatile ("inb %w1, %b0" : "=a" (data) : "d" (port));
    return data;
  }

  uint16 inw( uint16 port ) {
    uint16 data = 0;
    asm volatile("inw %w1, %w0" : "=a" (data) : "d" (port) );
    return data;
  }

  uint32 in( uint32 port ) {
    uint32 data = 0;
    asm volatile ("in %w1, %0" : "=a" (data) : "d" (port));
    return data;
  }

  void ins( uint32 port, uint32 pointer, uint32 bytes ) {
    //asm volatile( "rep insb;" : : "c"(bytes), "d"(port), "D"(pointer) );
    asm volatile (
        "push %%edi;"
        "mov %0, %%edx;"
        "mov %1, %%edi;"
        "mov %2, %%ecx;"
        "cld;"
        "lib_ins_loop : insl;"
        "loop lib_ins_loop;"
        "pop %%edi;"
        : : "d" (port), "a"( pointer ), "b" ( bytes / 4 )
    );
  }

  void sti() {
    asm volatile("sti;");
  }

  bool cli() {
    uint32 ret;
    asm volatile(
        "pushfl;"
        "popl %0;"
        "cli;"
        : "=a"(ret) :
    );
    return ret & ( 1 << 9 ) ? true : false;
  }

  void* alloc( uint32 size ) {
    return system->virtual_memory.alloc( size );
  }

  void free( void* ptr ) {
    system->virtual_memory.free( ptr );
  }

  void* realloc( void* ptr, uint32 size ) {
    return system->virtual_memory.realloc( ptr, size );
  }
}
