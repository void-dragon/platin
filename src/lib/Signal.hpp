/**
 * Signal.hpp
 *
 * @date 29.04.2012
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include <lib/collection/Array.hpp>

namespace lib {

  /**
   * A signal that can be emitted.
   */
  template< class T > class Signal {
    public:
      /**
       * A signal slot.
       */
      class Slot {
        public:
          virtual void call( T t ) = 0;
      };

    protected:
      collection::Array< Slot* > _slots;

    public:

      /**
       * Emitting the signal
       */
      void emit( T t ) {
        for ( uint32 i = 0; i < _slots.size(); i++ ) {
          _slots[ i ]->call( t );
        }
      }

      /**
       * Connects a slot.
       * @param l
       */
      void connect( Slot* slot ) {
        _slots.add( slot );
      }

  };

} /* namespace lib */
#endif /* SIGNAL_HPP_ */
