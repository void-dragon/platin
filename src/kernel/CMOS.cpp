/**
 * CMOS.cpp
 *
 * @since 20.05.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "CMOS.hpp"
#include <lib/std.hpp>

namespace kernel {

  namespace CMOS {

    void rtc( lib::Time* time ) {
      uint8 reg = read( REG_B );

      time->seconds = read( REG_RTC_SECONDS );
      time->minutes = read( REG_RTC_MINUTES );
      time->hours = read( REG_RTC_HOURS );
      time->weekday = read( REG_RTC_WEEKDAY );
      time->day_of_month = read( REG_RTC_DOM );
      time->month = read( REG_RTC_MONTH );
      time->year = read( REG_RTC_YEAR );
      time->century = read( REG_RTC_CENTURY );

      if ( ( reg & REG_B_MODE_BINARY ) == 0 ) {
        time->seconds = lib::from_bcd( time->seconds );
        time->minutes = lib::from_bcd( time->minutes );
        if ( ( reg & REG_B_MODE_24H ) == 0 ) {
          time->hours &= ~0x80;
        }
        time->hours = lib::from_bcd( time->hours );
        time->weekday = lib::from_bcd( time->weekday );
        time->day_of_month = lib::from_bcd( time->day_of_month );
        time->month = lib::from_bcd( time->month );
        time->year = lib::from_bcd( time->year );
        time->century = lib::from_bcd( time->century );
      }

    }

    uint8 read( uint8 reg ) {
      uint8 erg;

      lib::cli();
      lib::outb( REGISTER, reg | NMI_BIT ); // select register
      erg = lib::inb( DATA );
      lib::sti();

      return erg;
    }

    void write( uint8 reg, uint8 data ) {
      lib::cli();
      lib::outb( REGISTER, reg | NMI_BIT ); // select register
      lib::outb( DATA, data );
      lib::sti();
    }

  }

}
