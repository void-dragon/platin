/**
 * CPU.hpp
 *
 * @date 06.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef CPU_HPP_
#define CPU_HPP_

#include <kernel/Thread.hpp>
#include <lib/std.hpp>
#include <lib/stream/Out.hpp>

namespace kernel {

  class CPU {
    public:
      struct Core {
          Thread* current; ///< The thread currently working.
      };

      class LocalAPIC {

      };

      class Info {
          enum Flags {
            FPU = 1,
            VME = 1 << 1,
            DE = 1 << 2,
            PSE = 1 << 3,
            TSC = 1 << 4,
            MSR = 1 << 5,
            PAE = 1 << 6,
            MCE = 1 << 7,
            CX8 = 1 << 8,
            APIC = 1 << 9,
            Reserved1 = 1 << 10,
            SEP = 1 << 11,
            MTTR = 1 << 12,
            PGE = 1 << 13,
            MCA = 1 << 14,
            CMOV = 1 << 15,
            PAT = 1 << 16,
            PSE_36 = 1 << 17,
            PSN = 1 << 18,
            CLFSH = 1 << 19,
            Reserved2 = 1 << 20,
            DS = 1 << 21,
            ACPI = 1 << 22,
            MMX = 1 << 23,
            FXSR = 1 << 24,
            SSE = 1 << 25,
            SSE2 = 1 << 26,
            SS = 1 << 27,
            HTT = 1 << 28,
            TM = 1 << 29
          };
          uint32 flags;

        public:

          Info() {
            uint32 a[ 4 ];

            lib::cpuid( 0x02, a );

            flags = a[ 3 ];
          }

          static void print( lib::stream::Out& o );

      };

      Core cores[ 6 ];

      CPU();

      virtual ~CPU();
  };

}

#endif /* CPU_HPP_ */
