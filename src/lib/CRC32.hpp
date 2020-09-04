/**
 * CRC32.hpp
 *
 * @since 21.05.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef CRC32_HPP_
#define CRC32_HPP_

#include <cpp.hpp>

namespace lib {

  /**
   * @todo not fully implemented!
   *
   * @since 21.05.2010
   * @author Arne Simon => email::[arne_simon@gmx.de]
   */
  class CRC32 {
    private:
      /**
       * This is the CRC-32 table
       * Generated with:
       * width = 32 bits
       * poly = 0x1EDC6F41
       * reflect input bytes = true
       * reflect output bytes = true
       */
      static const uint32 table[ 256 ];

    public:

      /**
       * Steps through buffer one byte at at time, calculates reflected
       * crc using table.
       */
      static uint32 hash( uint32 crc, const uint8 *data, uint32 length );

      CRC32();

      virtual ~CRC32();
  };

}

#endif /* CRC32_HPP_ */
