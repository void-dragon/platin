/**
 * CMOS.hpp
 *
 * @since 20.05.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef CMOS_HPP_
#define CMOS_HPP_

#include <cpp.hpp>
#include <lib/Time.hpp>

namespace kernel {

  /**
   *
   */
  namespace CMOS {
      static const uint8 REGISTER = 0x70;
      static const uint8 DATA = 0x71;
      static const uint8 NMI_BIT = 0x80;

      // CMOS register
      static const uint8 REG_RTC_SECONDS = 0x00;
      static const uint8 REG_RTC_MINUTES = 0x02;
      static const uint8 REG_RTC_HOURS = 0x04;
      static const uint8 REG_RTC_WEEKDAY = 0x06;
      static const uint8 REG_RTC_DOM = 0x07;
      static const uint8 REG_RTC_MONTH = 0x08;
      static const uint8 REG_RTC_YEAR = 0x09;
      static const uint8 REG_RTC_CENTURY = 0x32;
      static const uint8 REG_B = 0x0b;
      static const uint8 REG_B_MODE_24H = 0x02;
      static const uint8 REG_B_MODE_BINARY = 0x04;

      /**
       *
       */
      void rtc( lib::Time* time );

      uint8 read( uint8 reg );

      void write( uint8 reg, uint8 data );

  }

}

#endif /* CMOS_HPP_ */
