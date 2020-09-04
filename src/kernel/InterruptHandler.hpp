/**
 * InterruptHandler.hpp
 *
 * @since 12.08.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef INTERRUPTHANDLER_HPP_
#define INTERRUPTHANDLER_HPP_

#include <kernel/Thread.hpp>

namespace kernel {

  /**
   * Handles the fetched interrupts.
   */
  class InterruptHandler: public Thread {
    public:
      InterruptHandler();

      void* execute();
  };

}

#endif /* INTERRUPTHANDLER_HPP_ */
