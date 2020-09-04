/**
 * Semaphor.hpp
 *
 * @since 08.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef SEMAPHOR_HPP_
#define SEMAPHOR_HPP_

#include <cpp.hpp>

namespace lib {

  namespace sync {

    /**
     *
     */
    class Semaphor {
      private:
        uint8 in;
        uint32 comperator;

      public:
        Semaphor( uint32 Max );

        void enter();

        void leave();

        virtual ~Semaphor();
    };

  }

}

#endif /* SEMAPHOR_HPP_ */
