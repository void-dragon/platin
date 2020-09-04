/**
 * Mutex.hpp
 *
 * @since 09.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef MUTEX_HPP_
#define MUTEX_HPP_

#include <cpp.hpp>

namespace lib {

  namespace sync {

    /**
     * Mutual Exclusion.
     *
     * @since 09.07.2010
     * @author Arne Simon => email::[arne_simon@gmx.de]
     */
    class Mutex {
      protected:
        uint8 _lock;

      public:
        Mutex();

        void enter();

        void leave();

        virtual ~Mutex();
    };

  }

}

#endif /* MUTEX_HPP_ */
