/**
 * ISR.hpp
 *
 * @since 07.10.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef ISR_HPP_
#define ISR_HPP_

namespace kernel {

  /**
   * An interface for a interrupt service routine.
   */
  class ISR {
    public:
      virtual bool call() = 0;
  };

}

#endif /* ISR_HPP_ */
