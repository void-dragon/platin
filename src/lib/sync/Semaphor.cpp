/**
 * Semaphor.cpp
 *
 * @since 08.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Semaphor.hpp"
#include <kernel/Video.hpp>

namespace lib {

  namespace sync {

    Semaphor::Semaphor( uint32 Max ) :
      in( 0 ), comperator( Max ) {
    }

    void Semaphor::enter() {

      // increment counter and store old counter in i
      //asm volatile( "lock inc %0;" : "=a"(in) : "a"(in) );

      while ( in > comperator ) {
        // *debug_msgs << id << ":" << ( **core->current )->id << ":" << i << ":" << counter << "|";
        // invoke schedular!
        asm volatile( "int $32;" );
        //asm volatile( "hlt;" );
      }
    }

    void Semaphor::leave() {
      //  *debug_msgs << id << ":" << ( **core->current )->id << ":" <<  counter << "|";
      //asm volatile( "lock dec %0;" : "=a"(in) : "a"(in) );
    }

    Semaphor::~Semaphor() {
    }

  }

}
