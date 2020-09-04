/**
 * PIT.cpp
 *
 * @date 06.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "PIT.hpp"
#include <lib/std.hpp>

namespace kernel {

  PIT::PIT( uint8 C ) :
    channel( C ) {

  }

  void PIT::init( uint8 mode ) {
    // 0x30 = write least significant byte, then most significant byte !
    lib::outb( Init, ( ( channel & 0xFC ) << 6 ) | mode | 0x30 );
  }

  void PIT::load( uint16 counter ) {
    lib::outb( channel, counter ); // write LSB.
    lib::outb( channel, counter >> 8 ); // write MSB.
  }

}
