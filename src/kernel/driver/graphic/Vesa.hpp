/**
 * Vesa.hpp
 *
 * @date 06.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef VESA_HPP_
#define VESA_HPP_

#include <cpp.hpp>
#include <kernel/Process.hpp>
#include <lib/stream/Out.hpp>
#include <lib/sync/Mutex.hpp>

namespace kernel {

  namespace driver {

    namespace graphic {

      /**
       * @note http://wiki.osdev.org/VBE
       */
      class Vesa: public kernel::Process {
          struct p {
              char Signature[ 4 ];
              uint16 EntryPoint;
              uint16 PMInitialize;
              uint16 BIOSDataSel;
              uint16 A0000Sel;
              uint16 B0000Sel;
              uint16 B8000Sel;
              uint16 CodeSegSel;
              uint8 InProtectMode;
              uint8 Checksum;
          }__attribute__ ((packed));

          struct INFO {
              unsigned char VESASignature[ 4 ];
              unsigned short VESAVersion;
              unsigned long OEMStringPtr;
              unsigned char Capabilities[ 4 ];
              unsigned long VideoModePtr;
              unsigned short TotalMemory;
              unsigned short OemSoftwareRev;
              unsigned long OemVendorNamePtr;
              unsigned long OemProductNamePtr;
              unsigned long OemProductRevPtr;
              unsigned char Reserved[ 222 ];
              unsigned char OemData[ 256 ];
          }__attribute__ ((packed));

          struct ModeInfo {
              uint16 attributes;
              uint8 winA;
              uint8 winB;
              uint16 granularity;
              uint16 winsize;
              uint16 segmentA, segmentB;
              uint32 realFctPtr;
              uint16 pitch; // bytes per scanline

              uint16 Xres, Yres;
              uint8 Wchar, Ychar, planes, bpp, banks;
              uint8 memory_model, bank_size, image_pages;
              uint8 reserved0;

              uint8 red_mask, red_position;
              uint8 green_mask, green_position;
              uint8 blue_mask, blue_position;
              uint8 rsv_mask, rsv_position;
              uint8 directcolor_attributes;

              uint32 physbase; // your LFB address ;)
              uint32 reserved1;
              uint16 reserved2;
          }__attribute__ ((packed));

          INFO* info;

        public:
          Vesa();

      };

    }

  }

}

#endif /* VESA_HPP_ */
