/**
 * Time.cpp
 *
 * @since 27.05.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Time.hpp"
#include <kernel/CMOS.hpp>

namespace lib {

   Time::Time() {
      kernel::CMOS::rtc( this ); // read real time clock!

      timestamp = seconds;
      timestamp += minutes * SECONDS_A_MIN;
      timestamp += hours * SECONDS_A_HOUR;
      timestamp += day_of_month * SECONDS_A_DAY;

      timestamp += ( ( century * 10 + year ) - 1970 ) * SECONDS_A_YEAR;
   }

   Time::Time( PosixTimeStamp Stamp ) {
      timestamp = Stamp;

      year = timestamp / SECONDS_A_YEAR;
   }

   Time::~Time() {
   }

}
