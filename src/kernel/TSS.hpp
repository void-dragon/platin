/**
 * TSS.hpp
 *
 * @date 07.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef TSS_HPP_
#define TSS_HPP_

#include <cpp.hpp>

namespace kernel {
  /**
   * Task State Segment.
   *
   * @note http://wiki.osdev.org/TSS
   */
  struct TSS {
      uint32 LINK; ///< 31-16 : reserved;
      uint32 ESP0;
      uint32 SS0; ///< 31-16 : reserved;
      uint32 ESP1;
      uint32 SS1; ///< 31-16 : reserved;
      uint32 ESP2;
      uint32 SS2; ///< 31-16 : reserved;
      uint32 CR3;
      uint32 EIP;
      uint32 EFLAGS;
      uint32 EAX;
      uint32 ECX;
      uint32 EDX;
      uint32 EBX;
      uint32 ESP;
      uint32 EBP;
      uint32 ESI;
      uint32 EDI;
      uint32 ES; ///< 31-16 : reserved;
      uint32 CS; ///< 31-16 : reserved;
      uint32 SS; ///< 31-16 : reserved;
      uint32 DS; ///< 31-16 : reserved;
      uint32 FS; ///< 31-16 : reserved;
      uint32 GS; ///< 31-16 : reserved;
      uint32 LDTR; ///< 31-16 : reserved;
      uint32 IOPB;///< 15-0 : reserved;
  }__attribute__((__packed__));
}

#endif /* TSS_HPP_ */
