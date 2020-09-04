/**
 * Exception.cpp
 *
 * @since 11.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Exception.hpp"
#include <kernel/System.hpp>

namespace lib {

  Exception Exception::buffer;

  void Exception::throwing( const char* M ) {
    //    Exception* e = &buffer;
    //    e->msg = M;
    //
    //    asm volatile(
    //        "pushl %0;"
    //        "int $0x30;"
    //        : : "a"(e)
    //    );

    system->video.color( kernel::Video::LightRed );
    system->video << "<EXCEPTION:" << M << ">";
    while ( true ) {
      asm volatile("hlt;");
    }
  }

  Exception::Exception() :
    msg( 0 ) {
  }

  Exception::Exception( const char* M ) :
    msg( M ) {
  }

  Exception::~Exception() {
  }

}
