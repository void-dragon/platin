/**
 * Time.hpp
 *
 * @since 27.05.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef TIME_HPP_
#define TIME_HPP_

#include <cpp.hpp>

namespace lib {

   /**
    * @attention weekday => Sunday = 1 -> Saturday = 7
    *
    * @since 27.05.2010
    * @date 23.10.2011
    * @author Arne Simon => email::[arne_simon@gmx.de]
    */
   class Time {
      public:
         typedef uint64 PosixTimeStamp;
         static const uint32 SECONDS_A_YEAR = 31104000;
         static const uint32 SECONDS_A_DAY = 86400;
         static const uint32 SECONDS_A_HOUR = 3600;
         static const uint32 SECONDS_A_MIN = 60;

         PosixTimeStamp timestamp;
         uint8 seconds;
         uint8 minutes;
         uint8 hours;
         uint8 weekday;
         uint8 day_of_month;
         uint8 month;
         uint8 year;
         uint8 century;

         /**
          * Reads the cmos real time clock.
          */
         Time();

         Time( PosixTimeStamp Stamp );

         virtual ~Time();
   };

}

#endif /* TIME_HPP_ */
