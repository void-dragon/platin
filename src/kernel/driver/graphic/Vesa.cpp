/**
 * Vesa.cpp
 *
 * @date 06.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Vesa.hpp"
#include <kernel/System.hpp>

namespace kernel {

  namespace driver {

    namespace graphic {

      void* getInfo() {
        int res = 0;
        asm volatile(
            "movl $0x4f00, %%eax;"
            "int $0x10;"
            : "=a"(res) );

        return ( void* ) res;
      }

      Vesa::Vesa() {
      }

    }

  }

}
