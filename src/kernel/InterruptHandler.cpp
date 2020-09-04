/**
 * InterruptHandler.cpp
 *
 * @since 12.08.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "InterruptHandler.hpp"
#include <kernel/System.hpp>

namespace kernel {

  InterruptHandler::InterruptHandler()
      : Thread( system ) {
    behavior.priority = 20;
  }

  void* InterruptHandler::execute() {
    System::ISRList *hlist;
    System::ISRList::Iterator* iter;
    uint8 irq;

    while ( true ) {
      while ( system->interrupts.size() ) { // got we any interrupts ?
        irq = system->interrupts.pop();

        hlist = &system->isr[ irq ]; // get array of interrupt handlers

        for ( iter = hlist->iterator(); *iter; ++( *iter ) ) { // call each interrupt handler
          ( **iter )->call();
        }
        delete iter;

      }
      yield(); // we do not need the cpu anymore
    }

    return 0;
  }

}
