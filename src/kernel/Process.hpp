/**
 * Process.hpp
 *
 * @since 08.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include <kernel/Thread.hpp>
#include <kernel/VirtualMemory.hpp>
#include <lib/File.hpp>
#include <lib/String.hpp>
#include <lib/collection/List.hpp>

namespace kernel {

  class Thread;
  class User;

  /**
   * A Process in the Platin kernel.
   *
   * @since 08.02.2011
   * @date 24.07.2011
   * @author Arne Simon => email::[arne_simon@gmx.de]
   */
  class Process: public lib::File {
    public:
      typedef lib::collection::List< Thread* > Threads;

      enum State {
        Active, Dead
      };

      uint32 _id;
      VirtualMemory virtual_memory;
      Threads threads;
      User* user; ///< The user which owns this process.
      State state; ///< The state in which the process is.

      /**
       * Creates a new process.
       *
       * @note The constructor is still executed in system space.
       */
      Process();

      virtual ~Process();
  };

}

#endif /* PROCESS_HPP_ */
