/**
 * PIT.hpp
 *
 * @date 06.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef PIT_HPP_
#define PIT_HPP_

#include <cpp.hpp>

namespace kernel {

  /**
   * Programmable Interval Timer.
   *
   * @note Moste commun setup for the PIT is as a rate generator, used for
   *    scheduling.
   *    init( 0x40, 0x04 );
   *    load( 0x40, counter );
   *
   * @note http://lowlevel.brainsware.org/wiki/index.php/PIT
   */
  class PIT {
      uint8 channel;

    public:
      const static uint8 Channel0 = 0x40; ///< Counter Channel 0. Bound to the PIC and fires IRQ0.
      const static uint8 Channel1 = 0x41; ///< Counter Channel 1. Used in early days for memory sync.
      const static uint8 Channel2 = 0x42; ///< Counter Channel 2. Linked to the system-speaker.
      const static uint8 Init = 0x43; ///< Init

      const static uint8 Mod_TerminalCount = 0; ///< Interrupt on terminal count.
      const static uint8 Mod_Retrigger = 0x02; ///< Hardware Retriggerable One-Shot.
      const static uint8 Mod_RateGenerator = 0x04; ///< Rate Generator. counter = interne freq / freq
      const static uint8 Mod_SqrWaveGenerator = 0x06; ///< Square Wave Generator.
      const static uint8 Mod_SoftwareStrobe = 0x08; ///<  Software Triggered Strobe.
      const static uint8 Mod_HardwareStrobe = 0x0a; ///< Hardware Triggered Strobe.

      PIT( uint8 C );

      /**
       * Sets a modus for a timer channel.
       *
       * @param mode
       */
      void init( uint8 mode );

      /**
       * Loads the counter of a timer channel.
       * @param counter
       */
      void load( uint16 counter );

  };

}

#endif /* PIT_HPP_ */
