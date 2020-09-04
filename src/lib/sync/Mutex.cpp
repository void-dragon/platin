/**
 * Mutex.cpp
 *
 * @since 09.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Mutex.hpp"
#include <kernel/Thread.hpp>
#include <lib/Exception.hpp>

namespace lib {

  namespace sync {

    Mutex::Mutex() :
      _lock( 0 ) {

    }

    void Mutex::enter() {
      uint8 l = 1;

      //kernel::Thread* t = kernel::Thread::current();

//      if ( t->mutex == this ) {
//        Exception::throwing( "mutex is already acquired by this thread!" );
//      }
//
//      if ( t->mutex ) {
//        Exception::throwing( "the thread has already another mutex acquired!" );
//      }
//
//      t->mutex = this;

      // set the lock to one, and retrive previous lock state
      asm volatile ("xchg %b1, %b0" : "=a"(_lock), "=b"(l): "b"(l), "a"(_lock) );

      // while locked
      while ( l ) {
        kernel::Thread::yield();

        asm volatile ("xchg %b1, %b0" : "=a"(_lock), "=b"(l): "b"(l), "a"(_lock) );
        // get lock state
      }
    }

    void Mutex::leave() {
//      kernel::Thread* t = kernel::Thread::current();
//
//      if ( t->mutex ) {
//        if ( t->mutex == this ) {
//          t->mutex = 0;
//        }
//        else {
//          Exception::throwing( "the thread tries to free a mutex he has not acquired!" );
//        }
//      }
//      else {
//        Exception::throwing( "the thread has no mutex acquired but tries to free one!" );
//      }

      asm volatile ("xchg %b1, %b0" : "=a"(_lock) : "b"(0), "a"(_lock) );
    }

    Mutex::~Mutex() {
    }

  }

}
