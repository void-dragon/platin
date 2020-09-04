/**
 * Thread.hpp
 *
 * @since 26.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <cpp.hpp>
#include <kernel/Process.hpp>
#include <lib/File.hpp>
#include <lib/String.hpp>
#include <lib/sync/Mutex.hpp>

namespace kernel {

  class System;
  class Process;

  /**
   * A Thread.
   *
   * @todo VM86 is not supported
   *
   * @section Examples
   *
   * @subsection simplekernelcreation Simple Kernel Thread Creation
   * Start a in kernel thread.
   * @code
   * void dummy() {
   *   for ( int i = 0; i < 5; ++i ) {
   *     system->video << "i am a thread\n";
   *   }
   * }
   *
   * kernel::Thread *thread = new kernel::Thread( system, &dummy );
   * @endcode
   *
   * @subsection simpleprocesscreation Simple Processes Thread Creation
   * Start a in kernel thread.
   * @code
   * void dummy() {
   *   for ( int i = 0; i < 5; ++i ) {
   *     system->video << "i am a thread\n";
   *   }
   * }
   *
   * kernel::Process *process = new kernel::Process();
   * kernel::Thread *thread = new kernel::Thread( process, &dummy );
   * @endcode
   *
   * @since 26.07.2010
   * @author Arne Simon => email::[arne_simon@gmx.de]
   */
  class Thread: public lib::File {
      friend class System;

    public:
      typedef void*(*Func)();

      enum Mode {
        READY, ///< The thread is ready for work, but is not executed.
        BLOCKED, ///< The thread is not capable to be executed by the lack of resources.
        RUNNING, ///< The thread is executed.
        START, ///< A new born thread, which is in the setup process.
        DEAD, ///< A dead thread which is finished and will be deconstruct.
      };

      /**
       * The state of the CPU before and after an interrupt.
       *
       * @attention The order of the elements is the order pushed and popped on the stack
       *        by our asm isr wrapper function!
       */
      struct State {
          // manually saved registers.
          uint32 cr3;
          uint32 cr0;

          uint32 DS;
          uint32 ES;
          uint32 FS;
          uint32 GS;
          uint32 SS;

          uint32 EAX;
          uint32 EBX;
          uint32 ECX;
          uint32 EDX;
          uint32 ESI;
          uint32 EDI;
          uint32 EBP;

          // pushed by our assemble isr stubs.

          uint32 irq;

          // saved by the cpu if an interrupt occurs.

          uint32 EIP;
          uint32 CS;
          uint32 EFLAGS;
          uint32 ESP;

          // stack parameters

          uint32 para; ///< A paramter for the thread. In most cases it is a pointer to our Thread-class!
      };

      struct Behavior {
          uint8 duration; ///< The number of continous time slices. min = 1 | max = 254
          uint8 step; ///< The count of current continous time slices. min = 1 | max = 254
          uint8 priority; ///< Tells how important the thread is. min = 0 | max = 254
      };

    public:
      Process* _process; ///< The process of the thread.
      uint32 _id; ///< The id of the thread.
      uint32 func; ///< The function to execute.
      uint8* stack; ///< The threads's stack. The address is the end of the stack, so it's the beginning of the memory area!
      uint32 stack_size; ///< The size of the stack for this thread.
      void* result; ///< The return value of the thread.
      lib::sync::Mutex* mutex; ///< One thread can only acquire on mutex at a time. This is this mutex or null.

      /**
       * Sets up the thread parameters.
       */
      void init();

      Mode mode; ///< The current mode in which the thread is.
      State* state; ///< The cpu state from our thread pushed on the stack.
      Behavior behavior;

      /**
       * Creates a new thread in the system.
       *
       * @param Entry Pointer to a function.
       * @param StackSize The size of the stack for this thread.
       */
      Thread( Process* P, uint32 Entry, uint32 StackSize = 4000 );

      /**
       * This constructor is  for inheritance, where the subclass overrides the execute function.
       *
       * @param StackSize The size of the stack for this thread.
       */
      Thread( Process*P, uint32 StackSize = 4000 );

      /**
       * Returns the id of this thread.
       * @return The id of this thread.
       */
      uint32 id() const;

      Process* process() const;

      void join();

      static void yield();

      /**
       * Sleeps, does nothing, for a given amount of time.
       *
       * @todo Sleep is not implemented.
       *
       * @param mircosec The time a thread sleeps.
       */
      static void sleep( uint32 mircosec );

      /**
       * Kills this thread.
       */
      void kill();

      uint32 size() const {
        return stack_size;
      }

      virtual ~Thread();
  };

}

#endif /* THREAD_HPP_ */
