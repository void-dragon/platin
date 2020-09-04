/**
 * AHCI.hpp
 *
 * @date 15.10.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef KERNEL_DRIVER_AHCI_HPP_
#define KERNEL_DRIVER_AHCI_HPP_

#include <lib/sync/Mutex.hpp>
#include <kernel/driver/PCI.hpp>

namespace kernel {
   namespace driver {

      /**
       * @note http://wiki.osdev.org/AHCI
       * @note Keep in mind a Sector in LBA is 512 Byte in size!
       */
      class AHCI {
            enum FIS_TYPE {
               FIS_TYPE_REG_H2D = 0x27, ///< Register FIS - host to device
               FIS_TYPE_REG_D2H = 0x34, ///< Register FIS - device to host
               FIS_TYPE_DMA_ACT = 0x39, ///< DMA activate FIS - device to host
               FIS_TYPE_DMA_SETUP = 0x41, ///< DMA setup FIS - bidirectional
               FIS_TYPE_DATA = 0x46, ///< Data FIS - bidirectional
               FIS_TYPE_BIST = 0x58, ///< BIST activate FIS - bidirectional
               FIS_TYPE_PIO_SETUP = 0x5F, ///< PIO setup FIS - device to host
               FIS_TYPE_DEV_BITS = 0xA1,
            ///< Set device bits FIS - device to host
            };

            struct FIS_REG_H2D {
                  // DWORD 0
                  uint8 fis_type;    ///< FIS_TYPE_REG_H2D

                  uint8 pmport :4;   ///< Port multiplier
                  uint8 rsv0 :3;     ///< Reserved
                  uint8 c :1;        ///< 1: Command, 0: Control

                  uint8 command;    ///< Command register
                  uint8 featurel;   ///< Feature register, 7:0

                  // DWORD 1
                  uint8 lba0;       ///< LBA low register, 7:0
                  uint8 lba1;       ///< LBA mid register, 15:8
                  uint8 lba2;       ///< LBA high register, 23:16
                  uint8 device;     ///< Device register

                  // DWORD 2
                  uint8 lba3;       ///< LBA register, 31:24
                  uint8 lba4;       ///< LBA register, 39:32
                  uint8 lba5;       ///< LBA register, 47:40
                  uint8 featureh;   ///< Feature register, 15:8

                  // DWORD 3
                  uint8 countl;    ///< Count register, 7:0
                  uint8 counth;    ///< Count register, 15:8
                  uint8 icc;       ///< Isochronous command completion
                  uint8 control;   ///< Control register

                  // DWORD 4
                  uint8 rsv1[ 4 ];   ///< Reserved
            };

            struct FIS_REG_D2H {
                  // DWORD 0
                  uint8 fis_type;    ///< FIS_TYPE_REG_D2H

                  uint8 pmport :4;   ///< Port multiplier
                  uint8 rsv0 :2;     ///< Reserved
                  uint8 i :1;        ///< Interrupt bit
                  uint8 rsv1 :1;     ///< Reserved

                  uint8 status;      ///< Status register
                  uint8 error;       ///< Error register

                  // DWORD 1
                  uint8 lba0;        ///< LBA low register, 7:0
                  uint8 lba1;        ///< LBA mid register, 15:8
                  uint8 lba2;        ///< LBA high register, 23:16
                  uint8 device;      ///< Device register

                  // DWORD 2
                  uint8 lba3;        ///< LBA register, 31:24
                  uint8 lba4;        ///< LBA register, 39:32
                  uint8 lba5;        ///< LBA register, 47:40
                  uint8 rsv2;        ///< Reserved

                  // DWORD 3
                  uint8 countl;      ///< Count register, 7:0
                  uint8 counth;      ///< Count register, 15:8
                  uint8 rsv3[ 2 ];   ///< Reserved

                  // DWORD 4
                  uint8 rsv4[ 4 ];   ///< Reserved
            };

            struct FIS_DATA {
                  // DWORD 0
                  uint8 fis_type;     ///< FIS_TYPE_DATA

                  uint8 pmport :4;   ///< Port multiplier
                  uint8 rsv0 :4;     ///< Reserved

                  uint8 rsv1[ 2 ];   ///< Reserved

                  // DWORD 1 ~ N
                  uint32 data[ 1 ];   ///< Payload
            };

            struct FIS_PIO_SETUP {
                  ///< DWORD 0
                  uint8 fis_type;   ///< FIS_TYPE_PIO_SETUP

                  uint8 pmport :4;   ///< Port multiplier
                  uint8 rsv0 :1;     ///< Reserved
                  uint8 d :1;        ///< Data transfer direction, 1 - device to host
                  uint8 i :1;        ///< Interrupt bit
                  uint8 rsv1 :1;

                  uint8 status;     ///< Status register
                  uint8 error;      ///< Error register

                  // DWORD 1
                  uint8 lba0;    ///< LBA low register, 7:0
                  uint8 lba1;    ///< LBA mid register, 15:8
                  uint8 lba2;    ///< LBA high register, 23:16
                  uint8 device;  ///< Device register

                  // DWORD 2
                  uint8 lba3;    ///< LBA register, 31:24
                  uint8 lba4;    ///< LBA register, 39:32
                  uint8 lba5;    ///< LBA register, 47:40
                  uint8 rsv2;    ///< Reserved

                  // DWORD 3
                  uint8 countl;     ///< Count register, 7:0
                  uint8 counth;     ///< Count register, 15:8
                  uint8 rsv3;       ///< Reserved
                  uint8 e_status;   ///< New value of status register

                  // DWORD 4
                  uint16 tc;       ///< Transfer count
                  uint8 rsv4[ 2 ]; ///< Reserved
            };
         public:
            AHCI( PCI::Device* dev );
            virtual ~AHCI();
      };

   } /* namespace driver */
} /* namespace kernel */

#endif /* KERNEL_DRIVER_AHCI_HPP_ */
